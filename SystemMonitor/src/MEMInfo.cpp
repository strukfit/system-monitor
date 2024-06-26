#include "MEMInfo.h"

MEMInfo::MEMInfo(std::shared_ptr<WMIManager> p_WMIManager) :
    m_totalGB(0.f),
    m_availGB(0.f),
    m_usedGB(0.f),
    m_availPageFileGB(0.f),
    m_totalPageFileGB(0.f),
    m_usedPageFileGB(0.f),
    m_WMIManager(p_WMIManager)
{
    updateInfo();
    updateSpeedInfo();
}

MEMInfo::~MEMInfo()
{
}

void MEMInfo::updateInfo()
{
	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memStatus);

    m_totalGB = memStatus.ullTotalPhys / 1024.f / 1024.f / 1024.f;
    m_availGB = memStatus.ullAvailPhys / 1024.f / 1024.f / 1024.f;
    m_usedGB = m_totalGB - m_availGB;

    m_availPageFileGB = memStatus.ullAvailPageFile / 1024.f / 1024.f / 1024.f;
    m_totalPageFileGB = memStatus.ullTotalPageFile / 1024.f / 1024.f / 1024.f;
    m_usedPageFileGB = m_totalPageFileGB - m_availPageFileGB;
}

void MEMInfo::updateSpeedInfo()
{
    std::wstring query = L"SELECT * FROM Win32_PhysicalMemory";
    std::wstring property = L"Speed";
    std::vector<WMIValue> results;
    m_WMIManager->execQuery(query, property, results);

    UINT speed = 0;
    for (const auto& result : results) {
        std::visit([&speed](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, long long>) {
                if (arg > speed) speed = arg;
            }
            }, result);
    }
    m_speedMHz = speed;
}

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

float MEMInfo::speedMHz() const
{
    return m_speedMHz;
}
