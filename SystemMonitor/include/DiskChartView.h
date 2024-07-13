#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

#include "Converter.h"

class DiskChartView : public QChartView
{
public:
	DiskChartView(QWidget* parent = nullptr);

	void updateSpace(ulonglong free, ulonglong used);

private:
	QPieSeries* m_series;
};