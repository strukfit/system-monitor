#include "InfoWidgetUI.h"

void InfoWidgetUI::setupUI(QWidget* widget)
{
	widget->setContentsMargins(0, 0, 0, 0);
	widget->setAttribute(Qt::WA_StyledBackground, true);
	setDefaultStyle(widget);

	layout = new QVBoxLayout(widget);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignCenter);
}

void InfoWidgetUI::setDefaultStyle(QWidget* widget)
{
	widget->setStyleSheet("background-color: #323232; border-radius: 10px; border: 0;");
}
