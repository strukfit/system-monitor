#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include "CustomChartView.h"
#include "InfoWidget.h"
#include "Info/CPUInfo.h"


class CPUInfoWidget : public InfoWidget
{
	Q_OBJECT

public:
	CPUInfoWidget(QWidget* parent = nullptr);
	
	void updateInfo() override;
	void updateLabels() override;

private:
	QLabel* m_usageLabel;
	QLabel* m_processesLabel;
	QLabel* m_threadsLabel;
	QLabel* m_handlesLabel;
	CPUInfo m_cpuInfo;
	CustomChartView* m_usageChartView;
};