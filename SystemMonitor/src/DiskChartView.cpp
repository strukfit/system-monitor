#include "DiskChartView.h"

DiskChartView::DiskChartView(QWidget* parent):
    QChartView(parent)
{    
    m_series = new QPieSeries(parent);
    auto chart = new QChart();
    chart->legend()->setVisible(false);
    chart->addSeries(m_series);
    this->setChart(chart);
}

void DiskChartView::updateSpace(ulonglong free, ulonglong used)
{
    m_series->clear();

    QString freeSpaceTitle = QString("Free space\n%1").arg(QString::fromStdString(Converter::convertBytes(free)));
    QString usedSpaceTitle = QString("Used space\n%1").arg(QString::fromStdString(Converter::convertBytes(used)));
    m_series->append(freeSpaceTitle, free);
    m_series->append(usedSpaceTitle, used);

    for (auto slice : m_series->slices()) {
        slice->setLabelVisible(true);
    }
}
