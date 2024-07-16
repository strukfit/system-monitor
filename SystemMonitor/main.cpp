#include <QApplication>
#include "MainWindow.h"


int main(int argc, char** argv)
{
    qputenv("QT_DEBUG_PLUGINS", "1");
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    MainWindow window;
    window.show();

    return app.exec();
}