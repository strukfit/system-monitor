#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include "CustomChartView.h"
#include "InfoWidget.h"
#include "Info/GPUInfo.h"
#include "Converter.h"


class GPUInfoWidget : public InfoWidget
{
public:
#ifdef _WIN32
	GPUInfoWidget(QWidget* parent = nullptr, QString modelName = "", gpu::Type type = gpu::NVIDIA, nvmlDevice_t nvmlDevice = nullptr);
	GPUInfoWidget(QWidget* parent = nullptr, QString modelName = "", gpu::Type type = gpu::AMD, IADLXGPUPtr adlxGpuPtr = nullptr, IADLXPerformanceMonitoringServicesPtr perfMonitoringServices = nullptr);
#endif // _WIN32

#ifdef __linux__
	GPUInfoWidget(QWidget* parent = nullptr, std::string index, gpu::Type type);
#endif // __linux__

	void updateInfo() override;
	void updateLabels() override;

private:
	void init();

	QLabel* m_usageLabel;
	QLabel* m_memUsageLabel;
	QLabel* m_temperatureLabel;
	GPUInfo m_gpuInfo;
	CustomChartView* m_usageChartView;
	CustomChartView* m_memUsageChartView;
};