#include "gui/MainWindow.h"

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace {
int failures = 0;
#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            std::cerr << __FILE__ << ':' << __LINE__ << ": check failed: " #condition "\n";        \
            ++failures;                                                                            \
        }                                                                                          \
    } while (false)
} // namespace

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    sdrcal::gui::MainWindow window;
    const auto edits = window.findChildren<QLineEdit*>();
    CHECK(edits.size() == 3);
    for (const auto* edit : edits)
        CHECK(!edit->accessibleDescription().isEmpty());
    const auto* status = window.findChild<QLabel*>("statusText");
    CHECK(status != nullptr);
    CHECK(status && !status->accessibleName().isEmpty());
    const auto* notice = window.findChild<QLabel*>("scopeNotice");
    CHECK(notice && notice->text().contains("recorded or live"));
    const auto texts = window.findChildren<QPlainTextEdit*>();
    CHECK(texts.size() >= 3);
    for (const auto* text : texts) {
        CHECK(text->isReadOnly());
        CHECK(!text->accessibleName().isEmpty());
    }
    const auto buttons = window.findChildren<QPushButton*>();
    auto findButton = [&](const QString& text) {
        for (auto* button : buttons)
            if (button->text() == text)
                return button;
        return static_cast<QPushButton*>(nullptr);
    };
    auto* start = findButton("Start Calibration");
    auto* cancel = findButton("Cancel");
    auto* review = findButton("Review Request");
    CHECK(start && start->isEnabled());
    CHECK(cancel && !cancel->isEnabled());
    CHECK(window.findChild<QTabWidget*>() != nullptr);

    auto* request = window.findChild<QLineEdit*>("requestPath");
    auto* trust = window.findChild<QLineEdit*>("trustPath");
    auto* output = window.findChild<QLineEdit*>("outputPath");
    CHECK(request && trust && output && start && cancel && review);
    if (request && trust && output && start && cancel && review) {
        const auto requestPath =
            std::filesystem::temp_directory_path() / "sdrcal-gui-widget-live-request.json";
        {
            std::ofstream stream(requestPath);
            stream
                << R"({"schema_name":"sdrcal-live-calibration-request","schema_version":"1.0.0","device":{"manufacturer":"Test","model":"Fake","identifier":"fake-1"},"observations":[]})";
        }
        request->setText(QString::fromStdString(requestPath.string()));
        trust->setText("missing-trust.json");
        output->setText("new-output");
        review->click();
        CHECK(status && status->text().contains("Live request reviewed"));
        {
            std::ofstream stream(requestPath);
            stream << R"({"schema_name":"unrecognized"})";
        }
        start->click();
        CHECK(start->isEnabled());
        CHECK(!cancel->isEnabled());
        CHECK(status && status->text().contains("review failed"));
        std::error_code ignored;
        std::filesystem::remove(requestPath, ignored);
    }
    return failures == 0 ? 0 : 1;
}
