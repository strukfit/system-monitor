#pragma once

#include <QtCharts/QChart> 
#include <QtCharts/QChartView> 
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <deque>

class UsageChartView : public QChartView
{
public:
	UsageChartView(QWidget* parent = nullptr, int minPoints = 0, int maxPoints = 1);

	void append(double data);

private:
	QLineSeries* m_series;
	int m_maxPoints;
	std::deque<double> m_dataPoints;
};