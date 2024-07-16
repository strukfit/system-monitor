#pragma once

#include <QWidget>
#include <QEvent>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>

class InfoWidget : public QWidget
{
	Q_OBJECT

public:
	InfoWidget(QWidget* parent = nullptr);
	~InfoWidget();

	virtual void updateInfo();

protected:
	Q_SIGNAL void infoUpdated();
	virtual Q_SLOT void updateLabels();

protected:
	bool event(QEvent* event) override;
	void setDefaultStyle();

	QVBoxLayout* m_layout;
};