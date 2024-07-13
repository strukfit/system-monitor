#pragma once

#include <QWidget>
#include <QEvent>
#include <QGraphicsDropShadowEffect>

#include "InfoWidgetUI.h"

class InfoWidget : public QWidget
{
	Q_OBJECT

public:
	InfoWidget(QWidget* parent = nullptr);
	~InfoWidget();

	Q_SIGNAL void clicked();

protected:
	void mouseReleaseEvent(QMouseEvent* event) override;
	bool event(QEvent* event) override;

	InfoWidgetUI* ui;
};