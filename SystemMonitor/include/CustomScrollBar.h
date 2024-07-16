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
#ifdef WIN32
	void enterEvent(QEnterEvent* event) override;
#endif // WIN32
#ifdef __linux__
	void enterEvent(QEvent* event) override;
#endif // __linux__
	void leaveEvent(QEvent* event) override;

private:
	void setDefaultStyle();
	void setHoveredStyle();
};