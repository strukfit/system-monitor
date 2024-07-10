#include "Info/CPUInfo.h"

CPUInfo::CPUInfo():
#ifdef __unix__
    m_prevIdleTime(0),
    m_prevTotalTime(0),
#endif
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
    // Update constant variables
    updateCPUCoreCount();
    updateCPUBaseSpeed();
    updateCPUModelName();
#endif // _WIN32
#ifdef __unix__
    updateConstantVariables();
#endif // __unix__
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
#ifdef _WIN32
    updatePDHInfo();
#endif // _WIN32
#ifdef __unix__
    updateProcessThreadHandleCount();
    updateCPUUsage();
#endif // __unix__
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

void CPUInfo::updatePDHInfo()
{
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
}

void CPUInfo::updateCPUCoreCount() 
{
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

void CPUInfo::updateCPUModelName()
{
    std::wstring query = L"SELECT Name FROM Win32_Processor";
    std::wstring property = L"Name";
    std::vector<WMIValue> results;

    WMIManager::execQuery(query, property, results);

    if (results.empty())
    {
        m_modelName = "";
        return;
    }

    for (const auto& result : results)
    {
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::wstring>)
            {
                m_modelName = std::string(arg.begin(), arg.end());
            }
        }, result);
    }
}
#endif

#ifdef __unix__
void CPUInfo::updateConstantVariables()
{
    std::ifstream file("/proc/cpuinfo");
    std::stringstream content;
    content << file.rdbuf();
    file.close();

    std::istringstream iss(content.str());
    std::string line;
    std::set<int> cores;
    while (getline(iss, line)) {
        // Get model name
        if (line.find("model name") != std::string::npos)
        {
            m_modelName = line.substr(line.find(":") + 2);
        }
        // Get base speed
        else if (line.find("cpu MHz") != std::string::npos)
        {
            m_baseSpeed = stod(line.substr(line.find(":") + 2));
        }
        // Calculate cores
        else if (line.find("core id") != std::string::npos)
        {
            int coreId;
            std::sscanf(line.c_str(), "core id\t: %d", &coreId);
            cores.insert(coreId);
        }
    }

    // Get cores count
    m_coreCount = cores.size();

    // Get logical processors count
    m_logicalProcessorCount = sysconf(_SC_NPROCESSORS_CONF);
}

void CPUInfo::updateProcessThreadHandleCount()
{
    m_processCount = 0;
    m_threadCount = 0;
    m_handleCount = 0;
    for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
        if (entry.is_directory()) {
            std::string pid = entry.path().filename().string();
            if (std::all_of(pid.begin(), pid.end(), ::isdigit)) {
                // Calculate proces count 
                m_processCount++;

                // Calculate thread count
                std::ifstream status(entry.path() / "status");
                std::string line;
                while (std::getline(status, line)) {
                    if (line.find("Threads:") != std::string::npos) {
                        int threads;
                        sscanf(line.c_str(), "Threads: %d", &threads);
                        m_threadCount += threads;
                        break;
                    }
                }
                status.close();

                // Calculate handle count
                std::string fd_path = "/proc/" + pid + "/fd";
                try {
                    for (const auto& fd_entry : std::filesystem::directory_iterator(fd_path)) {
                        m_handleCount++;
                    }
                }
                catch (const std::filesystem::filesystem_error& e) {
                    // Ignore permission errors and continue
                    continue;
                }
            }
        }
    }
}

void CPUInfo::getCPUTime(ulonglong& idleTime, ulonglong& totalTime)
{
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    file.close();

    std::istringstream ss(line);
    std::string cpu;
    ulonglong user, nice, system, idle, iowait, irq, softirq, steal;

    ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    totalTime = user + nice + system + idle + iowait + irq + softirq + steal;
    idleTime = user + nice + system;
}

void CPUInfo::updateCPUUsage()
{
    ulonglong idleTime, totalTime;
    getCPUTime(idleTime, totalTime);

    ulonglong idleDiff = idleTime - m_prevIdleTime;
    ulonglong totalDiff = totalTime - m_prevTotalTime;

    m_usage = (static_cast<double>(idleDiff) / totalDiff) * 100.0;

    m_prevIdleTime = idleTime;
    m_prevTotalTime = totalTime;
}
#endif // __unix__

std::string CPUInfo::modelName() const
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

ulong CPUInfo::baseSpeed() const
{
    return m_baseSpeed;
}

ulong CPUInfo::coreCount() const
{
    return m_coreCount;
}

ulong CPUInfo::logicalProcessorCount() const
{
    return m_logicalProcessorCount;
}