#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QMessageBox>

#ifdef _WIN32
#include <windows.h>
//#else 
//#include <sys/sysinfo.h>
//#include <fstream>
#endif

#include "CPUInfo.h"
#include "MEMInfo.h"


ULARGE_INTEGER getDiskSpace(LPCWSTR drive) 
{
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    GetDiskFreeSpaceEx(drive, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
    return totalNumberOfBytes;
}

QLabel* cpuLabel;
QLabel* memLabel;
QLabel* diskLabel;

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

    diskLabel = new QLabel(NULL, centralWidget);
    layout->addWidget(diskLabel);

    CPUInfo cpuInfo;
    MEMInfo memInfo;

    auto timer = new QTimer(centralWidget);
    QObject::connect(timer, &QTimer::timeout, [&] {
        cpuInfo.updateInfo();
        memInfo.updateInfo();
        ULARGE_INTEGER totalDiskUsage = getDiskSpace(L"C:");

        cpuLabel->setText(QString(
            "CPU_USAGE: %1\n"
            "CPU_PROCESSES: %2\n"
            "CPU_THREADS: %3\n"
            "CPU_HANDLES: %4\n"
            "CPU_BASE_SPEED: %5 GHz\n"
            "CPU_CORES: %6\n"
            "CPU_LOGIC_PROC: %7\n")
                .arg(cpuInfo.usage())
                .arg(cpuInfo.processCount())
                .arg(cpuInfo.threadCount())
                .arg(cpuInfo.handleCount())
                .arg(cpuInfo.baseSpeed())
                .arg(cpuInfo.coreCount())
                .arg(cpuInfo.logicalProcessorCount()));

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
        
        diskLabel->setText(QString("DISK_TOTAL: %1").arg(totalDiskUsage.QuadPart / 1024.f / 1024.f / 1024.f));

    });
    timer->start(1000);

    window->setCentralWidget(centralWidget);
    window->show();

    return app.exec();
}