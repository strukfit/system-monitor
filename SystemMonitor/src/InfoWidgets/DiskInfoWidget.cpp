#include "InfoWidgets/DiskInfoWidget.h"

#ifdef WIN32
DiskInfoWidget::DiskInfoWidget(QWidget* parent, const char diskLetter):
	InfoWidget(parent),
	m_diskInfo(diskLetter)
{
	init();
}
#endif // WIN32

#ifdef __linux__
DiskInfoWidget::DiskInfoWidget(QWidget* parent, std::string device):
	InfoWidget(parent),
	m_diskInfo(device)
{
	init();
}
#endif // __linux__

void DiskInfoWidget::init()
{
    auto titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(35, 0, 35, 0);
    titleLayout->setSpacing(0);

    QString title; 
#ifdef WIN32
    title = QString("Disk %1").arg(m_diskInfo.diskLetter());
#endif // WIN32
#ifdef __linux__
    title = QString("Disk %1").arg(QString::fromStdString(diskInfo.device()));
#endif // __linux__
    auto titleLabel = new QLabel(title, this);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    titleLabel->setStyleSheet("background-color: transparent; color: white; font-size: 18px; font-weight: bold; border: 0;");
    titleLayout->addWidget(titleLabel);

    auto modelNameLabel = new QLabel(QString::fromStdWString(m_diskInfo.modelName()), this);
    modelNameLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    modelNameLabel->setStyleSheet("background-color: transparent; color: white; font-size: 14px; font-weight: bold; border: 0;");
    titleLayout->addWidget(modelNameLabel);

    m_layout->addLayout(titleLayout);

    m_usageChartView = new DiskChartView(this);
    m_usageChartView->setMinimumHeight(500);
    m_layout->addWidget(m_usageChartView);

    m_speedChartView = new CustomChartView(
        this,
        0, 60, "\n",
        0, 500, "Write/read speed, Kb/s");
    m_speedChartView->setMinimumHeight(500);
    m_layout->addWidget(m_speedChartView);

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

    labelInit(m_activeTimeLabel);
    infoLayout->addWidget(m_activeTimeLabel, 0, 0);
    labelInit(m_readSpeedLabel);
    infoLayout->addWidget(m_readSpeedLabel, 0, 1);
    labelInit(m_writeSpeedLabel);
    infoLayout->addWidget(m_writeSpeedLabel, 1, 0);
    labelInit(m_avgResponseTimeLabel);
    infoLayout->addWidget(m_avgResponseTimeLabel, 1, 1);

    m_layout->addLayout(infoLayout);
}

void DiskInfoWidget::updateInfo()
{
    m_diskInfo.updateInfo();

    m_usageChartView->updateSpace(m_diskInfo.totalFreeBytes(), m_diskInfo.totalUsedBytes(), m_diskInfo.totalBytes());

    m_speedChartView->append(m_diskInfo.writeSpeed() / 1024., m_diskInfo.readSpeed() / 1024.);

    emit infoUpdated();
}

void DiskInfoWidget::updateLabels()
{
    QString templateText = QString(
        "<p style='color: #909099; margin-bottom: 0;'>%1</p>"
        "<p style='margin-top: 0;'>%2%3</p>");

    QString labelText = templateText.arg("Active time").arg(m_diskInfo.activeTime()).arg('%');
    m_activeTimeLabel->setText(labelText);

    labelText = templateText.arg("Average response time").arg(m_diskInfo.avgResponseTime(), 0, 'f', 5).arg(" ms");
    m_avgResponseTimeLabel->setText(labelText);

    auto readSpeed = QString::fromStdString(Converter::convertBytes(m_diskInfo.readSpeed()));
    labelText = templateText.arg("Read speed").arg(readSpeed).arg(" ");
    m_readSpeedLabel->setText(labelText);

    auto writeSpeed = QString::fromStdString(Converter::convertBytes(m_diskInfo.writeSpeed()));
    labelText = templateText.arg("Write speed").arg(writeSpeed).arg(" ");
    m_writeSpeedLabel->setText(labelText);
    
}
