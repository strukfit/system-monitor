#pragma once

#include <QtCharts/QChart> 
#include <QtCharts/QChartView> 
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QLabel>

#include <deque>
#include <cmath>

#ifdef __linux__
QT_CHARTS_USE_NAMESPACE
#endif // __linux__

class CustomChartView : public QChartView
{
	Q_OBJECT

public:
	CustomChartView(QWidget* parent = nullptr, double minPointsX = 0, double maxPointsX = 1, QString titleX = "", double minPointsY = 0, double maxPointsY = 1, QString titleY = "", QColor borderColor1 = QColor(150, 150, 250, 255), QColor fillColor1 = QColor(150, 150, 250, 100), QColor borderColor2 = QColor(230, 122, 50, 255), QColor fillColor2 = QColor(230, 122, 50, 100));

	void append(double data);
	void append(double data1, double data2);

private:
	Q_SIGNAL void dataReceived1(double data);
	Q_SIGNAL void dataReceived2(double data);

	Q_SLOT void updateSeries1(double data);
	Q_SLOT void updateSeries2(double data);

	double minY() const;
	double maxY() const;
	void setRangeY(double min, double max);
	void updateYAxisRange();

	QLineSeries* m_upperSeries1;
	QLineSeries* m_lowerSeries1;
	QAreaSeries* m_areaSeries1;

	QLineSeries* m_upperSeries2;
	QLineSeries* m_lowerSeries2;
	QAreaSeries* m_areaSeries2;

	double m_maxPointsX;
	double m_maxPointsY;
	std::deque<double> m_dataPoints1;
	std::deque<double> m_dataPoints2;

	QValueAxis* axisY;
};