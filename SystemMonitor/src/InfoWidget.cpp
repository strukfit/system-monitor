#include "InfoWidget.h"

InfoWidget::InfoWidget(QWidget* parent):
	QWidget(parent)
{
	this->setContentsMargins(0, 0, 0, 0);
	this->setAttribute(Qt::WA_StyledBackground, true);
	setDefaultStyle();

	layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignCenter);
}

InfoWidget::~InfoWidget()
{
}

void InfoWidget::mouseReleaseEvent(QMouseEvent* event)
{
	emit clicked();

	QWidget::mouseReleaseEvent(event);
}

bool InfoWidget::event(QEvent* event)
{
	if (event->type() == QEvent::Enter)
	{
		setStyleSheet("background-color: #2E2E2E; border-radius: 10px; border: 1px solid #363636;");

		QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect;
		shadowEffect->setBlurRadius(20);
		shadowEffect->setColor(QColor(0, 0, 0, 40));
		shadowEffect->setOffset(0, 7);

		this->setGraphicsEffect(shadowEffect);

		return true;
	}

	if (event->type() == QEvent::Leave)
	{
		setDefaultStyle();

		this->setGraphicsEffect(nullptr);

		return true;
	}

	return QWidget::event(event);
}

void InfoWidget::setDefaultStyle()
{
	setStyleSheet("background-color: #323232; border-radius: 10px; border: 0;");
}