#include "CustomChartView.h"


CustomChartView::CustomChartView(QWidget* parent, int minPointsX, int maxPointsX, QString titleX, int minPointsY, int maxPointsY, QString titleY, QColor borderColor, QColor fillColor):
	QChartView(parent),
	m_maxPointsX(maxPointsX)
{
    m_upperSeries = new QLineSeries(parent);
    m_lowerSeries = new QLineSeries(parent);

    m_areaSeries = new QAreaSeries(m_upperSeries, m_lowerSeries);
    m_areaSeries->setColor(fillColor);
    m_areaSeries->setBorderColor(borderColor);

    auto chart = new QChart();
    chart->addSeries(m_upperSeries);
    chart->addSeries(m_areaSeries);
    chart->createDefaultAxes();
    chart->legend()->setVisible(false);

    chart->setBackgroundBrush(QBrush(QColor("#202025")));

    auto axisX = new QValueAxis(parent);
    axisX->setRange(minPointsX, maxPointsX);
    axisX->setTickCount(2);
    axisX->setMinorTickCount(15);
    axisX->setReverse(true);
    axisX->setLabelFormat("%i");
    axisX->setTitleText(titleX);

    axisX->setLinePenColor(QColor("#36363B"));
    axisX->setGridLineColor(QColor("#36363B"));
    axisX->setMinorGridLineColor(QColor("#36363B"));
    axisX->setTitleBrush(QBrush(Qt::white));
    axisX->setLabelsColor(Qt::white);

    auto axisY = new QValueAxis(parent);
    axisY->setRange(minPointsY, maxPointsY);
    axisY->setLabelFormat("%i");
    axisY->setTitleText(titleY);

    axisY->setLinePenColor(QColor("#36363B"));
    axisY->setGridLineColor(QColor("#36363B"));
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setLabelsColor(Qt::white);

    chart->setAxisX(axisX, m_areaSeries);
    chart->removeAxis(chart->axisY());
    chart->addAxis(axisY, Qt::AlignRight);
    m_areaSeries->attachAxis(axisY);
    
    this->setChart(chart);
    this->setRenderHint(QPainter::Antialiasing);
}

void CustomChartView::append(double data)
{
    int size = m_dataPoints.size();
    if (size > m_maxPointsX)
        m_dataPoints.pop_back();
    m_dataPoints.push_front(data);

    m_upperSeries->clear();
    for (int i = 0; i < size; ++i)
        m_upperSeries->append(i, m_dataPoints[i]);

    m_lowerSeries->clear();
    m_lowerSeries->append(0, 0);
    m_lowerSeries->append(size, 0);
}
