#include <QApplication>
#include <QSurfaceFormat>
#include "view/mainwindow.h"
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QSurfaceFormat format;

    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);

    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);

    format.setSamples(4);

    QSurfaceFormat::setDefaultFormat(format);

    MainWindow mainWindow;
    mainWindow.resize(1024, 768);
    mainWindow.show();

    return app.exec();
}
