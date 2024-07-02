#pragma once

#include <vector>
#include <memory>

#include "GPU.h"

#ifdef _WIN32
#include "WMIManager.h"
#else
#endif

class GPUsInfo
{
public:
	GPUsInfo();
	~GPUsInfo();

	void updateInfo();
	const std::vector<std::unique_ptr<GPU>>& allGPUs() const;

private:
	GPUType gpuType(QString modelName);

	std::vector<std::unique_ptr<GPU>> m_allGPUs;
};