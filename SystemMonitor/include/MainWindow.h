#pragma once

#include <QMainWindow>

#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>

#include <thread>

#include "CustomChartView.h"
#include "DiskChartView.h"
#include "Converter.h"

#include "InfoWidgets/CPUInfoWidget.h"
#include "InfoWidgets/MEMInfoWidget.h"
#include "InfoWidgets/GPUInfoWidget.h"
#include "InfoWidgets/DiskInfoWidget.h"
#include "CustomScrollBar.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private:
	Q_SLOT void updateWidgets();

	void initDisks(QWidget* parent, QLayout* layout);
	void initNvidiaCards(QWidget* parent, QLayout* layout);
	void initAmdCards(QWidget* parent, QLayout* layout);

#ifdef _WIN32
	WMIManager wmiManager;
	ADLXHelper m_ADLXHelp;
#endif

	int updateIntervalMs;
	std::vector<InfoWidget*> allWidgets;
};