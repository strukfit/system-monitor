#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
#ifdef WIN32
    wmiManager(),
#endif // WIN32
    updateIntervalMs(1000)
{
    auto centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: #161618;");
    auto layout = new QVBoxLayout(centralWidget);

    auto childWidget = new QWidget(centralWidget);
    childWidget->setContentsMargins(0, 0, 0, 0);

    auto childLayout = new QVBoxLayout(childWidget);
    childLayout->setContentsMargins(0, 0, 0, 0);

    auto scrollArea = new QScrollArea(childWidget);
    auto scrollBar = new CustomScrollBar(childWidget);
    scrollBar->setSingleStep(5);
    scrollArea->setVerticalScrollBar(scrollBar);
    scrollArea->setStyleSheet("border: 0;");
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* scrollAreaWidgetContents = new QWidget();

    auto widgetsLayout = new QVBoxLayout(scrollAreaWidgetContents);

#ifdef __linux__
    DiskInfo::setUpdateInterval(updateIntervalMs / 1000);
#endif // __linux__

    auto cpuInfoWidget = new CPUInfoWidget(childWidget);
    allWidgets.push_back(cpuInfoWidget);
    widgetsLayout->addWidget(cpuInfoWidget);
    
    auto memInfoWidget = new MEMInfoWidget(childWidget);
    allWidgets.push_back(memInfoWidget);
    widgetsLayout->addWidget(memInfoWidget);

    initNvidiaCards(childWidget, widgetsLayout);
    initAmdCards(childWidget, widgetsLayout);

    initDisks(childWidget, widgetsLayout);

    scrollArea->setWidget(scrollAreaWidgetContents);

    childLayout->addWidget(scrollArea);

    layout->addWidget(childWidget);

    updateWidgets();

    auto timer = new QTimer(centralWidget);
    QObject::connect(timer, &QTimer::timeout, this, &MainWindow::updateWidgets);
    timer->start(updateIntervalMs);

    this->setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
#ifdef WIN32
    // Shutdown NVML library
    nvmlReturn_t result;
    result = nvmlShutdown();
    if (NVML_SUCCESS != result)
        qDebug() << "Failed to shutdown NVML: " << nvmlErrorString(result);
#endif // WIN32
}

void MainWindow::initDisks(QWidget* parent, QLayout* layout)
{
#ifdef WIN32
    DWORD drivesMask = GetLogicalDrives();
    for (char letter = 'A'; letter <= 'Z'; ++letter)
    {
        // If the bit is set, the disk exists
        if (drivesMask & 1)
        {
            auto diskInfoWidget = new DiskInfoWidget(parent, letter);
            allWidgets.push_back(diskInfoWidget);
            layout->addWidget(diskInfoWidget);
        }

        drivesMask >>= 1;
    }
#endif // WIN32

#ifdef __linux__
    FILE* fp;
    char line[1024];
    fp = popen("df -h | grep /dev/", "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return;
    }

    while (fgets(line, sizeof(line) - 2, fp) != NULL) {
        // Extract the disk device name from the line. 
        std::istringstream iss(line);
        std::string device;
        iss >> device;

        if (device.find("sd") == std::string::npos)
            continue;

        if (device.substr(0, 5) == "/dev/")
            device = device.substr(5);

        auto diskInfoWidget = new DiskInfoWidget(parent, device);
        allWidgets.push_back(diskInfoWidget);
        layout->addWidget(diskInfoWidget);
    }
    pclose(fp);
#endif // __linux__
}

void MainWindow::initNvidiaCards(QWidget* parent, QLayout* layout)
{
#ifdef WIN32
    nvmlReturn_t result;

    // Initialize NVML library
    result = nvmlInit();
    if (NVML_SUCCESS != result) {
        qDebug() << "Failed to initialize NVML: " << nvmlErrorString(result);
        return;
    }

    unsigned int device_count;

    // Get the number of devices in the system
    result = nvmlDeviceGetCount(&device_count);
    if (NVML_SUCCESS != result) {
        qDebug() << "Failed to get device count: " << nvmlErrorString(result);
        nvmlShutdown();
        return;
    }

    // Loop over each device and get its name and handle
    for (unsigned int i = 0; i < device_count; ++i) {
        nvmlDevice_t device;
        char name[NVML_DEVICE_NAME_BUFFER_SIZE];

        // Get the handle for the device
        result = nvmlDeviceGetHandleByIndex(i, &device);
        if (NVML_SUCCESS != result) {
            qDebug() << "Failed to get handle for device " << i << ": " << nvmlErrorString(result);
            continue;
        }

        // Get the name of the device
        result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
        if (NVML_SUCCESS != result) {
            qDebug() << "Failed to get name for device " << i << ": " << nvmlErrorString(result);
            continue;
        }
        auto gpuInfoWidget = new GPUInfoWidget(parent, static_cast<QString>(name), gpu::NVIDIA, device);
        allWidgets.push_back(gpuInfoWidget);
        layout->addWidget(gpuInfoWidget);
    }
#endif // WIN32

#ifdef __linux__
    FILE* fp;
    char index[1035];

    // Open the command for reading.
    fp = popen("nvidia-smi --query-gpu=index --format=csv,noheader", "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return;
    }

    // Read the output a line at a time - each line should contain one GPU index. 
    while (fgets(index, sizeof(index) - 1, fp) != NULL) {
        // Extract the GPU index from the line. 
        allGPUs.push_back(GPUInfo(index, gpu::NVIDIA));
    }
    pclose(fp);
#endif // __linux__
}

void MainWindow::initAmdCards(QWidget* parent, QLayout* layout)
{
#ifdef WIN32
    ADLX_RESULT res = ADLX_FAIL;

    // Initialize ADLX
    m_ADLXHelp = ADLXHelper();
    res = m_ADLXHelp.Initialize();
    if (ADLX_FAILED(res))
    {
        qDebug() << "Failed to initialize ADLX";
        return;
    }

    // Get Performance Monitoring services
    IADLXPerformanceMonitoringServicesPtr perfMonitoringServices;
    res = m_ADLXHelp.GetSystemServices()->GetPerformanceMonitoringServices(&perfMonitoringServices);
    if (ADLX_FAILED(res))
    {
        qDebug() << "Failed to get performance monitoring services";
        return;
    }

    // Get GPU list
    IADLXGPUListPtr gpus;
    res = m_ADLXHelp.GetSystemServices()->GetGPUs(&gpus);
    if (ADLX_FAILED(res))
    {
        qDebug() << "Failed to get GPU list";
        return;
    }

    for (auto it = gpus->Begin(); it < gpus->End(); it++)
    {
        IADLXGPUPtr gpu;
        res = gpus->At(it, &gpu);
        if (ADLX_FAILED(res))
        {
            qDebug() << "Failed to get particular GPU";
            continue;
        }

        const char* gpuName = nullptr;
        gpu->Name(&gpuName);

        auto gpuInfoWidget = new GPUInfoWidget(parent, static_cast<QString>(gpuName), gpu::AMD, gpu, perfMonitoringServices);
        allWidgets.push_back(gpuInfoWidget);
        layout->addWidget(gpuInfoWidget);
    }
#endif // WIN32
}

void MainWindow::updateWidgets()
{
    for (auto& widget : allWidgets)
    {
        std::thread updateWidgetThread(&InfoWidget::updateInfo, widget);
        updateWidgetThread.detach();
    }
}