#include "MainWindow.h"

DiskChartView* MainWindow::diskChartView;
CustomChartView* MainWindow::diskSpeedChartView;

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

    initDisks();

    for (int i = 0; i < allDisks.size(); i++)
    {
        auto diskLabel = new QLabel(NULL, childWidget);
        widgetsLayout->addWidget(diskLabel);
        allDisksLabels.push_back(diskLabel);
    }

    auto cpuInfoWidget = new CPUInfoWidget(childWidget);
    allWidgets.push_back(cpuInfoWidget);
    widgetsLayout->addWidget(cpuInfoWidget);
    
    auto memInfoWidget = new MEMInfoWidget(childWidget);
    allWidgets.push_back(memInfoWidget);
    widgetsLayout->addWidget(memInfoWidget);

    initNvidiaCards(childWidget, widgetsLayout);
    initAmdCards(childWidget, widgetsLayout);

    diskChartView = new DiskChartView(childWidget);
    diskChartView->setMinimumHeight(500);
    widgetsLayout->addWidget(diskChartView);

    diskSpeedChartView = new CustomChartView(
        childWidget,
        0, 60, "\n",
        0, 500, "Write/read speed, Kb/s");
    diskSpeedChartView->setMinimumHeight(500);
    widgetsLayout->addWidget(diskSpeedChartView);

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

void MainWindow::initDisks()
{
#ifdef WIN32
    DWORD drivesMask = GetLogicalDrives();
    for (char letter = 'A'; letter <= 'Z'; ++letter)
    {
        // If the bit is set, the disk exists
        if (drivesMask & 1)
            allDisks.push_back(DiskInfo(letter));

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

        allDisks.push_back(DiskInfo(device));
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

void MainWindow::updateDiskAsync(DiskInfo& diskInfo, QLabel* diskLabel)
{
    diskInfo.updateInfo();
    QString labelText = QString(
        "DISK: %1\n"
        "DISK_NAME: %2\n"
        "DISK_ACTIVE_TIME: %3 %\n"
        "DISK_USAGE: %4/%5\n"
        "DISK_FREE_SPACE: %6\n"
        "DISK_READ_SPEED: %7/s\n"
        "DISK_WRITE_SPEED: %8/s\n"
        "DISK_AVG_RESPONSE_TIME: %9 ms\n\n")
#ifdef WIN32
        .arg(QString::fromStdWString(diskInfo.diskLetter()))
#elif __linux__
        .arg(QString::fromStdString(diskInfo.device()))
#else
        .arg("");
#endif 
    .arg(QString::fromStdWString(diskInfo.modelName()))
        .arg(diskInfo.activeTime())
        .arg(QString::fromStdString(Converter::convertBytes(diskInfo.totalUsedBytes())))
        .arg(QString::fromStdString(Converter::convertBytes(diskInfo.totalBytes())))
        .arg(QString::fromStdString(Converter::convertBytes(diskInfo.totalFreeBytes())))
        .arg(QString::fromStdString(Converter::convertBytes(diskInfo.readSpeed())))
        .arg(QString::fromStdString(Converter::convertBytes(diskInfo.writeSpeed())))
        .arg(diskInfo.avgResponseTime());

    QMetaObject::invokeMethod(diskLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, labelText));
}

void MainWindow::updateWidgets()
{
    /*
    std::thread updateCPUThread(&CPUInfoWidget::updateInfo, cpuInfoWidget);
    updateCPUThread.detach();

    std::thread updateMEMThread(&MEMInfoWidget::updateInfo, memInfoWidget);
    updateMEMThread.detach();

    auto iGPU = allGPUs.begin();
    auto iGPULabel = allGPUsLabels.begin();
    for (; iGPU < allGPUs.end(); iGPU++, iGPULabel++)
    {
        std::thread updateGPUThread(updateGPUAsync, std::ref(*iGPU), *iGPULabel);
        updateGPUThread.detach();
    }*/

    for (auto& widget : allWidgets)
    {
        std::thread updateWidgetThread(&InfoWidget::updateInfo, widget);
        updateWidgetThread.detach();
    }

    auto iDisk = allDisks.begin();
    auto iDiskLabel = allDisksLabels.begin();
    for (; iDisk < allDisks.end(); iDisk++, iDiskLabel++)
    {
        std::thread updateDiskThread(updateDiskAsync, std::ref(*iDisk), *iDiskLabel);
        updateDiskThread.detach();
    }

    diskChartView->updateSpace(allDisks[0].totalFreeBytes(), allDisks[0].totalUsedBytes(), allDisks[0].totalBytes());
    
    QString writeSpeed = QString::fromStdString(Converter::convertBytes(allDisks[0].writeSpeed()));
    QString readSpeed = QString::fromStdString(Converter::convertBytes(allDisks[0].readSpeed()));
    QString labelText = QString("Write speed: %1\nRead speed: %2").arg(writeSpeed).arg(readSpeed);
    diskSpeedChartView->append(allDisks[0].writeSpeed() / 1024., allDisks[0].readSpeed() / 1024.);
}