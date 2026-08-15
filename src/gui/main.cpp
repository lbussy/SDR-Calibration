#include "gui/MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    application.setApplicationName("SDR Calibration");
    application.setApplicationVersion("0.1.0");
    sdrcal::gui::MainWindow window;
    window.show();
    return application.exec();
}
