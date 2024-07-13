#pragma once

#include <QWidget>
#include <QVBoxLayout>

class InfoWidgetUI
{
public:
	void setupUI(QWidget* widget);
	void setDefaultStyle(QWidget* widget);

	QVBoxLayout* layout;
};