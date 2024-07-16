#include <QApplication>
#include "MainWindow.h"

using ulonglong = unsigned long long;

int main(int argc, char** argv)
{
    qputenv("QT_DEBUG_PLUGINS", "1");
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    qRegisterMetaType<ulonglong>("ulonglong");

    MainWindow window;
    window.show();

    return app.exec();
}