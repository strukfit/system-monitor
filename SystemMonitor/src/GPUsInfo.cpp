#include "GPUsInfo.h"

GPUsInfo::GPUsInfo()
{
    std::wstring query = L"SELECT caption FROM win32_videocontroller";
    std::wstring property = L"caption";
    std::vector<WMIValue> results;

    WMIManager::execQuery(query, property, results);

    if (results.empty())
    {
        std::cout << "results is empty";
        return;
    }

    for (const auto& result : results)
    {
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::wstring>)
            {
                QString GPUModelName = QString::fromStdString(std::string(arg.begin(), arg.end()));
                m_allGPUs.push_back(std::make_unique<GPU>(GPUModelName));
            }
        }, result);
    }
}

GPUsInfo::~GPUsInfo()
{
}

void GPUsInfo::updateInfo()
{
}

const std::vector<std::unique_ptr<GPU>>& GPUsInfo::allGPUs() const
{
	return m_allGPUs;
}
