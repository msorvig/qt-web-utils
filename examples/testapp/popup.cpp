#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

#include <qtwebutils.h>

#include <QtCore/qsize.h>

// This file exports functions for opening and closing popup windows, using
// utility functions provided by QtWebUtils.

QVector<emscripten::val> popups;

void openPopup(std::string source)
{
    QString name = QString("popup %1").arg(popups.count()); // note: the name identifies the window and must be unique.
    QSize size(640, 480);
    qtwebutils::OpenWindowMode mode = qtwebutils::OpenWindowMode::Window;
    emscripten::val popup = qtwebutils::openBrowserWindow(QString::fromStdString(source), name, size, mode);
    popups.append(popup);
}

void closePopup()
{
    if (popups.count() == 0)
        return;
    qtwebutils::closeBrowserWindow(popups.takeLast());
}

EMSCRIPTEN_BINDINGS(testapp) {
    emscripten::function("openPopup", &openPopup);
    emscripten::function("closePopup", &closePopup);
}
