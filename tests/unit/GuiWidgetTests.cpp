#include "gui/MainWindow.h"

#include <QApplication>
#include <QElapsedTimer>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QThread>
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
    CHECK(start && start->isEnabled());
    CHECK(cancel && !cancel->isEnabled());
    CHECK(window.findChild<QTabWidget*>() != nullptr);

    auto* request = window.findChild<QLineEdit*>("requestPath");
    auto* trust = window.findChild<QLineEdit*>("trustPath");
    auto* output = window.findChild<QLineEdit*>("outputPath");
    CHECK(request && trust && output && start && cancel);
    if (request && trust && output && start && cancel) {
        request->setText("missing-request.json");
        trust->setText("missing-trust.json");
        output->setText("new-output");
        start->click();
        CHECK(!start->isEnabled());
        CHECK(cancel->isEnabled());
        QElapsedTimer timer;
        timer.start();
        while (!start->isEnabled() && timer.elapsed() < 5000) {
            application.processEvents();
            QThread::msleep(1);
        }
        CHECK(start->isEnabled());
        CHECK(!cancel->isEnabled());
        const auto* terminal = window.findChild<QPlainTextEdit*>("Terminal result");
        CHECK(terminal && terminal->toPlainText().contains("input_error"));
    }
    return failures == 0 ? 0 : 1;
}
