#include "InfoWidgets/CPUInfoWidget.h"

CPUInfoWidget::CPUInfoWidget(QWidget* parent) :
    InfoWidget(parent),
    m_cpuInfo()
{
    auto titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(35, 0, 35, 0);
    titleLayout->setSpacing(0);

    auto titleLabel = new QLabel("CPU", this);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    titleLabel->setStyleSheet("background-color: transparent; color: white; font-size: 18px; font-weight: bold; border: 0;");
    titleLayout->addWidget(titleLabel);

    auto modelNameLabel = new QLabel(QString::fromStdString(m_cpuInfo.modelName()), this);
    modelNameLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    modelNameLabel->setStyleSheet("background-color: transparent; color: white; font-size: 14px; font-weight: bold; border: 0;");
    titleLayout->addWidget(modelNameLabel);

    m_layout->addLayout(titleLayout);

    m_usageChartView = new CustomChartView(
        this,
        0, 60, "",
        0, 100, "Usage, %");
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
    labelInit(m_processesLabel);
    infoLayout->addWidget(m_processesLabel, 0, 1);
    labelInit(m_threadsLabel);
    infoLayout->addWidget(m_threadsLabel, 0, 2);
    labelInit(m_handlesLabel);
    infoLayout->addWidget(m_handlesLabel, 0, 3);

    QLabel *baseSpeedLabel, *coresLabel, *logicalProcessorsLabel;
    labelInit(baseSpeedLabel);
    infoLayout->addWidget(baseSpeedLabel, 1, 0);
    labelInit(coresLabel);
    infoLayout->addWidget(coresLabel, 1, 1);
    labelInit(logicalProcessorsLabel);
    infoLayout->addWidget(logicalProcessorsLabel, 1, 2);

    QString templateText = QString(
        "<p style='color: #909099; margin-bottom: 0;'>%1</p>"
        "<p style='margin-top: 0;'>%2%3</p>");

    QString labelText = templateText.arg("Base speed").arg(m_cpuInfo.baseSpeed()).arg(" GHz");
    baseSpeedLabel->setText(labelText);

    labelText = templateText.arg("Cores").arg(m_cpuInfo.coreCount()).arg("");
    coresLabel->setText(labelText);

    labelText = templateText.arg("Logical processors").arg(m_cpuInfo.logicalProcessorCount()).arg("");
    logicalProcessorsLabel->setText(labelText);

    m_layout->addLayout(infoLayout);

    /*QString labelText = QString(
        "CPU_NAME: %1\n"
        "CPU_USAGE: %2\n"
        "CPU_PROCESSES: %3\n"
        "CPU_THREADS: %4\n"
        "CPU_HANDLES: %5\n"
        "CPU_BASE_SPEED: %6 GHz\n"
        "CPU_CORES: %7\n"
        "CPU_LOGIC_PROC: %8\n")
        .arg(QString::fromStdString(m_cpuInfo.modelName()))
        .arg(m_cpuInfo.usage())
        .arg(m_cpuInfo.processCount())
        .arg(m_cpuInfo.threadCount())
        .arg(m_cpuInfo.handleCount())
        .arg(m_cpuInfo.baseSpeed())
        .arg(m_cpuInfo.coreCount())
        .arg(m_cpuInfo.logicalProcessorCount());*/
}

void CPUInfoWidget::updateInfo()
{
    m_cpuInfo.updateInfo();

    double usage = m_cpuInfo.usage();
    m_usageChartView->append(usage);

    emit infoUpdated();
}

void CPUInfoWidget::updateLabels()
{
    QString templateText = QString(
        "<p style='color: #909099; margin-bottom: 0;'>%1</p>"
        "<p style='margin-top: 0;'>%2%3</p>");

    QString labelText = templateText.arg("Usage").arg(m_cpuInfo.usage(), 0, 'f', 2).arg('%');
    m_usageLabel->setText(labelText);

    labelText = templateText.arg("Processes").arg(m_cpuInfo.processCount()).arg("");
    m_processesLabel->setText(labelText);

    labelText = templateText.arg("Threads").arg(m_cpuInfo.threadCount()).arg("");
    m_threadsLabel->setText(labelText);

    labelText = templateText.arg("Handles").arg(m_cpuInfo.handleCount()).arg("");
    m_handlesLabel->setText(labelText);
}

