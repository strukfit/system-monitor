#pragma once

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QLabel>

#include "Converter.h"


class DiskChartView : public QChartView
{
	Q_OBJECT

public:
	DiskChartView(QWidget* parent = nullptr);

	void updateSpace(ulonglong free, ulonglong used, ulonglong total);

protected:
	void resizeEvent(QResizeEvent* event) override;

private:
	Q_SIGNAL void spaceUpdated(QString labelText);
	Q_SIGNAL void valuesReceived(ulonglong free, ulonglong used, ulonglong total);
	Q_SLOT void updateTotalLabel(QString labelText);
	Q_SLOT void updateSeries(ulonglong free, ulonglong used, ulonglong total);

	static const QString BACKGROUND_COLOR;

	QPieSeries* m_series;
	QLabel* m_totalSpaceLabel;
};