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

#include "InfoWidget.h"
#include "CustomScrollBar.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private:
	Q_SLOT void updateLabels();

	void initDisks();
	void initNvidiaCards();
	void initAmdCards();

	static void updateCPUAsync(CPUInfo& cpuInfo, QLabel* cpuLabel);
	static void updateMEMAsync(MEMInfo& memInfo, QLabel* memLabel);
	static void updateGPUAsync(GPUInfo& gpuInfo, QLabel* gpuLabel);
	static void updateDiskAsync(DiskInfo& diskInfo, QLabel* diskLabel);

#ifdef _WIN32
	WMIManager wmiManager;
	ADLXHelper m_ADLXHelp;
#endif

	int updateIntervalMs;

	QLabel* cpuLabel;
	QLabel* memLabel;

	CPUInfo cpuInfo;
	MEMInfo memInfo;

	std::vector<DiskInfo> allDisks;
	std::vector<QLabel*> allDisksLabels;

	std::vector<GPUInfo> allGPUs;
	std::vector<QLabel*> allGPUsLabels;

	static CustomChartView* usageChartView;
	static DiskChartView* diskChartView;
};