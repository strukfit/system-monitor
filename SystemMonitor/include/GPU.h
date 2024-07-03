#pragma once

#include <QString>
#include <QDebug>
#include <nvml.h>
#include "SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h"
#include "SDK/Include/IPerformanceMonitoring.h"

namespace gpu
{
	enum Type
	{
		NVIDIA,
		AMD,
		UNKNOWN
	};
}

using namespace adlx;

class GPU
{
public:
	GPU(QString modelName, gpu::Type type, nvmlDevice_t nvmlDevice);
	GPU(QString modelName, gpu::Type type, IADLXGPUPtr adlxGpuPtr, IADLXPerformanceMonitoringServicesPtr perfMonitoringServices);
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

	nvmlDevice_t m_nvmlDevice;

	IADLXGPUPtr m_AdlxGpuPtr;
	IADLXPerformanceMonitoringServicesPtr m_perfMonitoringServices;
	IADLXGPUMetricsSupportPtr m_gpuMetricsSupport;
	IADLXGPUMetricsPtr m_gpuMetrics;

	QString m_modelName;

	gpu::Type m_type;

	unsigned int m_usage;
	unsigned long long m_memoryUsed;
	unsigned long long m_memoryTotal;
	unsigned int m_temperature;
};