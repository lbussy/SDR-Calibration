#pragma once

#include "gui/GuiWorkflow.h"

#include <QFutureWatcher>
#include <QMainWindow>

class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QTabWidget;

namespace sdrcal::gui {

class MainWindow final : public QMainWindow {
  public:
    explicit MainWindow(cli::LiveBoundaryFactory live_boundary_factory = {});
    ~MainWindow() override;

  protected:
    void closeEvent(QCloseEvent* event) override;

  private:
    void chooseRequest();
    void chooseTrust();
    void chooseOutput();
    void reviewRequest();
    void openResult();
    void startRun();
    void cancelRun();
    void runFinished();
    [[nodiscard]] bool loadArtifacts(const std::filesystem::path& directory);
    void setRunning(bool running);
    [[nodiscard]] RunSelection selection() const;

    QLineEdit* request_ = nullptr;
    QLineEdit* trust_ = nullptr;
    QLineEdit* output_ = nullptr;
    QPushButton* review_ = nullptr;
    QPushButton* start_ = nullptr;
    QPushButton* cancel_ = nullptr;
    QPushButton* open_result_ = nullptr;
    QLabel* status_ = nullptr;
    QPlainTextEdit* request_review_ = nullptr;
    QPlainTextEdit* diagnostics_ = nullptr;
    QPlainTextEdit* terminal_ = nullptr;
    QTabWidget* artifacts_ = nullptr;
    cli::LiveBoundaryFactory live_boundary_factory_;
    std::filesystem::path reviewed_request_path_;
    std::string reviewed_request_contents_;
    cli::ProductInputMode reviewed_mode_ = cli::ProductInputMode::recorded;
    bool reviewed_schema_ = false;
    QFutureWatcher<RunResult> watcher_;
    std::shared_ptr<CancellationToken> cancellation_;
};

} // namespace sdrcal::gui
