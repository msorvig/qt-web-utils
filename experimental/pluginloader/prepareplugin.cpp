#include "prepareplugin.h"

#include <emscripten/emscripten.h>
#include <dlfcn.h>

// preparePlugin() prepares a plugin for loading by Qt, and should be called before
// calling standard pluign loading API such ash QPluginLoader.
//
// This is an asynchronous operation, and the prepareComplete callback will be called
// when the plugin is ready.
//
// Arguments:
//
//  - pluginPathName: the name of the plugin, or the path to the plugin. See QPluginLoader::fileName.
//
//  - prepareComplete: A callback that will be called when the plugin is ready (or have failed to load).

void preparePlugin(const QString &fileName, std::function<void (PreparePluginResult)> prepareComplete)
{
    // prefix "lib" and postfix ".so" to plugin name if missing
    QFileInfo fileInfo(fileName);
    QString path = fileInfo.path();
    QString name = fileInfo.fileName();
    if (!name.startsWith("lib"))
        name.prepend("lib");
    if (!name.endsWith(".so"))
        name.append(".so");
    QString finalPath = QDir(path).filePath(name);

    struct Context {
        QByteArray pluginPath;
        std::function<void (PreparePluginResult)> prepareComplete;
    };
    Context *context = new Context {
        finalPath.toUtf8(),
        std::move(prepareComplete)
    };
    emscripten_dlopen(context->pluginPath.constData(), RTLD_NOW, context,
        [](void *ctx, void *handle) {
            Q_UNUSED(handle);
            Context *context = static_cast<Context *>(ctx);
            context->prepareComplete(PreparePluginResult::Ok);
            delete context;
        },
        [](void *ctx) {
            Context *context = static_cast<Context *>(ctx);
            context->prepareComplete(PreparePluginResult::DlopenError);
            delete context;
        }
    );
}
