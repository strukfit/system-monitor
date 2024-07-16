#include "CustomChartView.h"


CustomChartView::CustomChartView(QWidget* parent, double minPointsX, double maxPointsX, QString titleX, double minPointsY, double maxPointsY, QString titleY, QColor borderColor1, QColor fillColor1, QColor borderColor2, QColor fillColor2):
	QChartView(parent),
	m_maxPointsX(maxPointsX),
    m_maxPointsY(maxPointsY)
{
    this->setStyleSheet("border: 0;");

    m_upperSeries1 = new QLineSeries(this);
    m_upperSeries1->setColor(borderColor1);
    m_lowerSeries1 = new QLineSeries(this);
    m_lowerSeries1->setColor(borderColor1);

    m_upperSeries2 = new QLineSeries(this);
    m_upperSeries2->setColor(borderColor2);
    m_lowerSeries2 = new QLineSeries(this);
    m_lowerSeries2->setColor(borderColor2);

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

    chart->setBackgroundBrush(QBrush(Qt::transparent));

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

    axisY = new QValueAxis(this);
    axisY->setRange(minPointsY, maxPointsY);
    axisY->setLabelFormat("%.2f");
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

    connect(this, &CustomChartView::dataReceived1, this, &CustomChartView::updateSeries1);
    connect(this, &CustomChartView::dataReceived2, this, &CustomChartView::updateSeries2);
}

void CustomChartView::append(double data)
{
    emit dataReceived1(data);
    updateYAxisRange();
}

void CustomChartView::append(double data1, double data2)
{
    append(data1);
    dataReceived2(data2);
    updateYAxisRange();
}

void CustomChartView::updateSeries1(double data)
{
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

void CustomChartView::updateSeries2(double data)
{
    int size = m_dataPoints2.size();
    if (size > m_maxPointsX)
        m_dataPoints2.pop_back();
    m_dataPoints2.push_front(data);

    m_upperSeries2->clear();
    for (int i = 0; i < size; ++i)
        m_upperSeries2->append(i, m_dataPoints2[i]);

    m_lowerSeries2->clear();
    m_lowerSeries2->append(0, 0);
    m_lowerSeries2->append(size, 0);
}

double CustomChartView::minY() const {
    return axisY->min();
}

double CustomChartView::maxY() const {
    return axisY->max();
}

void CustomChartView::setRangeY(double min, double max) {
    axisY->setRange(min, max);
}

void CustomChartView::updateYAxisRange() {
    double maxData1 = std::numeric_limits<double>::min();
    for (const auto& data : m_dataPoints1) {
        if (data > maxData1) {
            maxData1 = data;
        }
    }

    double maxData2 = std::numeric_limits<double>::min();
    for (const auto& data : m_dataPoints2) {
        if (data > maxData2) {
            maxData2 = data;
        }
    }

    double newYMax = std::max(maxData1, maxData2);

    if (newYMax < m_maxPointsY)
        newYMax = m_maxPointsY;

    if (newYMax != maxY()) {
        setRangeY(minY(), std::ceil(newYMax / 10.) * 10);
    }
}