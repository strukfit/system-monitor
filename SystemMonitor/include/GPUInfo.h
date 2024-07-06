#pragma once

#include <QString>
#include <QDebug>

#ifdef _WIN32
#include <nvml.h>
#include "SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h"
#include "SDK/Include/IPerformanceMonitoring.h"  

using namespace adlx;
#endif // _WIN32

#ifdef __linux__
#include <sstream>
#endif // __linux__

using ulonglong = unsigned long long;
using uint = unsigned int;

namespace gpu
{
	enum Type
	{
		NVIDIA,
		AMD,
		UNKNOWN
	};
}

class GPUInfo
{
public:
#ifdef _WIN32
	GPUInfo(QString modelName, gpu::Type type, nvmlDevice_t nvmlDevice);
	GPUInfo(QString modelName, gpu::Type type, IADLXGPUPtr adlxGpuPtr, IADLXPerformanceMonitoringServicesPtr perfMonitoringServices);
#endif // _WIN32

#ifdef __linux__
	GPUInfo(std::string index, gpu::Type type);
#endif // __linux__

	~GPUInfo();

	void updateInfo();

	QString modelName() const;
	uint usage() const;
	ulonglong memoryUsed() const;
	ulonglong memoryTotal() const;
	uint temperature() const;

private:
	void updateUsageNvidia();
	void updateMemoryNvidia();
	void updateTemperatureNvidia();

	void updateUsageAMD();
	void updateMemoryAMD();
	void updateTemperatureAMD();

#ifdef _WIN32
	nvmlDevice_t m_nvmlDevice;

	IADLXGPUPtr m_AdlxGpuPtr;
	IADLXPerformanceMonitoringServicesPtr m_perfMonitoringServices;
	IADLXGPUMetricsSupportPtr m_gpuMetricsSupport;
	IADLXGPUMetricsPtr m_gpuMetrics;
#endif // _WIN32

#ifdef __linux__
	void updateModelName();
	void updateInfoNvidia();
	std::string exec(const char* cmd);

	std::string m_index;
#endif // __linux__

	QString m_modelName;

	gpu::Type m_type;

	uint m_usage;
	ulonglong m_memoryUsed;
	ulonglong m_memoryTotal;
	uint m_temperature;
};