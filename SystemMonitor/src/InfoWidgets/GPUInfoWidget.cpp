#include "InfoWidgets/GPUInfoWidget.h"


#ifdef WIN32
GPUInfoWidget::GPUInfoWidget(QWidget* parent, QString modelName, gpu::Type type, nvmlDevice_t nvmlDevice) :
    InfoWidget(parent),
    m_gpuInfo(modelName, type, nvmlDevice)
{
    init();
}

GPUInfoWidget::GPUInfoWidget(QWidget* parent, QString modelName, gpu::Type type, IADLXGPUPtr adlxGpuPtr, IADLXPerformanceMonitoringServicesPtr perfMonitoringServices) :
    InfoWidget(parent),
    m_gpuInfo(modelName, type, adlxGpuPtr, perfMonitoringServices)
{
    init();
}
#endif // WIN32

#ifdef __linux__
GPUInfoWidget::GPUInfoWidget(QWidget* parent, std::string index, gpu::Type type) :
    InfoWidget(parent),
    m_gpuInfo(index, type)
{
    init();
}
#endif // __linux__

void GPUInfoWidget::init()
{
    auto titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(35, 0, 35, 0);
    titleLayout->setSpacing(0);

    auto titleLabel = new QLabel("GPU", this);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    titleLabel->setStyleSheet("background-color: transparent; color: white; font-size: 18px; font-weight: bold; border: 0;");
    titleLayout->addWidget(titleLabel);

    auto modelNameLabel = new QLabel(m_gpuInfo.modelName(), this);
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

    m_gpuInfo.updateInfo();

    // Bytes to Gbytes
    float memoryTotalGb = m_gpuInfo.memoryTotal() / 1024.f / 1024.f / 1024.f;
    m_memUsageChartView = new CustomChartView(
        this,
        0, 60, "",
        0, memoryTotalGb, "Memory usage, Gb");
    m_memUsageChartView->setMinimumHeight(300);

    m_layout->addWidget(m_memUsageChartView);

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
    labelInit(m_temperatureLabel);
    infoLayout->addWidget(m_temperatureLabel, 0, 1);
    labelInit(m_memUsageLabel);
    infoLayout->addWidget(m_memUsageLabel, 1, 0);

    m_layout->addLayout(infoLayout);
}

void GPUInfoWidget::updateInfo()
{
    m_gpuInfo.updateInfo();

    m_usageChartView->append(m_gpuInfo.usage());

    // Bytes to Gbytes
    double memUsage = m_gpuInfo.memoryUsed() / 1024.f / 1024.f / 1024.f;
    m_memUsageChartView->append(memUsage);

    emit infoUpdated();
}

void GPUInfoWidget::updateLabels()
{
    QString templateText1 = QString(
        "<p style='color: #909099; margin-bottom: 0;'>%1</p>"
        "<p style='margin-top: 0;'>%2%3</p>");

    QString templateText2 = QString(
        "<p style='color: #909099; margin-bottom: 0;'>%1</p>"
        "<p style='margin-top: 0;'>%2/%3</p>");

    QString labelText = templateText1.arg("Usage").arg(m_gpuInfo.usage()).arg('%');
    m_usageLabel->setText(labelText);

    auto memoryUsed = QString::fromStdString(Converter::convertBytes(m_gpuInfo.memoryUsed()));
    auto memoryTotal = QString::fromStdString(Converter::convertBytes(m_gpuInfo.memoryTotal()));

    labelText = templateText2.arg("Memory usage").arg(memoryUsed).arg(memoryTotal);
    m_memUsageLabel->setText(labelText);

    labelText = templateText1.arg("Temperature").arg(m_gpuInfo.temperature()).arg(" C");
    m_temperatureLabel->setText(labelText);
}
