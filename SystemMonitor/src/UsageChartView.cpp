#include "UsageChartView.h"


UsageChartView::UsageChartView(QWidget* parent, int minPoints, int maxPoints):
	QChartView(parent),
	m_maxPoints(maxPoints)
{
    m_series = new QLineSeries(parent);
    auto chart = new QChart();
    chart->addSeries(m_series);
    chart->createDefaultAxes();
    chart->legend()->setVisible(false);

    auto axisX = new QValueAxis(parent);
    axisX->setRange(minPoints, maxPoints);
    axisX->setTickCount(2);
    axisX->setMinorTickCount(15);
    axisX->setReverse(true);
    axisX->setLabelFormat("%i");
    axisX->setTitleText("Time");

    auto axisY = new QValueAxis(parent);
    axisY->setRange(0, 100);
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Usage %");

    chart->setAxisX(axisX, m_series);
    chart->removeAxis(chart->axisY());
    chart->addAxis(axisY, Qt::AlignRight);
    m_series->attachAxis(axisY);

    this->setChart(chart);
    this->setRenderHint(QPainter::Antialiasing);
}

void UsageChartView::append(double data)
{
    int size = m_dataPoints.size();
    if (size > m_maxPoints)
        m_dataPoints.pop_back();
    m_dataPoints.push_front(data);

    m_series->clear();
    for (int i = 0; i < size; ++i)
        m_series->append(i, m_dataPoints[i]);
}
