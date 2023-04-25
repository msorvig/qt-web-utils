#include "qtwebutils.h"

#include <QtCore/qfile.h>
#include <QtCore/qsize.h>
#include <QtCore/qhash.h>
#include <QtGui/private/qwasmlocalfileaccess_p.h>

#include <emscripten/val.h>
#include <emscripten/bind.h>

/*!
    Displays a native file dialog where the user can select a file from
    the local file system.

    The file is copied \a destinationFilePath, which must be a valid file
    path on the file system provided by Emscripten. This will typicallly
    be an in-memory temporary file system. Files on this file system can
    be access using standard file API such as QFile.

    The \a fileReady callback will be called when file load is complete. It
    provides two arguments: the first on is the destination file path (identical
    to the destinationFilePath argument), the second is name of the file
    selected by the user.
*/
bool qtwebutils::loadFileToFileSystem(const QString &accept, const QString &destinationFilePath,
                                    std::function<void(const QString &, const QString &)> fileReady)
{
    struct LoadFileData {
        QString name;
        QByteArray buffer;
    };
    auto fileData = new LoadFileData();

    QWasmLocalFileAccess::openFile(accept.toStdString(),
        [](int fileCount) { Q_ASSERT(fileCount == 1); },
        [fileData](uint64_t size, const std::string name) -> char * {
            fileData->name = QString::fromStdString(name);
            fileData->buffer.resize(size);
            return fileData->buffer.data();
        },
        [fileData, destinationFilePath, fileReady](){
            QFile destinationFile(destinationFilePath);
            destinationFile.open(QIODevice::WriteOnly);
            destinationFile.write(fileData->buffer);
            destinationFile.close();
            fileReady(destinationFilePath, fileData->name);
            delete fileData;
        }
    );
    return true;
}

/*!
    Saves a file to the local file system.

    The \a sourceFilePath is on the in-sandbox file system provided by Emscripten.
*/
void qtwebutils::saveFileFromFileSystem(const QString &sourceFilePath, const QString &fileNameHint)
{
    QFile sourceFile(sourceFilePath);
    sourceFile.open(QIODevice::ReadOnly);
    QByteArray content = sourceFile.readAll();
    QWasmLocalFileAccess::saveFile(content.constData(), size_t(content.size()), fileNameHint.toStdString());
}

/*!
    Toggles fullscreen state for the global document.

    This function can be used to toggle a browser window
    or tab between the normal state and fullscreen state.

    On Qt for WebAssembly the application window is embedded
    in the web document. This means that it does not control
    any native window, and that it may allready be in fullscreen
    state where it does not have window decorations. Using
    e.g. QWindow::setWindowState() will not change the state
    of the browser window.

    Qt for WebAssembly
*/
void qtwebutils::toggleFullscreen()
{
    using emscripten::val;
    const val document = val::global("document");
    const val fullscreenElement = document["fullscreenElement"];
    if (fullscreenElement.isUndefined() || fullscreenElement.isNull())
        document["documentElement"].call<void>("requestFullscreen");
    else
        document.call<void>("exitFullscreen");
}

/*!
    Enables or disables tab close confirmation.

    Setting this to true will make the browser display a popup on tab close,
    asking the user to confirm that they want to prceeed. The wording of
    the dialog is determined by the browser, but will typically include
    a statement to the effect that there are unsaved changes which will be
    lost.

    This is implemented by setting a beforeunload handler on the window.
*/
void qtwebutils::enableTabCloseConfirmation(bool enable)
{
    using emscripten::val;
    const val window = val::global("window");
    const bool capture = true;
    const val eventHandler = val::module_property("qt_webutils_beforeUnloadHandler");
    if (enable) {
        window.call<void>("addEventListener", std::string("beforeunload"), eventHandler, capture);
    } else {
        window.call<void>("removeEventListener", std::string("beforeunload"), eventHandler, capture);
    }
}

namespace {
     void beforeUnloadhandler(emscripten::val event) {
         // Adding this event handler is sufficent to make the browsers display
         // the confirmation dialog, provided the calls below are also made:
         event.call<void>("preventDefault"); // call preventDefault as required by standard
         event.set("returnValue", std::string(" ")); // set returnValue to something, as required by Chrome
    }
}

EMSCRIPTEN_BINDINGS(qt_webutils) {
    function("qt_webutils_beforeUnloadHandler", &beforeUnloadhandler);
}

/*!
    Opens a new browser window or tab.

    \a source is the url to the source code for the new tab.
    \a name is the identifier for the window, and must be unique
    \a size is the requested size for the window
    \a mode is the open mode: Window or Tab

    Returns a handle to the window as an emscripten::val. The handle
    is a WindowProxy, which is again a wrapper around a Window.

    See related documentation:
        https://developer.mozilla.org/en-US/docs/Web/API/Window/open
        https://developer.mozilla.org/en-US/docs/Glossary/WindowProxy
        https://developer.mozilla.org/en-US/docs/Web/API/Window
*/
emscripten::val qtwebutils::openBrowserWindow(const QString& source, const QString &name, QSize size, qtwebutils::OpenWindowMode openMode)
{
    QString popup = openMode != qtwebutils::OpenWindowMode::Window ? QString("Popup") : QString("");
    QString features = QString("height=%2, width=%1 ").arg(size.width()).arg(size.height()) + popup;
    emscripten::val popupWindow = emscripten::val::global("window")
        .call<emscripten::val>("open", source.toStdString(), name.toStdString(),
                               features.toStdString());
    return popupWindow;
}

/*!
    Closes a browser window by calling close() on it.
*/
void qtwebutils::closeBrowserWindow(emscripten::val window)
{
    window.call<void>("close");
}

QtWebUtils::QtWebUtils(QObject *parent)
:QObject(parent)
{

}

bool QtWebUtils::loadFileToFileSystem(const QString &accept, const QString &destinationFilePath, const QJSValue &fileReady)
{
    auto ready = [destinationFilePath, fileReady](const QString &filePath, const QString &fileName) {
        QJSValueList args = { QJSValue(destinationFilePath), QJSValue(fileName) };
        fileReady.call(args);
    };
    return qtwebutils::loadFileToFileSystem(accept, destinationFilePath, ready);
}

void QtWebUtils::saveFileFromFileSystem(const QString &sourceFilePath, const QString &fileNameHint)
{
    qtwebutils::saveFileFromFileSystem(sourceFilePath, fileNameHint);
}

void QtWebUtils::toggleFullscreen()
{
    qtwebutils::toggleFullscreen();
}

void QtWebUtils::enableTabCloseConfirmation(bool enable)
{
    qtwebutils::enableTabCloseConfirmation(enable);
}

typedef QHash<QString, emscripten::val> WindowHash;
Q_GLOBAL_STATIC(WindowHash, g_windows); // (use global static to keep it out of the header)

// QString-based API for Qt Quick, which returns the window name instead of an emscritpen::val
QString QtWebUtils::openBrowserWindow(const QString& source, const QString &name, QSize size, bool createWindow)
{
    qtwebutils::OpenWindowMode mode = createWindow ? qtwebutils::OpenWindowMode::Window : qtwebutils::OpenWindowMode::Tab;
    emscripten::val window = qtwebutils::openBrowserWindow(source, name, size, mode);
    g_windows->insert(name, window);
    return name;
}

Q_INVOKABLE void QtWebUtils::closeBrowserWindow(const QString &name)
{
    emscripten::val window = g_windows->take(name);
    qtwebutils::closeBrowserWindow(window);
}
