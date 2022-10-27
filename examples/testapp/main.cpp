// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include <QtGui>
#include "rasterwindow.h"

QHash<QScreen *, QWindow *> g_windows;
QGuiApplication *g_app = nullptr;

void addWindow(QScreen *screen);
void removeWindow(QScreen *screen);

int main(int argc, char **argv)
{
    // Qt for WebAassembly supports the standard Qt application startup
    // approach where the application assumes the presence of at least
    // one screen, and then creates and shows an applicaiton window:
    //
    // RasterWindow window;
    // window.show();
    //
    // This example is slightly different and implements dynamic screen handling,
    // where it supports starting up with no screens and maintains one window per
    // screen as screens are added or removed.
    //
    // Note that "screen" on Qt for WebAssembly corresponds to an html container
    // element, not to a physical screen.

    g_app = new QGuiApplication(argc, argv);

    // Add windows for screens present at startup
    for (QScreen *screen: QGuiApplication::screens())
        addWindow(screen);
    
    // Maintain windows as screens are added and removeds
    QObject::connect(g_app, &QGuiApplication::screenAdded, addWindow);
    QObject::connect(g_app, &QGuiApplication::screenRemoved, removeWindow);

    // Normally, the app would call app.exec() here, however this is not required
    // on Qt for WebAssemmbly. Emscripten keeps the application instance alive also
    // after main has returned.
    return 0;
}

// Adds a (fullscreen) window for the given screen
void addWindow(QScreen *screen)
{
    Q_ASSERT(!g_windows.contains(screen));
    QWindow *window = new RasterWindow();
    window->setScreen(screen);
    window->show();
    g_windows.insert(screen, window);
}

// Removes the window for the given screen
void removeWindow(QScreen *screen)
{
    delete g_windows.take(screen);
}

