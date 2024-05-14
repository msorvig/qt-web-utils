#ifndef PREPAREPLUGIN_H
#define PREPAREPLUGIN_H

#include <QtCore>

enum PreparePluginResult
{
    Ok,
    FetchError,
    DlopenError,
};

void preparePlugin(const QString &fileName, std::function<void (PreparePluginResult)> prepareComplete);

#endif // PREPAREPLUGIN_H
