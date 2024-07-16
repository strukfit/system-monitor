#include "DiskChartView.h"

const QString DiskChartView::BACKGROUND_COLOR = "transparent";

DiskChartView::DiskChartView(QWidget* parent):
    QChartView(parent)
{    
    this->setStyleSheet("border: 0;");

    m_series = new QPieSeries(parent);

    auto chart = new QChart();
    chart->legend()->setVisible(false);
    chart->addSeries(m_series);
    chart->setBackgroundBrush(QBrush(QColor(BACKGROUND_COLOR)));

    this->setChart(chart);

    m_totalSpaceLabel = new QLabel(this);
    m_totalSpaceLabel->setStyleSheet("background-color: transparent; color: white; font-weight: bold;");

    connect(this, &DiskChartView::valuesReceived, this, &DiskChartView::updateSeries);
    connect(this, &DiskChartView::spaceUpdated, this, &DiskChartView::updateTotalLabel);
}

void DiskChartView::updateSpace(ulonglong free, ulonglong used, ulonglong total)
{
    emit valuesReceived(free, used, total);

    QString totalLabelText = "Total: " + QString::fromStdString(Converter::convertBytes(total));
    emit spaceUpdated(totalLabelText);
}

void DiskChartView::resizeEvent(QResizeEvent* event)
{
    m_totalSpaceLabel->adjustSize();
    m_totalSpaceLabel->move(this->width() / 2.f - m_totalSpaceLabel->width() / 2.f, this->height() / 1.15 - m_totalSpaceLabel->height() / 2.f);

    QChartView::resizeEvent(event);
}

void DiskChartView::updateTotalLabel(QString labelText)
{
    m_totalSpaceLabel->setText(labelText);
    m_totalSpaceLabel->adjustSize();
    m_totalSpaceLabel->move(this->width() / 2.f - m_totalSpaceLabel->width() / 2.f, this->height() / 1.15 - m_totalSpaceLabel->height() / 2.f);
}

void DiskChartView::updateSeries(ulonglong free, ulonglong used, ulonglong total)
{
    if(!m_series->isEmpty())
        m_series->clear();

    QString freeSpaceTitle = QString("Free space\n\n%1").arg(QString::fromStdString(Converter::convertBytes(free)));
    QString usedSpaceTitle = QString("Used space\n\n%1").arg(QString::fromStdString(Converter::convertBytes(used)));
    m_series->append(freeSpaceTitle, free);
    m_series->append(usedSpaceTitle, used);

    m_series->setLabelsPosition(QPieSlice::LabelInsideHorizontal);
    for (auto slice : m_series->slices()) {
        slice->setLabelVisible(true);
        slice->setLabelBrush(Qt::white);
        slice->setBorderColor(BACKGROUND_COLOR);
        slice->setBorderWidth(2);
    }

    m_series->slices().at(0)->setColor("#DFB476");
    m_series->slices().at(1)->setColor("#E17276");
}
