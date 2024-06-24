#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QMessageBox>
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "wbemuuid.lib")
//#else
//#include <sys/sysinfo.h>
//#include <fstream>
#endif

QLabel* cpuLabel;
QLabel* memLabel;
QLabel* diskLabel;

static PDH_HQUERY cpuQuery;
PDH_HCOUNTER cpuTotal, processCounter, threadCounter, handleCounter;

void pdhInit()
{
    PdhOpenQuery(NULL, NULL, &cpuQuery);
    PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhAddEnglishCounter(cpuQuery, TEXT("\\System\\Processes"), NULL, &processCounter);
    PdhAddEnglishCounter(cpuQuery, TEXT("\\System\\Threads"), NULL, &threadCounter);
    PdhAddEnglishCounter(cpuQuery, TEXT("\\Process(_Total)\\Handle Count"), NULL, &handleCounter);
    PdhCollectQueryData(cpuQuery);

    // PdhCloseQuery(&cpuQuery);
}

double getCPUUsage() 
{
    PDH_FMT_COUNTERVALUE usageVal;

    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &usageVal);

    return usageVal.doubleValue;
}

DWORD baseSpeed;

LONG getCPUProcessCount()
{
    PDH_FMT_COUNTERVALUE processVal, threadVal, handleVal;
    
    PdhCollectQueryData(cpuQuery);

    PdhGetFormattedCounterValue(processCounter, PDH_FMT_LONG, NULL, &processVal);
    PdhGetFormattedCounterValue(threadCounter, PDH_FMT_LONG, NULL, &threadVal);
    PdhGetFormattedCounterValue(handleCounter, PDH_FMT_LONG, NULL, &handleVal);

    return processVal.longValue;
}

LONG getCPUThreadCount()
{
    PDH_FMT_COUNTERVALUE threadVal;

    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(threadCounter, PDH_FMT_LONG, NULL, &threadVal);

    return threadVal.longValue;
}

LONG getCPUHandleCount()
{
    PDH_FMT_COUNTERVALUE handleVal;

    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(handleCounter, PDH_FMT_LONG, NULL, &handleVal);

    return handleVal.longValue;
}

struct CPUCoreInfo
{
    DWORD coresCount;
    DWORD logicalProcessorsCount;
} cpuCoreInfo;

void CPUCoreCountUpdate() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    cpuCoreInfo.logicalProcessorsCount = sysInfo.dwNumberOfProcessors;

    DWORD length = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &length);
    std::vector<char> buffer(length);
    auto info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data());
    if (!GetLogicalProcessorInformationEx(RelationProcessorCore, info, &length)) {
        return;
    }

    int processorCount = 0;
    while (length > 0) {
        if (info->Relationship == RelationProcessorCore) {
            ++processorCount;
        }
        length -= info->Size;
        info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(reinterpret_cast<char*>(info) + info->Size);
    }
    cpuCoreInfo.coresCount = processorCount;
}

void CPUBaseSpeedUpdate() {
    HKEY hKey;
    LONG lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),
        0,
        KEY_READ,
        &hKey);

    DWORD mhz;
    if (lError == ERROR_SUCCESS) {
        DWORD mhzSize = sizeof(mhz);
        RegQueryValueEx(hKey, TEXT("~MHz"), NULL, NULL, (LPBYTE)&mhz, &mhzSize);
        RegCloseKey(hKey);
    }
    baseSpeed = mhz;
}

double getCPUSpeed() 
{
    return 0;
}

UINT getCPUTemperature() 
{
    return 0;
}

MEMORYSTATUSEX getMemoryStatus() 
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return memInfo;
}

ULARGE_INTEGER getDiskSpace(LPCWSTR drive) 
{
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    GetDiskFreeSpaceEx(drive, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
    return totalNumberOfBytes;
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QMainWindow* window = new QMainWindow();

    auto centralWidget = new QWidget(window);

    auto layout = new QVBoxLayout(centralWidget);

    cpuLabel = new QLabel(NULL, centralWidget);
    layout->addWidget(cpuLabel);

    memLabel = new QLabel(NULL, centralWidget);
    layout->addWidget(memLabel);

    diskLabel = new QLabel(NULL, centralWidget);
    layout->addWidget(diskLabel);

    pdhInit();
    CPUBaseSpeedUpdate();
    CPUCoreCountUpdate();
    
    auto timer = new QTimer(centralWidget);
    QObject::connect(timer, &QTimer::timeout, [&] {
        double cpuUsage = getCPUUsage();
        double cpuFreq = getCPUSpeed();
        UINT cpuTempr = getCPUTemperature();
        MEMORYSTATUSEX memStatus = getMemoryStatus();
        ULARGE_INTEGER totalDiskUsage = getDiskSpace(L"C:");
        LONG processCount = getCPUProcessCount();
        LONG threadCount = getCPUThreadCount();
        LONG handleCount = getCPUHandleCount();

        cpuLabel->setText(QString(
            "CPU_USAGE: %1\n"
            "CPU_CURR_SPEED: %2 HHz\n"
            "CPU_BASE_SPEED: %3 HHz\n"
            "CPU_TEMP: %4 °C\n"
            "CPU_PROCESSES: %5\n"
            "CPU_THREADS: %6\n"
            "CPU_HANDLES: %7\n"
            "CPU_CORES: %8\n"
            "CPU_LOGIC_PROC: %9\n")
                .arg(cpuUsage)
                .arg(cpuFreq/1000.f)
                .arg(baseSpeed/1000.f)
                .arg(cpuTempr/10.f - 273.15f)
                .arg(processCount)
                .arg(threadCount)
                .arg(handleCount)
                .arg(cpuCoreInfo.coresCount)
                .arg(cpuCoreInfo.logicalProcessorsCount));
        
        auto memUsedGB = (memStatus.ullTotalPhys - memStatus.ullAvailPhys) / 1024.f / 1024.f / 1024.f;
        auto memTotalGB = memStatus.ullTotalPhys / 1024.f / 1024.f / 1024.f;

        memLabel->setText(QString("MEM: %1/%2 GB").arg(memUsedGB).arg(memTotalGB));
        
        diskLabel->setText(QString("DISK_TOTAL: %1").arg(totalDiskUsage.QuadPart / 1024.f / 1024.f / 1024.f));
    });
    timer->start(1000);

    window->setCentralWidget(centralWidget);
    window->show();

    return app.exec();
}