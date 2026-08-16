#include "gui/MainWindow.h"
#ifdef SDRCAL_GUI_LIVE_ENABLED
#include "cli/LiveCliSupport.h"
#endif

#include <QApplication>
#include <utility>

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    application.setApplicationName("SDR Calibration");
    application.setApplicationVersion("0.1.0");
    sdrcal::cli::LiveBoundaryFactory liveFactory;
#ifdef SDRCAL_GUI_LIVE_ENABLED
    liveFactory = sdrcal::cli::productionLiveBoundaryFactory();
#endif
    sdrcal::gui::MainWindow window(std::move(liveFactory));
    window.show();
    return application.exec();
}
