#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include "CustomChartView.h"
#include "DiskChartView.h"
#include "InfoWidget.h"
#include "Info/DiskInfo.h"
#include "Converter.h"


class DiskInfoWidget : public InfoWidget
{
public:
#ifdef WIN32
	DiskInfoWidget(QWidget* parent = nullptr, const char diskLetter = '\0');
#endif // WIN32
#ifdef __linux__
	DiskInfoWidget(QWidget* parent = nullptr, std::string device = "");
#endif // __linux__

	void updateInfo() override;
	void updateLabels() override;

private:
	void init();

	QLabel* m_activeTimeLabel;
	QLabel* m_readSpeedLabel;
	QLabel* m_writeSpeedLabel;
	QLabel* m_avgResponseTimeLabel;
	DiskInfo m_diskInfo;
	DiskChartView* m_usageChartView;
	CustomChartView* m_speedChartView;
};