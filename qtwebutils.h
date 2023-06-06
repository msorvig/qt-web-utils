#ifndef QTWEBUTILS_H
#define QTWEBUTILS_H

#include <QtCore/QString>
#include <QtCore/QSize>
#include <QtCore/QObject>

#include <QtQml/QQmlEngine>

namespace emscripten {
    class val;
}

namespace qtwebutils {
    
    bool loadFileToFileSystem(const QString &accept, const QString &destinationFilePath,
                                        std::function<void(const QString &, const QString &)> loaded);
    void saveFileFromFileSystem(const QString &sourceFilePath, const QString &fileNameHint);

    void toggleFullscreen();
    void enableTabCloseConfirmation(bool enable);

    enum OpenWindowMode {
        Tab,
        Window
    };

    emscripten::val openBrowserWindow(const QString& source, const QString &name, QSize size, OpenWindowMode openMode);
    void closeBrowserWindow(emscripten::val window);
}

class QtWebUtils : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit QtWebUtils(QObject *parent = nullptr);

    Q_INVOKABLE bool loadFileToFileSystem(const QString &accept, const QString &destinationFilePath, const QJSValue &fileReady);
    Q_INVOKABLE void saveFileFromFileSystem(const QString &sourceFilePath, const QString &fileNameHint);

    Q_INVOKABLE void toggleFullscreen();
    Q_INVOKABLE void enableTabCloseConfirmation(bool enable);

    Q_INVOKABLE QString openBrowserWindow(const QString& source, const QString &name, QSize size, bool createWindow);
    Q_INVOKABLE void closeBrowserWindow(const QString &name);
};

#endif // QTWEBUTILS_H
