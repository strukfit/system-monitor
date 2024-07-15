#include "InfoWidgets/MEMInfoWidget.h"

MEMInfoWidget::MEMInfoWidget(QWidget* parent):
	InfoWidget(parent),
	m_memInfo()
{
    auto titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(35, 0, 35, 0);
    titleLayout->setSpacing(0);

    auto titleLabel = new QLabel("Memory", this);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    titleLabel->setStyleSheet("background-color: transparent; color: white; font-size: 18px; font-weight: bold; border: 0;");
    titleLayout->addWidget(titleLabel);

    m_layout->addLayout(titleLayout);

    m_memInfo.updateInfo();

    // Bytes to Gbytes
    float total = m_memInfo.total() / 1024.f / 1024.f / 1024.f;

    m_usageChartView = new CustomChartView(
        this,
        0, 60, "", 
        0, total, "Usage, Gb",
        QColor(0, 255, 60), QColor(0, 255, 60, 100));
    m_usageChartView->setMinimumHeight(300);

    m_layout->addWidget(m_usageChartView);

    auto infoLayout = new QGridLayout();
    infoLayout->setContentsMargins(35, 0, 35, 0);
    infoLayout->setVerticalSpacing(10);
    infoLayout->setHorizontalSpacing(0);

    auto labelInit = [this](QLabel*& label)
    {
        label = new QLabel(this);
        label->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
        label->setStyleSheet("background-color: transparent; color: white; font-size: 14px; font-weight: bold; border: 0;");
    };

    labelInit(m_usageLabel);
    infoLayout->addWidget(m_usageLabel, 0, 0);
    labelInit(m_availLabel);
    infoLayout->addWidget(m_availLabel, 0, 1);
    labelInit(m_usagePageFileLabel);
    infoLayout->addWidget(m_usagePageFileLabel, 1, 0);
    labelInit(m_availPageFileLabel);
    infoLayout->addWidget(m_availPageFileLabel, 1, 1);

#ifdef WIN32
    QLabel* speedLabel;
    labelInit(speedLabel);
    infoLayout->addWidget(speedLabel, 0, 2);

    QString labelText = QString(
        "<p style='color: #909099; margin-bottom: 0;'>Speed</p>"
        "<p style='margin-top: 0;'>%2 MHz</p>")
        .arg(m_memInfo.speedMHz());
    speedLabel->setText(labelText);
#endif // WIN32

    m_layout->addLayout(infoLayout);
}

void MEMInfoWidget::updateInfo()
{
    m_memInfo.updateInfo();

    // Bytes to Gbytes
    float used = m_memInfo.used() / 1024.f / 1024.f / 1024.f;
    m_usageChartView->append(used);

    emit infoUpdated();
}

void MEMInfoWidget::updateLabels()
{
    QString templateText1 = QString(
        "<p style='color: #909099; margin-bottom: 0;'>%1</p>"
        "<p style='margin-top: 0;'>%2/%3</p>");

    QString templateText2 = QString(
        "<p style='color: #909099; margin-bottom: 0;'>%1</p>"
        "<p style='margin-top: 0;'>%2</p>");

    auto used = QString::fromStdString(Converter::convertBytes(m_memInfo.used()));
    auto total = QString::fromStdString(Converter::convertBytes(m_memInfo.total()));
    auto avail = QString::fromStdString(Converter::convertBytes(m_memInfo.avail()));

    auto usedPageFile = QString::fromStdString(Converter::convertBytes(m_memInfo.usedPageFile()));
    auto totalPageFile = QString::fromStdString(Converter::convertBytes(m_memInfo.totalPageFile()));
    auto availPageFile = QString::fromStdString(Converter::convertBytes(m_memInfo.availPageFile()));

    QString labelText = templateText1.arg("Usage").arg(used).arg(total);
    m_usageLabel->setText(labelText);

    labelText = templateText2.arg("Available").arg(avail);
    m_availLabel->setText(labelText);

    labelText = templateText1.arg("Page file usage").arg(usedPageFile).arg(totalPageFile);
    m_usagePageFileLabel->setText(labelText);

    labelText = templateText2.arg("Page file available").arg(availPageFile);
    m_availPageFileLabel->setText(labelText);
}
