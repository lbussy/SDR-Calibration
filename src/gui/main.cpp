#include "gui/MainWindow.h"
#include "sdrcal/Version.h"
#ifdef SDRCAL_GUI_LIVE_ENABLED
#include "cli/LiveCliSupport.h"
#endif

#include <QApplication>
#include <utility>

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    application.setApplicationName("SDR Calibration");
    application.setApplicationVersion(QString::fromUtf8(
        sdrcal::kVersion.data(), static_cast<qsizetype>(sdrcal::kVersion.size())));
    sdrcal::cli::LiveBoundaryFactory liveFactory;
#ifdef SDRCAL_GUI_LIVE_ENABLED
    liveFactory = sdrcal::cli::productionLiveBoundaryFactory();
#endif
    sdrcal::gui::MainWindow window(std::move(liveFactory));
    window.show();
    return application.exec();
}
