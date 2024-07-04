#include "DiskInfo.h"

DiskInfo::DiskInfo(const char diskLetter) :
    m_diskLetter({ static_cast<wchar_t>(diskLetter), ':' }),
    m_modelName(L""),
    m_activeTime(0),
    m_readSpeed(0.f),
    m_writeSpeed(0.f),
    m_avgResponseTime(0.f),
    m_totalUsedBytes(0),
    m_totalBytes(0),
    m_totalFreeBytes(0)
{
#ifdef _WIN32
    pdhInit();
#endif
    // Update constant info
    updateModelName();
}

DiskInfo::~DiskInfo()
{
}

#ifdef _WIN32
void DiskInfo::pdhInit()
{
    PdhOpenQuery(NULL, NULL, &m_hQuery);
    PdhAddEnglishCounter(m_hQuery, (L"\\LogicalDisk(" + m_diskLetter + L")\\Disk Read Bytes/sec").c_str(), NULL, &m_readCounter);
    PdhAddEnglishCounter(m_hQuery, (L"\\LogicalDisk(" + m_diskLetter + L")\\Disk Write Bytes/sec").c_str(), NULL, &m_writeCounter);
    PdhAddEnglishCounter(m_hQuery, (L"\\LogicalDisk(" + m_diskLetter + L")\\Avg. Disk sec/Transfer").c_str(), NULL, &m_responseTimeCounter);
    PdhCollectQueryData(m_hQuery);
}
#endif // _WIN32


void DiskInfo::updateInfo()
{
#ifdef _WIN32
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    GetDiskFreeSpaceEx(m_diskLetter.c_str(), &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);

    m_totalBytes = totalNumberOfBytes.QuadPart;
    m_totalFreeBytes = totalNumberOfFreeBytes.QuadPart;
    m_totalUsedBytes = m_totalBytes - m_totalFreeBytes;

    PDH_FMT_COUNTERVALUE readSpeedVal, writeSpeedVal, avgResponseTimeVal;

    PdhCollectQueryData(m_hQuery);
    PdhGetFormattedCounterValue(m_readCounter, PDH_FMT_LONG, NULL, &readSpeedVal);
    PdhGetFormattedCounterValue(m_writeCounter, PDH_FMT_LONG, NULL, &writeSpeedVal);
    PdhGetFormattedCounterValue(m_responseTimeCounter, PDH_FMT_DOUBLE, NULL, &avgResponseTimeVal);

    m_readSpeed = readSpeedVal.longValue;
    m_writeSpeed = writeSpeedVal.longValue;
    m_avgResponseTime = avgResponseTimeVal.doubleValue;
#endif // _WIN32

    updateActiveTime();
}

void DiskInfo::updateActiveTime()
{
#ifdef _WIN32
    std::wstring query = L"SELECT PercentDiskTime FROM Win32_PerfFormattedData_PerfDisk_LogicalDisk WHERE Name = \'" + m_diskLetter + L"\'";
    std::wstring property = L"PercentDiskTime";
    std::vector<WMIValue> results;

    WMIManager::execQuery(query, property, results);

    if (results.empty())
    {
        m_activeTime = 0;
        return;
    }

    for (const auto& result : results)
    {
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::wstring>)
            {
                m_activeTime = std::stoi(arg);
            }
            else {
                m_activeTime = arg;
            }
            }, result);
    }

    if (m_activeTime > 100)
        m_activeTime = 100;
#endif // _WIN32
}

void DiskInfo::updateModelName()
{
#ifdef _WIN32
    std::wstring query = L"ASSOCIATORS OF {Win32_LogicalDisk.DeviceID='" + m_diskLetter + L"'} WHERE AssocClass=Win32_LogicalDiskToPartition";
    std::wstring property = L"DeviceId";
    std::vector<WMIValue> results;

    WMIManager::execQuery(query, property, results);

    if (results.empty())
    {
        m_modelName = L"";
        return;
    }

    std::wstring deviceId;
    for (const auto& result : results)
    {
        std::visit([&deviceId](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::wstring>)
            {
                //m_modelName = arg;
                deviceId = arg;
            }
        }, result);
    }

    
    query = L"ASSOCIATORS OF {Win32_DiskPartition.DeviceID='" + deviceId + L"'} WHERE AssocClass=Win32_DiskDriveToDiskPartition";
    property = L"Model";
    results.clear();

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
#endif // _WIN32
}

std::wstring DiskInfo::diskLetter() const
{
    return m_diskLetter;
}

std::wstring DiskInfo::modelName() const
{
    return m_modelName;
}

byte DiskInfo::activeTime() const
{
    return m_activeTime;
}

long DiskInfo::readSpeed() const
{
    return m_readSpeed;
}

long DiskInfo::writeSpeed() const
{
    return m_writeSpeed;
}

double DiskInfo::avgResponseTime() const
{
    return m_avgResponseTime;
}

ulonglong DiskInfo::totalUsedBytes() const
{
    return m_totalUsedBytes;
}

ulonglong DiskInfo::totalBytes() const
{
    return m_totalBytes;
}

ulonglong DiskInfo::totalFreeBytes() const
{
    return m_totalFreeBytes;
}
