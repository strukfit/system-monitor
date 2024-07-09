#include "DiskInfo.h"

#ifdef _WIN32
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
    pdhInit();
    // Update constant info
    updateModelName();
}
#endif

#ifdef __linux__
uint DiskInfo::m_updateIntervalS = 1;

DiskInfo::DiskInfo(std::string device):
    m_device(device),
    m_modelName(L""),
    m_activeTime(0),
    m_readSpeed(0.f),
    m_writeSpeed(0.f),
    m_avgResponseTime(0.f),
    m_totalUsedBytes(0),
    m_totalBytes(0),
    m_totalFreeBytes(0),
    m_prevSectorsRead(0),
    m_prevSectorsWritten(0)
{
    updateModelName();
}

void DiskInfo::setUpdateInterval(int seconds)
{
    m_updateIntervalS = static_cast<uint>(seconds);
}
#endif // __linux__

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

    updateActiveTime();
#endif // _WIN32

#ifdef __linux__
    std::ifstream file("/proc/diskstats");
    std::string line;

    ulonglong readsCompleted, readsMerged, sectorsRead, msReading, writesCompleted, writesMerged, sectorsWritten, msWriting, iOsInProgress, msIOs, weightedMsIo;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string dev;
        std::string device;
        iss >> dev >> dev >> device;
        if (device == m_device) {
            iss >> readsCompleted >> readsMerged >> sectorsRead >> msReading
                >> writesCompleted >> writesMerged >> sectorsWritten >> msWriting
                >> iOsInProgress >> msIOs >> weightedMsIo;
            break;
        }
    }
    file.close();

    m_readSpeed = (sectorsRead - m_prevSectorsRead) * 512 / m_updateIntervalS;
    m_writeSpeed = (sectorsWritten - m_prevSectorsWritten) * 512 / m_updateIntervalS;

    ulonglong total = (sectorsRead - m_prevSectorsRead) + (sectorsWritten - m_prevSectorsWritten);
    ulonglong totalOld = m_prevSectorsRead + m_prevSectorsWritten;

    if(totalOld == 0)
        m_activeTime = (static_cast<double>(total) / totalOld) * 100.0;

    m_prevSectorsRead = sectorsRead;
    m_prevSectorsWritten = sectorsWritten;

    m_avgResponseTime = weightedMsIo / static_cast<double>(readsCompleted + writesCompleted);

    FILE* fp;
    char buffer[1024];
    std::string cmd = "df -k | grep /dev/" + m_device;
    fp = popen(cmd.c_str(), "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return;
    }

    while (fgets(buffer, sizeof(buffer) - 2, fp) != NULL) {
        // Extract the disk device name from the line. 
        std::istringstream iss(buffer);
        std::string device;
        iss >> device >> m_totalBytes >> m_totalUsedBytes >> m_totalFreeBytes;
    }
    pclose(fp);

    m_totalBytes *= 1024;
    m_totalUsedBytes *= 1024;
    m_totalFreeBytes = m_totalBytes - m_totalUsedBytes;
#endif // __linux__
}

#ifdef _WIN32
void DiskInfo::updateActiveTime()
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
#endif // _WIN32

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

#ifdef __linux__
    std::string path = "/sys/class/block/" + m_device + "/device/model";
    std::ifstream file(path);
    if (!file) {
        qDebug() << "Failed to open " << QString::fromStdString(path);
        return;
    }

    std::string model;
    std::getline(file, model);
    m_modelName = std::wstring(model.begin(), model.end());
#endif // __linux__

}

#ifdef WIN32
std::wstring DiskInfo::diskLetter() const
{
    return m_diskLetter;
}
#endif // WIN32

#ifdef __linux__
std::string DiskInfo::device() const
{
    return m_device;
}
#endif // __linux__


std::wstring DiskInfo::modelName() const
{
    return m_modelName;
}

byte DiskInfo::activeTime() const
{
    return m_activeTime;
}

ulonglong DiskInfo::readSpeed() const
{
    return m_readSpeed;
}

ulonglong DiskInfo::writeSpeed() const
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
