#ifndef PREPAREPLUGIN_H
#define PREPAREPLUGIN_H

#include <QtCore>

enum PreparePluginResult
{
    Ok,
    FetchError,
    DlopenError,
};

void preparePlugin(const QString &sourcePath, const QString &pluginName, std::function<void (PreparePluginResult)> prepareComplete);

#endif // PREPAREPLUGIN_H
