#include "MEMInfo.h"

MEMInfo::MEMInfo() :
    m_totalGB(0.f),
    m_availGB(0.f),
    m_usedGB(0.f),
    m_availPageFileGB(0.f),
    m_totalPageFileGB(0.f),
    m_usedPageFileGB(0.f)
{
#ifdef _WIN32
    // Update constant info
    updateSpeedInfo();
#endif // _WIN32
}

MEMInfo::~MEMInfo()
{
}

void MEMInfo::updateInfo()
{
#ifdef _WIN32
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memStatus);

    m_totalGB = memStatus.ullTotalPhys / 1024.f / 1024.f / 1024.f;
    m_availGB = memStatus.ullAvailPhys / 1024.f / 1024.f / 1024.f;
    m_usedGB = m_totalGB - m_availGB;

    m_availPageFileGB = memStatus.ullAvailPageFile / 1024.f / 1024.f / 1024.f;
    m_totalPageFileGB = memStatus.ullTotalPageFile / 1024.f / 1024.f / 1024.f;
    m_usedPageFileGB = m_totalPageFileGB - m_availPageFileGB;
#endif // _WIN32

#ifdef __linux__
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    ulonglong totalMemoryKb = 0, availableMemoryKb = 0, usedMemoryKb = 0;

    while (std::getline(meminfo, line)) {
        if (line.compare(0, 8, "MemTotal") == 0) {
            totalMemoryKb = extractValue(line);
        }
        else if (line.compare(0, 12, "MemAvailable") == 0) {
            availableMemoryKb = extractValue(line);
        }
    }
    meminfo.close();

    usedMemoryKb = totalMemoryKb - availableMemoryKb;
    m_totalGB = totalMemoryKb / 1024.f / 1024.f;
    m_availGB = availableMemoryKb / 1024.f / 1024.f;
    m_usedGB = usedMemoryKb / 1024.f / 1024.f;

    std::ifstream swapInfo("/proc/swaps");
    line = "";
    ulonglong totalSwap = 0, usedSwap = 0;

    while (std::getline(swapInfo, line)) {
        if (line.find("/dev") != std::string::npos) {
            std::istringstream iss(line);
            std::string dev, type;
            ulonglong size, used;
            iss >> dev >> type >> size >> used;
            totalSwap += size;
            usedSwap += used;
        }
    }
    swapInfo.close();

    ulonglong availSwap = totalSwap - usedSwap;
    m_totalPageFileGB = totalSwap / 1024.f / 1024.f;
    m_usedPageFileGB = usedSwap / 1024.f / 1024.f;
    m_availPageFileGB = availSwap / 1024.f / 1024.f;
#endif // __linux__
}

#ifdef __linux__
ulonglong MEMInfo::extractValue(const std::string& line)
{
    std::string::size_type colon_pos = line.find(':');
    if (colon_pos == std::string::npos) {
        return 0; // Invalid format
    }
    return std::stoull(line.substr(colon_pos + 1));
}
#endif // __linux__


#ifdef _WIN32
void MEMInfo::updateSpeedInfo()
{
    std::wstring query = L"SELECT * FROM Win32_PhysicalMemory";
    std::wstring property = L"Speed";
    std::vector<WMIValue> results;
    WMIManager::execQuery(query, property, results);

    UINT speed = 0;
    for (const auto& result : results) {
        std::visit([&speed](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, long long>) {
                if (arg > speed) speed = arg;
            }
        }, result);
    }
    m_speedMHz = static_cast<int>(speed);
}
#endif // _WIN32

float MEMInfo::totalGB() const
{
    return m_totalGB;
}

float MEMInfo::availGB() const
{
    return m_availGB;
}

float MEMInfo::usedGB() const
{
    return m_usedGB;
}

float MEMInfo::availPageFileGB() const
{
    return m_availPageFileGB;
}

float MEMInfo::totalPageFileGB() const
{
    return m_totalPageFileGB;
}

float MEMInfo::usedPageFileGB() const
{
    return m_usedPageFileGB;
}

#ifdef _WIN32
int MEMInfo::speedMHz() const
{
    return m_speedMHz;
}
#endif // _WIN32
