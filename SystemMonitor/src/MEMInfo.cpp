#include "MEMInfo.h"

MEMInfo::MEMInfo() :
    m_total(0.f),
    m_avail(0.f),
    m_used(0.f),
    m_availPageFile(0.f),
    m_totalPageFile(0.f),
    m_usedPageFile(0.f)
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

    m_total = memStatus.ullTotalPhys;
    m_avail = memStatus.ullAvailPhys;
    m_used = m_total - m_avail;

    m_availPageFile = memStatus.ullAvailPageFile;
    m_totalPageFile = memStatus.ullTotalPageFile;
    m_usedPageFile = m_totalPageFile - m_availPageFile;
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

    m_total = totalMemoryKb * 1024;
    m_avail = availableMemoryKb * 1024;
    m_used = totalMemoryKb - availableMemoryKb;

    std::ifstream swapInfo("/proc/swaps");
    line = "";
    ulonglong totalSwapKb = 0, usedSwapKb = 0;

    while (std::getline(swapInfo, line)) {
        if (line.find("/dev") != std::string::npos) {
            std::istringstream iss(line);
            std::string dev, type;
            ulonglong size, used;
            iss >> dev >> type >> size >> used;
            totalSwapKb += size;
            usedSwapKb += used;
        }
    }
    swapInfo.close();

    m_totalPageFile = totalSwapKb * 1024;
    m_usedPageFile = usedSwapKb * 1024;
    m_availPageFile = m_totalPageFile - m_usedPageFile;
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

ulonglong MEMInfo::total() const
{
    return m_total;
}

ulonglong MEMInfo::avail() const
{
    return m_avail;
}

ulonglong MEMInfo::used() const
{
    return m_used;
}

ulonglong MEMInfo::availPageFile() const
{
    return m_availPageFile;
}

ulonglong MEMInfo::totalPageFile() const
{
    return m_totalPageFile;
}

ulonglong MEMInfo::usedPageFile() const
{
    return m_usedPageFile;
}

#ifdef _WIN32
int MEMInfo::speedMHz() const
{
    return m_speedMHz;
}
#endif // _WIN32
