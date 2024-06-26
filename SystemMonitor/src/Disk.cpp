#include "Disk.h"

Disk::Disk(const char diskLetter, std::shared_ptr<WMIManager> p_WMIManager) :
    m_diskLetter({ static_cast<wchar_t>(diskLetter), ':' }),
    m_diskModel(""),
    m_activeTime(0),
    m_readSpeed(0.f),
    m_writeSpeed(0.f),
    m_avgResponseTime(0.f),
    m_totalUsedBytes(0),
    m_totalBytes(0),
    m_totalFreeBytes(0),
    m_WMIManager(p_WMIManager)
{
	updateInfo();
}

Disk::~Disk() 
{
	PdhCloseQuery(m_hQuery);
}

void Disk::updateInfo()
{
	ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
	GetDiskFreeSpaceEx(m_diskLetter.c_str(), &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);

	m_totalBytes = totalNumberOfBytes.QuadPart;
	m_totalFreeBytes = totalNumberOfFreeBytes.QuadPart;
	m_totalUsedBytes = m_totalBytes - m_totalFreeBytes;

	updateActiveTime();
}

void Disk::updateActiveTime()
{
    std::wstring query = L"SELECT * FROM Win32_PerfFormattedData_PerfDisk_LogicalDisk WHERE Name = \'" + m_diskLetter + L"\'";
    std::wstring property = L"PercentDiskTime";
    std::vector<WMIValue> results; 
    m_WMIManager->execQuery(query, property, results);
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (
            std::is_same_v<T, std::wstring>)
        {
            m_activeTime = std::stoi(arg);
        }
        else {
            m_activeTime = arg;
        }
        }, results[0]);

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

float Disk::readSpeed() const
{
    return m_readSpeed;
}

float Disk::writeSpeed() const
{
    return m_writeSpeed;
}

float Disk::avgResponseTime() const
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
