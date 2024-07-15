#pragma once

#include <QMainWindow>

#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>

#include <thread>

#include "CustomChartView.h"
#include "DiskChartView.h"

#include "Info/CPUInfo.h"
#include "Info/MEMInfo.h"
#include "Info/DiskInfo.h"
#include "Info/GPUInfo.h"
#include "Converter.h"

#include "InfoWidgets/CPUInfoWidget.h"
#include "InfoWidgets/MEMInfoWidget.h"
#include "InfoWidgets/GPUInfoWidget.h"
#include "CustomScrollBar.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private:
	Q_SLOT void updateWidgets();

	void initDisks();
	void initNvidiaCards(QWidget* parent, QLayout* layout);
	void initAmdCards(QWidget* parent, QLayout* layout);

	static void updateDiskAsync(DiskInfo& diskInfo, QLabel* diskLabel);

#ifdef _WIN32
	WMIManager wmiManager;
	ADLXHelper m_ADLXHelp;
#endif

	int updateIntervalMs;

	std::vector<DiskInfo> allDisks;
	std::vector<QLabel*> allDisksLabels;

	std::vector<InfoWidget*> allWidgets;

	/*CPUInfoWidget* cpuInfoWidget;
	MEMInfoWidget* memInfoWidget;*/
	static DiskChartView* diskChartView;
	static CustomChartView* diskSpeedChartView;
};