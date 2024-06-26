#include "CPUInfo.h"

CPUInfo::CPUInfo():
    m_usage(0.),
    m_processCount(0),
    m_threadCount(0),
    m_handleCount(0),
    m_baseSpeed(0),
    m_coreCount(0),
    m_logicalProcessorCount(0)
{
    pdhInit();
    updateInfo();
}

CPUInfo::~CPUInfo()
{
    PdhCloseQuery(&m_hQuery);
}

void CPUInfo::updateInfo()
{
    updateCPUInfo();
    updateCPUCoreCount();
    updateCPUBaseSpeed();
}

double CPUInfo::usage() const
{
    return m_usage;
}

LONG CPUInfo::processCount() const
{
    return m_processCount;
}

LONG CPUInfo::threadCount() const
{
    return m_threadCount;
}

LONG CPUInfo::handleCount() const
{
    return m_handleCount;
}

DWORD CPUInfo::baseSpeed() const
{
    return m_baseSpeed;
}

DWORD CPUInfo::coreCount() const
{
    return m_coreCount;
}

DWORD CPUInfo::logicalProcessorCount() const
{
    return m_logicalProcessorCount;
}

void CPUInfo::pdhInit()
{
    PdhOpenQuery(NULL, NULL, &m_hQuery);
    PdhAddEnglishCounter(m_hQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &m_cpuTotal);
    PdhAddEnglishCounter(m_hQuery, L"\\System\\Processes", NULL, &m_processCounter);
    PdhAddEnglishCounter(m_hQuery, L"\\System\\Threads", NULL, &m_threadCounter);
    PdhAddEnglishCounter(m_hQuery, L"\\Process(_Total)\\Handle Count", NULL, &m_handleCounter);
    PdhCollectQueryData(m_hQuery);
}

void CPUInfo::updateCPUInfo()
{
    PDH_FMT_COUNTERVALUE usageVal, processVal, threadVal, handleVal;

    PdhCollectQueryData(m_hQuery);
    PdhGetFormattedCounterValue(m_cpuTotal, PDH_FMT_DOUBLE, NULL, &usageVal);
    PdhGetFormattedCounterValue(m_processCounter, PDH_FMT_LONG, NULL, &processVal);
    PdhGetFormattedCounterValue(m_threadCounter, PDH_FMT_LONG, NULL, &threadVal);
    PdhGetFormattedCounterValue(m_handleCounter, PDH_FMT_LONG, NULL, &handleVal);

    m_usage = usageVal.doubleValue;
    m_processCount = processVal.longValue;
    m_threadCount = threadVal.longValue;
    m_handleCount = handleVal.longValue;
}

void CPUInfo::updateCPUCoreCount() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    m_logicalProcessorCount = sysInfo.dwNumberOfProcessors;

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
    m_coreCount = processorCount;
}

void CPUInfo::updateCPUBaseSpeed() {
    HKEY hKey;
    LONG lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),
        0,
        KEY_READ,
        &hKey);

    DWORD mhz = 0;
    if (lError == ERROR_SUCCESS) {
        DWORD mhzSize = sizeof(mhz);
        RegQueryValueEx(hKey, TEXT("~MHz"), NULL, NULL, (LPBYTE)&mhz, &mhzSize);
        RegCloseKey(hKey);
    }
    m_baseSpeed = mhz / 1000.f;
}
