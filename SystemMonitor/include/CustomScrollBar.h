#pragma once

#include <QScrollBar>
#include <QEvent>
#include <QTimer>
#include <QPropertyAnimation>

class CustomScrollBar : public QScrollBar
{
public:
	CustomScrollBar(QWidget* parent = nullptr);

protected:
	void enterEvent(QEnterEvent* event) override;
	void leaveEvent(QEvent* event) override;

private:
	void setDefaultStyle();
	void setHoveredStyle();
};