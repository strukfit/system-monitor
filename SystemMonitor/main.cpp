#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QProcess>

#ifdef _WIN32
#include <windows.h>
#include "WMIManager.h"
#endif // _WIN32

#ifdef __unix__

#endif // __unix__


#include "CPUInfo.h"
#include "MEMInfo.h"
#include "DisksInfo.h"
#include "GPUsInfo.h"


QLabel* cpuLabel;
QLabel* memLabel;
QLabel* diskLabel;
QLabel* gpuLabel;

static void updateCPUAsync(CPUInfo& cpuInfo, QLabel* cpuLabel)
{
    cpuInfo.updateInfo();
    QString labelText = QString(
        "CPU_NAME: %1\n"
        "CPU_USAGE: %2\n"
        "CPU_PROCESSES: %3\n"
        "CPU_THREADS: %4\n"
        "CPU_HANDLES: %5\n"
        "CPU_BASE_SPEED: %6 GHz\n"
        "CPU_CORES: %7\n"
        "CPU_LOGIC_PROC: %8\n")
        .arg(QString::fromStdWString(cpuInfo.modelName()))
        .arg(cpuInfo.usage())
        .arg(cpuInfo.processCount())
        .arg(cpuInfo.threadCount())
        .arg(cpuInfo.handleCount())
        .arg(cpuInfo.baseSpeed())
        .arg(cpuInfo.coreCount())
        .arg(cpuInfo.logicalProcessorCount());

    QMetaObject::invokeMethod(cpuLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, labelText));
}

static void updateMEMAsync(MEMInfo& memInfo, QLabel* memLabel)
{
    memInfo.updateInfo();
    QString labelText = QString(
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
        .arg(memInfo.speedMHz());

    QMetaObject::invokeMethod(memLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, labelText));
}

static void updateGPUsAsync(GPUsInfo& gpusInfo, QLabel* gpuLabel)
{
    gpusInfo.updateInfo();
    QString labelText;
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

        labelText.append(gpuText);
    }

    QMetaObject::invokeMethod(gpuLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, labelText));
}

static void updateDisksAsync(DisksInfo& disksInfo, QLabel* diskLabel)
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
            .arg(QString::fromStdWString(disk->diskLetter()))
            .arg(QString::fromStdWString(disk->modelName()))
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

#ifdef _WIN32
WMIManager wmiManager;
#endif

CPUInfo cpuInfo;
MEMInfo memInfo;
DisksInfo disksInfo;
GPUsInfo gpusInfo;

Q_SLOT void updateLabels()
{
    std::thread updateCPUThread(updateCPUAsync, std::ref(cpuInfo), cpuLabel);
    updateCPUThread.detach();

    std::thread updateMEMThread(updateMEMAsync, std::ref(memInfo), memLabel);
    updateMEMThread.detach();

    std::thread updateGPUsThread(updateGPUsAsync, std::ref(gpusInfo), gpuLabel);
    updateGPUsThread.detach();

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