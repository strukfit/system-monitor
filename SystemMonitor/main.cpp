#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QProcess>

#include <thread>
#include <sstream>
#include <iomanip>

#include "CPUInfo.h"
#include "MEMInfo.h"
#include "DiskInfo.h"
#include "GPUInfo.h"

std::string convertBytes(ulonglong bytes)
{
    const float KILOBYTE = 1024.f;
    const float MEGABYTE = 1024.f * KILOBYTE;
    const float GIGABYTE = 1024.f * MEGABYTE;
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);

    if (bytes < KILOBYTE)
    {
        oss << bytes << " B";
    }
    else if (bytes < MEGABYTE)
    {
        oss << bytes / KILOBYTE << " Kb";
    }
    else if (bytes < GIGABYTE)
    {
        oss << bytes / MEGABYTE << " Mb";
    }
    else
    {
        oss << bytes / GIGABYTE << " Gb";
    }

    return oss.str();
}

static void updateCPUAsync(CPUInfo& cpuInfo, QLabel* cpuLabel)
{
    cpuInfo.updateInfo();
    QString labelText = QString(
        "CPU_NAME: %1\n"
        "CPU_USAGE: %2\n"
        "CPU_PROCESSES: %3\n"
        "CPU_THREADS: %4\n"
        "CPU_HANDLES: %5\n"
        "CPU_BASE_SPEED: %6 GHz\n"
        "CPU_CORES: %7\n"
        "CPU_LOGIC_PROC: %8\n")
        .arg(QString::fromStdString(cpuInfo.modelName()))
        .arg(cpuInfo.usage())
        .arg(cpuInfo.processCount())
        .arg(cpuInfo.threadCount())
        .arg(cpuInfo.handleCount())
        .arg(cpuInfo.baseSpeed())
        .arg(cpuInfo.coreCount())
        .arg(cpuInfo.logicalProcessorCount());

    QMetaObject::invokeMethod(cpuLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, labelText));
}

static void updateMEMAsync(MEMInfo& memInfo, QLabel* memLabel)
{
    auto used = convertBytes(memInfo.used());
    auto total = convertBytes(memInfo.total());
    auto avail = convertBytes(memInfo.avail());

    auto usedPageFile = convertBytes(memInfo.usedPageFile());
    auto totalPageFile = convertBytes(memInfo.totalPageFile());
    auto availPageFile = convertBytes(memInfo.availPageFile());

    memInfo.updateInfo();
    QString labelText = QString(
        "MEM: %1/%2\n"
        "MEM_AVAIL: %3\n"
        "MEM_PAGE_FILE: %4/%5\n"
        "MEM_PAGE_FILE_AVAIL: %6\n")
        .arg(QString::fromStdString(used)).arg(QString::fromStdString(total))
        .arg(QString::fromStdString(avail))
        .arg(QString::fromStdString(usedPageFile)).arg(QString::fromStdString(totalPageFile))
        .arg(QString::fromStdString(availPageFile));

#ifdef _WIN32
    labelText.append(QString("MEM_SPEED: %1 MHz\n").arg(memInfo.speedMHz()));
#endif // _WIN32

    QMetaObject::invokeMethod(memLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, labelText));
}

static void updateGPUAsync(GPUInfo& gpuInfo, QLabel* gpuLabel)
{
    auto memoryUsed = convertBytes(gpuInfo.memoryUsed());
    auto memoryTotal = convertBytes(gpuInfo.memoryTotal());

    gpuInfo.updateInfo();
    QString labelText = QString(
        "GPU_NAME: %1\n"
        "GPU_USAGE: %2 %\n"
        "GPU_MEM_USAGE: %3/%4\n"
        "GPU_TEMPERATURE: %5 C\n\n")
        .arg(gpuInfo.modelName())
        .arg(gpuInfo.usage())
        .arg(QString::fromStdString(memoryUsed))
        .arg(QString::fromStdString(memoryTotal))
        .arg(gpuInfo.temperature());

    QMetaObject::invokeMethod(gpuLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, labelText));
}

static void updateDiskAsync(DiskInfo& diskInfo, QLabel* diskLabel)
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
        .arg(QString::fromStdString(convertBytes(diskInfo.totalUsedBytes())))
        .arg(QString::fromStdString(convertBytes(diskInfo.totalBytes())))
        .arg(QString::fromStdString(convertBytes(diskInfo.totalFreeBytes())))
        .arg(QString::fromStdString(convertBytes(diskInfo.readSpeed())))
        .arg(QString::fromStdString(convertBytes(diskInfo.writeSpeed())))
        .arg(diskInfo.avgResponseTime());

    QMetaObject::invokeMethod(diskLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, labelText));
}

#ifdef _WIN32
WMIManager wmiManager;
#endif

int updateIntervalMs = 1000;

QLabel* cpuLabel;
QLabel* memLabel;

CPUInfo cpuInfo;
MEMInfo memInfo;

std::vector<DiskInfo> allDisks;
std::vector<QLabel*> allDisksLabels;

#ifdef _WIN32
static ADLXHelper m_ADLXHelp;
#endif // _WIN32

std::vector<GPUInfo> allGPUs;
std::vector<QLabel*> allGPUsLabels;

void initDisks()
{
#ifdef _WIN32
    DWORD drivesMask = GetLogicalDrives();
    for (char letter = 'A'; letter <= 'Z'; ++letter)
    {
        // If the bit is set, the disk exists
        if (drivesMask & 1)
            allDisks.push_back(DiskInfo(letter));
        
        drivesMask >>= 1;
    }
#endif // _WIN32

#ifdef __linux__
  
    //FILE* fp;
    //char* kname[1035]
    
    // Open the command for reading.
    //fp = popen("lsblk -io KNAME --noheadings", "r");
    //if (fp == NULL) {
    //    printf("Failed to run command\n");
    //    return;
    //}

    //while (fgets(kname, sizeof(kname) - 2, fp) != NULL) {
    //    // Extract the disk kname from the line. 
    //    kname[strcspn(kname, "\n")] = 0;

    //    if(strstr(kname, "sd") != nullptr)
    //        allDisks.push_back(DiskInfo(kname));
    //}
    //pclose(fp);

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

void initNvidiaCards()
{
#ifdef _WIN32
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

        allGPUs.push_back(GPUInfo(static_cast<QString>(name), gpu::NVIDIA, device));
    }
#endif // _WIN32
    
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

void initAmdCards()
{
#ifdef _WIN32
    ADLX_RESULT  res = ADLX_FAIL;

    // Initialize ADLX
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

        allGPUs.push_back(GPUInfo(static_cast<QString>(gpuName), gpu::AMD, gpu, perfMonitoringServices));
    }
#endif // _WIN32
}

static void cleanGPUsLibs()
{
#ifdef _WIN32
    // Shutdown NVML library
    nvmlReturn_t result;
    result = nvmlShutdown();
    if (NVML_SUCCESS != result)
        qDebug() << "Failed to shutdown NVML: " << nvmlErrorString(result);

    // Destroy ADLX
    ADLX_RESULT res = ADLX_FAIL;
    res = m_ADLXHelp.Terminate();
    if (ADLX_FAILED(res))
        qDebug() << "Failed to terminate ADLX";
#endif // _WIN32
}

Q_SLOT void updateLabels()
{
    std::thread updateCPUThread(updateCPUAsync, std::ref(cpuInfo), cpuLabel);
    updateCPUThread.detach();

    std::thread updateMEMThread(updateMEMAsync, std::ref(memInfo), memLabel);
    updateMEMThread.detach();

    auto iGPU = allGPUs.begin();
    auto iGPULabel = allGPUsLabels.begin();
    for (; iGPU < allGPUs.end(); iGPU++, iGPULabel++)
    {
        std::thread updateGPUThread(updateGPUAsync, std::ref(*iGPU), *iGPULabel);
        updateGPUThread.detach();
    }

    auto iDisk = allDisks.begin();
    auto iDiskLabel = allDisksLabels.begin();
    for (; iDisk < allDisks.end(); iDisk++, iDiskLabel++)
    {
        std::thread updateDiskThread(updateDiskAsync, std::ref(*iDisk), *iDiskLabel);
        updateDiskThread.detach();
    }
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QMainWindow* window = new QMainWindow();

    auto centralWidget = new QWidget(window);

    auto layout = new QVBoxLayout(centralWidget);

    cpuLabel = new QLabel(nullptr, centralWidget);
    layout->addWidget(cpuLabel);

    memLabel = new QLabel(nullptr, centralWidget);
    layout->addWidget(memLabel);

    initNvidiaCards();
    initAmdCards();

    for (int i = 0; i < allGPUs.size(); i++)
    {
        auto gpuLabel = new QLabel(NULL, centralWidget);
        layout->addWidget(gpuLabel);
        allGPUsLabels.push_back(gpuLabel);
    }
    
#ifdef __linux__
    DiskInfo::setUpdateInterval(updateIntervalMs / 1000);
#endif // __linux__

    initDisks();

    for (int i = 0; i < allDisks.size(); i++)
    {
        auto diskLabel = new QLabel(NULL, centralWidget);
        layout->addWidget(diskLabel);
        allDisksLabels.push_back(diskLabel);
    }

    updateLabels();

    auto timer = new QTimer(centralWidget);
    QObject::connect(timer, &QTimer::timeout, &updateLabels);
    timer->start(updateIntervalMs);

    window->setCentralWidget(centralWidget);
    window->show();

    return app.exec();
}