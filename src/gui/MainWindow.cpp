#include "gui/MainWindow.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QFormLayout>
#include <QFuture>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QtConcurrentRun>

namespace sdrcal::gui {
namespace {
QPlainTextEdit* readOnlyText(const QString& name, const QString& description) {
    auto* text = new QPlainTextEdit;
    text->setReadOnly(true);
    text->setObjectName(name);
    text->setAccessibleName(name);
    text->setAccessibleDescription(description);
    text->setLineWrapMode(QPlainTextEdit::NoWrap);
    return text;
}

} // namespace

MainWindow::MainWindow() {
    setWindowTitle(tr("SDR Calibration — Recorded Input"));
    resize(980, 720);
    auto* central = new QWidget;
    auto* layout = new QVBoxLayout(central);

    auto* notice = new QLabel(tr("Recorded CF32LE input only. This application does not access an "
                                 "SDR or establish calibration accuracy."));
    notice->setWordWrap(true);
    notice->setAccessibleName(tr("Scope notice"));
    layout->addWidget(notice);

    auto* inputs = new QGroupBox(tr("Calibration inputs"));
    auto* form = new QFormLayout(inputs);
    request_ = new QLineEdit;
    trust_ = new QLineEdit;
    output_ = new QLineEdit;
    request_->setObjectName("requestPath");
    trust_->setObjectName("trustPath");
    output_->setObjectName("outputPath");
    request_->setAccessibleDescription(tr("Versioned recorded-calibration request JSON file"));
    trust_->setAccessibleDescription(tr("Independent local registry signature pin file"));
    output_->setAccessibleDescription(tr("New output directory; existing directories are refused"));
    auto addSelector = [&](const QString& labelText, QLineEdit* edit, const QString& buttonText,
                           auto callback) {
        auto* row = new QWidget;
        auto* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        auto* button = new QPushButton(buttonText);
        rowLayout->addWidget(edit);
        rowLayout->addWidget(button);
        auto* label = new QLabel(labelText);
        label->setBuddy(edit);
        form->addRow(label, row);
        connect(button, &QPushButton::clicked, this, callback);
    };
    addSelector(tr("Run request:"), request_, tr("Choose…"), &MainWindow::chooseRequest);
    addSelector(tr("Trust pins:"), trust_, tr("Choose…"), &MainWindow::chooseTrust);
    addSelector(tr("New output directory:"), output_, tr("Choose…"), &MainWindow::chooseOutput);
    layout->addWidget(inputs);

    auto* actions = new QHBoxLayout;
    review_ = new QPushButton(tr("Review Request"));
    start_ = new QPushButton(tr("Start Calibration"));
    cancel_ = new QPushButton(tr("Cancel"));
    open_result_ = new QPushButton(tr("Open Result…"));
    for (auto* button : {review_, start_, cancel_, open_result_})
        actions->addWidget(button);
    actions->addStretch();
    layout->addLayout(actions);
    connect(review_, &QPushButton::clicked, this, &MainWindow::reviewRequest);
    connect(start_, &QPushButton::clicked, this, &MainWindow::startRun);
    connect(cancel_, &QPushButton::clicked, this, &MainWindow::cancelRun);
    connect(open_result_, &QPushButton::clicked, this, &MainWindow::openResult);
    connect(&watcher_, &QFutureWatcher<RunResult>::finished, this, &MainWindow::runFinished);

    status_ = new QLabel(tr("Ready — select explicit recorded-input files."));
    status_->setObjectName("statusText");
    status_->setAccessibleName(tr("Calibration status"));
    layout->addWidget(status_);

    auto* tabs = new QTabWidget;
    request_review_ = readOnlyText("Request review", tr("Bounded read-only request review"));
    diagnostics_ = readOnlyText("Progress and diagnostics", tr("Production workflow diagnostics"));
    diagnostics_->setMaximumBlockCount(2000);
    terminal_ = readOnlyText("Terminal result", tr("Exact production terminal JSON"));
    artifacts_ = new QTabWidget;
    artifacts_->setAccessibleName(tr("Published artifact review"));
    tabs->addTab(request_review_, tr("Request / Device / Measurements"));
    tabs->addTab(diagnostics_, tr("Progress"));
    tabs->addTab(terminal_, tr("Result"));
    tabs->addTab(artifacts_, tr("Profile / Evidence / Interoperability"));
    layout->addWidget(tabs, 1);
    setCentralWidget(central);
    setTabOrder(request_, trust_);
    setTabOrder(trust_, output_);
    setTabOrder(output_, review_);
    setTabOrder(review_, start_);
    setRunning(false);
}

MainWindow::~MainWindow() {
    if (watcher_.isRunning()) {
        cancellation_->cancel();
        watcher_.waitForFinished();
    }
}

void MainWindow::chooseRequest() {
    const auto path =
        QFileDialog::getOpenFileName(this, tr("Select run request"), {}, tr("JSON (*.json)"));
    if (!path.isEmpty())
        request_->setText(path);
}
void MainWindow::chooseTrust() {
    const auto path =
        QFileDialog::getOpenFileName(this, tr("Select trust-pin file"), {}, tr("JSON (*.json)"));
    if (!path.isEmpty())
        trust_->setText(path);
}
void MainWindow::chooseOutput() {
    const auto parent = QFileDialog::getExistingDirectory(this, tr("Select output parent"));
    if (!parent.isEmpty())
        output_->setText(parent + "/calibration-result");
}

RunSelection MainWindow::selection() const {
    return {request_->text().toStdString(), trust_->text().toStdString(),
            output_->text().toStdString()};
}

void MainWindow::reviewRequest() {
    const auto reviewed = reviewFile(selection().request_path);
    if (!reviewed.ok()) {
        request_review_->setPlainText(
            tr("Review failed: %1").arg(QString::fromStdString(reviewed.error)));
        status_->setText(tr("Request review failed."));
        return;
    }
    const auto bytes = QByteArray::fromStdString(reviewed.contents);
    QJsonParseError error;
    const auto document = QJsonDocument::fromJson(bytes, &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        request_review_->setPlainText(tr("Review failed: request is not a JSON object.\n\n") +
                                      QString::fromUtf8(bytes));
        status_->setText(tr("Request review failed."));
        return;
    }
    const auto root = document.object();
    const auto device = root.value("device").toObject();
    const auto summary =
        tr("Schema: %1 %2\nDevice: %3 %4 — %5\nObservations: %6\n"
           "Interoperability requested: %7\n\nExact bounded request:\n")
            .arg(root.value("schema_name").toString(), root.value("schema_version").toString(),
                 device.value("manufacturer").toString(), device.value("model").toString(),
                 device.value("identifier").toString())
            .arg(root.value("observations").toArray().size())
            .arg(root.contains("interoperability") ? tr("yes — lossy WSJT-X projection")
                                                   : tr("no"));
    request_review_->setPlainText(summary +
                                  QString::fromUtf8(document.toJson(QJsonDocument::Indented)));
    status_->setText(tr("Request reviewed. Execution validation remains authoritative."));
}

void MainWindow::startRun() {
    if (request_->text().isEmpty() || trust_->text().isEmpty() || output_->text().isEmpty()) {
        status_->setText(tr("Request, trust pins, and new output directory are required."));
        return;
    }
    cancellation_ = std::make_shared<CancellationToken>();
    diagnostics_->clear();
    terminal_->clear();
    artifacts_->clear();
    setRunning(true);
    status_->setText(tr("Calibration running — cancellation is cooperative."));
    const auto selected = selection();
    watcher_.setFuture(QtConcurrent::run([this, selected, token = cancellation_] {
        return runRecordedCalibration(selected, token, [this](std::string text) {
            QMetaObject::invokeMethod(
                this,
                [this, text = std::move(text)] {
                    diagnostics_->appendPlainText(QString::fromStdString(text).trimmed());
                },
                Qt::QueuedConnection);
        });
    }));
}

void MainWindow::cancelRun() {
    if (cancellation_) {
        cancellation_->cancel();
        cancel_->setEnabled(false);
        status_->setText(tr("Cancellation requested — waiting for a safe stopping point."));
    }
}

void MainWindow::runFinished() {
    const auto result = watcher_.result();
    terminal_->setPlainText(QString::fromStdString(result.terminal_json));
    setRunning(false);
    if (result.exit == cli::ProductExit::success) {
        if (loadArtifacts(selection().output_directory))
            status_->setText(tr("Calibration succeeded; published artifacts loaded for review."));
    } else if (result.exit == cli::ProductExit::cancelled) {
        status_->setText(tr("Calibration cancelled; no result was published."));
    } else {
        QString category = tr("unknown");
        if (result.exit == cli::ProductExit::usage)
            category = tr("usage");
        if (result.exit == cli::ProductExit::input)
            category = tr("input or authentication");
        if (result.exit == cli::ProductExit::workflow)
            category = tr("scientific workflow");
        if (result.exit == cli::ProductExit::output)
            category = tr("output publication");
        status_->setText(tr("Calibration failed: %1 (production exit %2).")
                             .arg(category)
                             .arg(static_cast<int>(result.exit)));
    }
}

bool MainWindow::loadArtifacts(const std::filesystem::path& directory) {
    artifacts_->clear();
    bool requiredFailure = false;
    for (const auto& file : reviewResultDirectory(directory)) {
        auto* view = readOnlyText(QString::fromStdString(file.path.filename().string()),
                                  tr("Read-only published artifact"));
        view->setPlainText(file.ok()
                               ? QString::fromStdString(file.contents)
                               : tr("Review failed: %1").arg(QString::fromStdString(file.error)));
        requiredFailure = requiredFailure || (!file.ok() && file.path.filename() != "wsjtx.ini");
        artifacts_->addTab(view, QString::fromStdString(file.path.filename().string()));
    }
    if (requiredFailure)
        status_->setText(tr("Calibration published, but required artifact review failed."));
    return !requiredFailure;
}

void MainWindow::openResult() {
    const auto directory = QFileDialog::getExistingDirectory(this, tr("Open calibration result"));
    if (!directory.isEmpty()) {
        if (loadArtifacts(directory.toStdString()))
            status_->setText(tr("Existing result loaded read-only."));
    }
}

void MainWindow::setRunning(bool running) {
    request_->setEnabled(!running);
    trust_->setEnabled(!running);
    output_->setEnabled(!running);
    review_->setEnabled(!running);
    start_->setEnabled(!running);
    open_result_->setEnabled(!running);
    cancel_->setEnabled(running);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (!watcher_.isRunning()) {
        event->accept();
        return;
    }
    const auto answer = QMessageBox::question(
        this, tr("Calibration is running"),
        tr("Cancel the calibration and wait for safe cleanup before closing?"));
    if (answer == QMessageBox::Yes) {
        cancellation_->cancel();
        watcher_.waitForFinished();
        event->accept();
    } else {
        event->ignore();
    }
}

} // namespace sdrcal::gui
