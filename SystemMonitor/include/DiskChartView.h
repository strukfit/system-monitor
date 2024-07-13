#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QLabel>

#include "Converter.h"


class DiskChartView : public QChartView
{
public:
	DiskChartView(QWidget* parent = nullptr);

	void updateSpace(ulonglong free, ulonglong used, ulonglong total);

protected:
	void resizeEvent(QResizeEvent* event) override;

private:
	static const QString BACKGROUND_COLOR;

	QPieSeries* m_series;
	QLabel* m_totalSpaceLabel;
};