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
#ifdef _WIN32
    pdhInit();
#endif // _WIN32
    // Update constant variables
    updateCPUCoreCount();
    updateCPUBaseSpeed();
    updateCPUModelName();
}

CPUInfo::~CPUInfo()
{
#ifdef _WIN32
    PdhCloseQuery(&m_hQuery);
#endif // _WIN32
}

void CPUInfo::updateInfo()
{
    // Update inconstant variables
    updateCPUInfo();
}

#ifdef _WIN32
void CPUInfo::pdhInit()
{
    PdhOpenQuery(NULL, NULL, &m_hQuery);
    PdhAddEnglishCounter(m_hQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &m_cpuTotal);
    PdhAddEnglishCounter(m_hQuery, L"\\System\\Processes", NULL, &m_processCounter);
    PdhAddEnglishCounter(m_hQuery, L"\\System\\Threads", NULL, &m_threadCounter);
    PdhAddEnglishCounter(m_hQuery, L"\\Process(_Total)\\Handle Count", NULL, &m_handleCounter);
    PdhCollectQueryData(m_hQuery);
}
#endif // _WIN32


void CPUInfo::updateCPUInfo()
{
#ifdef _WIN32
    PDH_FMT_COUNTERVALUE usageVal, processVal, threadVal, handleVal;

    PdhCollectQueryData(m_hQuery);
    PdhGetFormattedCounterValue(m_cpuTotal, PDH_FMT_DOUBLE, NULL, &usageVal);
    PdhGetFormattedCounterValue(m_processCounter, PDH_FMT_LONG, NULL, &processVal);
    PdhGetFormattedCounterValue(m_threadCounter, PDH_FMT_LONG, NULL, &threadVal);
    PdhGetFormattedCounterValue(m_handleCounter, PDH_FMT_LONG, NULL, &handleVal);

    m_usage = usageVal.doubleValue;
    m_processCount = static_cast<int>(processVal.longValue);
    m_threadCount = static_cast<int>(threadVal.longValue);
    m_handleCount = static_cast<int>(handleVal.longValue);
#endif // _WIN32

}

void CPUInfo::updateCPUCoreCount() {
#ifdef _WIN32
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
#endif
}

void CPUInfo::updateCPUBaseSpeed() {
#ifdef _WIN32
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
#endif // _WIN32
}

void CPUInfo::updateCPUModelName()
{
#ifdef _WIN32
    std::wstring query = L"SELECT Name FROM Win32_Processor";
    std::wstring property = L"Name";
    std::vector<WMIValue> results;

    WMIManager::execQuery(query, property, results);

    if (results.empty())
    {
        m_modelName = L"";
        return;
    }

    for (const auto& result : results)
    {
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::wstring>)
            {
                m_modelName = arg;
            }
        }, result);
    }
#endif
}

std::wstring CPUInfo::modelName() const
{
    return m_modelName;
}

double CPUInfo::usage() const
{
    return m_usage;
}

int CPUInfo::processCount() const
{
    return m_processCount;
}

int CPUInfo::threadCount() const
{
    return m_threadCount;
}

int CPUInfo::handleCount() const
{
    return m_handleCount;
}

unsigned long CPUInfo::baseSpeed() const
{
    return m_baseSpeed;
}

unsigned long CPUInfo::coreCount() const
{
    return m_coreCount;
}

unsigned long CPUInfo::logicalProcessorCount() const
{
    return m_logicalProcessorCount;
}