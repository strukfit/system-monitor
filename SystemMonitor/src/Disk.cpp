#include "Disk.h"

Disk::Disk(const char diskLetter) :
    m_diskLetter({ static_cast<wchar_t>(diskLetter), ':' }),
    m_diskModel(""),
    m_activeTime(0),
    m_readSpeed(0.f),
    m_writeSpeed(0.f),
    m_avgResponseTime(0.f),
    m_totalUsedBytes(0),
    m_totalBytes(0),
    m_totalFreeBytes(0)
{
	//updateInfo();
    pdhInit();
}

Disk::~Disk() 
{
}

void Disk::pdhInit()
{
    PdhOpenQuery(NULL, NULL, &m_hQuery);
    PdhAddEnglishCounter(m_hQuery, (L"\\LogicalDisk(" + m_diskLetter + L")\\Disk Read Bytes/sec").c_str(), NULL, &m_readCounter);
    PdhAddEnglishCounter(m_hQuery, (L"\\LogicalDisk(" + m_diskLetter + L")\\Disk Write Bytes/sec").c_str(), NULL, &m_writeCounter);
    PdhAddEnglishCounter(m_hQuery, (L"\\LogicalDisk(" + m_diskLetter + L")\\Avg. Disk sec/Transfer").c_str(), NULL, &m_responseTimeCounter);
    PdhCollectQueryData(m_hQuery);
}

void Disk::updateInfo()
{
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

    updateActiveTime();
}

void Disk::updateActiveTime()
{
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
}

std::wstring Disk::diskLetter() const
{
    return m_diskLetter;
}

byte Disk::activeTime() const
{
    return m_activeTime;
}

LONG Disk::readSpeed() const
{
    return m_readSpeed;
}

LONG Disk::writeSpeed() const
{
    return m_writeSpeed;
}

double Disk::avgResponseTime() const
{
    return m_avgResponseTime;
}

ULONGLONG Disk::totalUsedBytes() const
{
    return m_totalUsedBytes;
}

ULONGLONG Disk::totalBytes() const
{
    return m_totalBytes;
}

ULONGLONG Disk::totalFreeBytes() const
{
    return m_totalFreeBytes;
}
