#pragma once

#ifdef _WIN32
#include <nvml.h>
#include "SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h"
#include "SDK/Include/IPerformanceMonitoring.h"  
#endif // _WIN32

#include <QString>
#include <QDebug>

namespace gpu
{
	enum Type
	{
		NVIDIA,
		AMD,
		UNKNOWN
	};
}

#ifdef _WIN32
using namespace adlx;
#endif // _WIN32

class GPU
{
public:
#ifdef _WIN32
	GPU(QString modelName, gpu::Type type, nvmlDevice_t nvmlDevice);
	GPU(QString modelName, gpu::Type type, IADLXGPUPtr adlxGpuPtr, IADLXPerformanceMonitoringServicesPtr perfMonitoringServices);
#endif // _WIN32

	GPU();
	~GPU();

	void updateInfo();

	QString modelName() const;
	unsigned int usage() const;
	unsigned long long memoryUsed() const;
	unsigned long long memoryTotal() const;
	unsigned int temperature() const;

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

	QString m_modelName;

	gpu::Type m_type;

	unsigned int m_usage;
	unsigned long long m_memoryUsed;
	unsigned long long m_memoryTotal;
	unsigned int m_temperature;
};