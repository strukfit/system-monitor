#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QProcess>

#ifdef _WIN32
#include <windows.h>
#include "WMIManager.h"
//#else 
//#include <sys/sysinfo.h>
//#include <fstream>
#endif

#include "CPUInfo.h"
#include "MEMInfo.h"
#include "DisksInfo.h"
#include "GPUsInfo.h"

QLabel* cpuLabel;
QLabel* memLabel;
QLabel* diskLabel;
QLabel* gpuLabel;

void updateDisksAsync(DisksInfo& disksInfo, QLabel* diskLabel)
{
    disksInfo.updateInfo();
    QString labelText;
    for (const auto& disk : disksInfo.allDisks())
    {
        QString diskText = QString(
            "DISK: %1\n"
            "DISK_NAME: %2\n"
            "DISK_ACTIVE_TIME: %3 %\n"
            "DISK_USAGE: %4/%5 Gb\n"
            "DISK_FREE_SPACE: %6 Gb\n"
            "DISK_READ_SPEED: %7 Mb/s\n"
            "DISK_WRITE_SPEED: %8 Mb/s\n"
            "DISK_AVG_RESPONSE_TIME: %9 ms\n\n")
            .arg(disk->diskLetter())
            .arg(disk->modelName())
            .arg(disk->activeTime())
            .arg(disk->totalUsedBytes() / 1024.f / 1024.f / 1024.f)
            .arg(disk->totalBytes() / 1024.f / 1024.f / 1024.f)
            .arg(disk->totalFreeBytes() / 1024.f / 1024.f / 1024.f)
            .arg(disk->readSpeed() / 1024.f / 1024.f)
            .arg(disk->writeSpeed() / 1024.f / 1024.f)
            .arg(disk->avgResponseTime() * 1000.f);

        labelText.append(diskText);
    }

    QMetaObject::invokeMethod(diskLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, labelText));
}

WMIManager wmiManager;

CPUInfo cpuInfo;
MEMInfo memInfo;
DisksInfo disksInfo;
GPUsInfo gpusInfo;

Q_SLOT void updateLabels()
{
    cpuInfo.updateInfo();

    cpuLabel->setText(QString(
        "CPU_NAME: %1\n"
        "CPU_USAGE: %2\n"
        "CPU_PROCESSES: %3\n"
        "CPU_THREADS: %4\n"
        "CPU_HANDLES: %5\n"
        "CPU_BASE_SPEED: %6 GHz\n"
        "CPU_CORES: %7\n"
        "CPU_LOGIC_PROC: %8\n")
        .arg(cpuInfo.modelName())
        .arg(cpuInfo.usage())
        .arg(cpuInfo.processCount())
        .arg(cpuInfo.threadCount())
        .arg(cpuInfo.handleCount())
        .arg(cpuInfo.baseSpeed())
        .arg(cpuInfo.coreCount())
        .arg(cpuInfo.logicalProcessorCount()));

    memInfo.updateInfo();
    memLabel->setText(QString(
        "MEM: %1/%2 GB\n"
        "MEM_AVAIL: %3\n"
        "MEM_PAGE_FILE: %4/%5 GB\n"
        "MEM_PAGE_FILE_AVAIL: %6\n"
        "MEM_SPEED: %7 MHz")
        .arg(memInfo.usedGB())
        .arg(memInfo.totalGB())
        .arg(memInfo.availGB())
        .arg(memInfo.usedPageFileGB())
        .arg(memInfo.totalPageFileGB())
        .arg(memInfo.availPageFileGB())
        .arg(memInfo.speedMHz()));

    gpusInfo.updateInfo();

    QString gpuLabelText;
    for (const auto& gpu : gpusInfo.allGPUs())
    {
        QString gpuText = QString(
            "GPU_NAME: %1\n"
            "GPU_USAGE: %2 %\n"
            "GPU_MEM_USAGE: %3/%4 Gb\n"
            "GPU_TEMPERATURE: %5 C\n\n")
            .arg(gpu->modelName())
            .arg(gpu->usage() / 1.f)
            .arg(gpu->memoryUsed() / 1024.f / 1024.f / 1024.f)
            .arg(gpu->memoryTotal() / 1024.f / 1024.f / 1024.f)
            .arg(gpu->temperature());

        gpuLabelText.append(gpuText);
    }

    gpuLabel->setText(gpuLabelText);

    std::thread updateDisksThread(updateDisksAsync, std::ref(disksInfo), diskLabel);
    updateDisksThread.detach();
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QMainWindow* window = new QMainWindow();

    auto centralWidget = new QWidget(window);

    auto layout = new QVBoxLayout(centralWidget);

    cpuLabel = new QLabel(NULL, centralWidget);
    layout->addWidget(cpuLabel);

    memLabel = new QLabel(NULL, centralWidget);
    layout->addWidget(memLabel);

    gpuLabel = new QLabel(NULL, centralWidget);
    layout->addWidget(gpuLabel);

    diskLabel = new QLabel(NULL, centralWidget);
    layout->addWidget(diskLabel);

    updateLabels();

    auto timer = new QTimer(centralWidget);
    QObject::connect(timer, &QTimer::timeout, &updateLabels);
    timer->start(1000);

    window->setCentralWidget(centralWidget);
    window->show();

    return app.exec();
}