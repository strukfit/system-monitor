#pragma once

#include <QtCharts/QChart> 
#include <QtCharts/QChartView> 
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>

#include <deque>

class CustomChartView : public QChartView
{
public:
	CustomChartView(QWidget* parent = nullptr, int minPointsX = 0, int maxPointsX = 1, QString titleX = "", int minPointsY = 0, int maxPointsY = 1, QString titleY = "", QColor borderColor = QColor(150, 150, 250, 255), QColor fillColor = QColor(150, 150, 250, 100));

	void append(double data);

private:
	QLineSeries* m_upperSeries;
	QLineSeries* m_lowerSeries;
	QAreaSeries* m_areaSeries;
	int m_maxPointsX;
	std::deque<double> m_dataPoints;
};