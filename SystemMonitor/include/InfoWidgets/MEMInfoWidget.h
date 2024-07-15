#pragma once

#include "CustomChartView.h"
#include "InfoWidget.h"
#include "Info/MEMInfo.h"
#include "Converter.h"

class MEMInfoWidget : public InfoWidget
{
public:
	MEMInfoWidget(QWidget* parent = nullptr);

	void updateInfo() override;
	void updateLabels() override;

private:
    QLabel* m_usageLabel;
    QLabel* m_availLabel;
    QLabel* m_usagePageFileLabel;
    QLabel* m_availPageFileLabel;
	MEMInfo m_memInfo;
	CustomChartView* m_usageChartView;
};