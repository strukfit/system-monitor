#include "InfoWidgets/InfoWidget.h"

InfoWidget::InfoWidget(QWidget* parent):
	QWidget(parent)
{
	this->setContentsMargins(1, 20, 1, 20);
	this->setAttribute(Qt::WA_StyledBackground, true);
	setDefaultStyle();

	m_layout = new QVBoxLayout(this);
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setSpacing(0);
	m_layout->setAlignment(Qt::AlignCenter);

	connect(this, &InfoWidget::infoUpdated, this, &InfoWidget::updateLabels);
}

InfoWidget::~InfoWidget()
{
}

void InfoWidget::updateInfo()
{
}

void InfoWidget::updateLabels()
{
}

bool InfoWidget::event(QEvent* event)
{
	/*if (event->type() == QEvent::Enter)
	{
		setStyleSheet("background-color: #212126; border-radius: 10px; border: 1px solid #00A0FF;");

		QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect;
		shadowEffect->setBlurRadius(30);
		shadowEffect->setColor(QColor(0, 160, 255, 100));
		shadowEffect->setOffset(0, 1);

		this->setGraphicsEffect(shadowEffect);

		return true;
	}

	if (event->type() == QEvent::Leave)
	{
		setDefaultStyle();

		this->setGraphicsEffect(nullptr);

		return true;
	}*/

	return QWidget::event(event);
}

void InfoWidget::setDefaultStyle()
{
	setStyleSheet("background-color: #202025; border-radius: 10px; border: 0;");
}