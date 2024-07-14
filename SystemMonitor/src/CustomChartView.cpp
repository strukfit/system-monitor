#include "CustomChartView.h"


CustomChartView::CustomChartView(QWidget* parent, int minPointsX, int maxPointsX, QString titleX, int minPointsY, int maxPointsY, QString titleY, QColor borderColor1, QColor fillColor1, QColor borderColor2, QColor fillColor2):
	QChartView(parent),
	m_maxPointsX(maxPointsX)
{
    m_label = new QLabel(this);
    m_label->setStyleSheet("background-color: transparent; color: white; font-weight: bold;");

    m_upperSeries1 = new QLineSeries(this);
    m_lowerSeries1 = new QLineSeries(this);

    m_upperSeries2 = new QLineSeries(this);
    m_lowerSeries2 = new QLineSeries(this);

    m_areaSeries1 = new QAreaSeries(m_upperSeries1, m_lowerSeries1);
    m_areaSeries1->setColor(fillColor1);
    m_areaSeries1->setBorderColor(borderColor1);

    m_areaSeries2 = new QAreaSeries(m_upperSeries2, m_lowerSeries2);
    m_areaSeries2->setColor(fillColor2);
    m_areaSeries2->setBorderColor(borderColor2);

    auto chart = new QChart();
    chart->addSeries(m_upperSeries1);
    chart->addSeries(m_areaSeries1);
    chart->addSeries(m_upperSeries2);
    chart->addSeries(m_areaSeries2);
    chart->createDefaultAxes();
    chart->legend()->setVisible(false);

    chart->setBackgroundBrush(QBrush(QColor("#202025")));

    auto axisX = new QValueAxis(this);
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

    auto axisY = new QValueAxis(this);
    axisY->setRange(minPointsY, maxPointsY);
    axisY->setLabelFormat("%i");
    axisY->setTitleText(titleY);

    axisY->setLinePenColor(QColor("#36363B"));
    axisY->setGridLineColor(QColor("#36363B"));
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setLabelsColor(Qt::white);

    chart->removeAxis(chart->axisX());
    chart->removeAxis(chart->axisY());
    chart->addAxis(axisX, Qt::AlignBottom);
    m_areaSeries1->attachAxis(axisX);
    m_areaSeries2->attachAxis(axisX);
    chart->addAxis(axisY, Qt::AlignRight);
    m_areaSeries1->attachAxis(axisY);
    m_areaSeries2->attachAxis(axisY);
    
    this->setChart(chart);
    this->setRenderHint(QPainter::Antialiasing);
}

void CustomChartView::append(double data, QString labelText)
{
    m_label->clear();
    m_label->setText(labelText);
    moveLabel();

    int size = m_dataPoints1.size();
    if (size > m_maxPointsX)
        m_dataPoints1.pop_back();
    m_dataPoints1.push_front(data);

    m_upperSeries1->clear();
    for (int i = 0; i < size; ++i)
        m_upperSeries1->append(i, m_dataPoints1[i]);

    m_lowerSeries1->clear();
    m_lowerSeries1->append(0, 0);
    m_lowerSeries1->append(size, 0);
}

void CustomChartView::append(double data1, double data2, QString labelText)
{
    append(data1, labelText);

    int size = m_dataPoints2.size();
    if (size > m_maxPointsX)
        m_dataPoints2.pop_back();
    m_dataPoints2.push_front(data2);

    m_upperSeries2->clear();
    for (int i = 0; i < size; ++i)
        m_upperSeries2->append(i, m_dataPoints2[i]);
    
    m_lowerSeries2->clear();
    m_lowerSeries2->append(0, 0);
    m_lowerSeries2->append(size, 0);
}

void CustomChartView::resizeEvent(QResizeEvent* event)
{
    moveLabel();
    QChartView::resizeEvent(event);
}

void CustomChartView::moveLabel()
{
    m_label->adjustSize();
    m_label->move(this->width() / 2.f - m_label->width() / 2.f, this->height() / 1.09 - m_label->height() / 2.f);
}
