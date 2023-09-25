#include "prepareplugin.h"

#include <emscripten/fetch.h>
#include <emscripten/emscripten.h>
#include <dlfcn.h>

// preparePlugin() prepares a plugin for loading by Qt, and should be called before
// calling standard pluign loading API such ash QPluginLoader.
// 
// The function downloads a plugin file to the in-memory ile system provided by
// Emsripten, then compiles and pre-loads it using emscripten_dlopen(). 
// 
// This is an asynchronous operation, and the prepareComplete callback will be called
// when the plugin is ready.
//
//  - sourcePath: The path to the directory containing the plugin file on the web server.
//                If empty, the plugin file will be loaded relative to the .html file.
//  - pluginName: The name of the plugin file, without the "lib" prefix or ".so" suffix.
//                For instance, "myplugin" will load "libmyplugin.so". This corresponds
//                to the prefix and suffix used by the Qt plugin loading API.
//
//  - prepareComplete: A callback that will be called when the plugin is ready (or have failed to load).

// FIXME: emscripten_dlopen() re-downloads the plugin file (though the second download will be cached).
void preparePlugin(const QString &sourcePath, const QString &pluginName, std::function<void (PreparePluginResult)> prepareComplete)
{
    // Add "lib" and ".so" to plugin name
    QString pluginFileName = pluginName;
    if (!pluginFileName.startsWith("lib"))
        pluginFileName.prepend("lib");
    if (!pluginFileName.endsWith(".so"))
        pluginFileName.append(".so");
    QString pluginFinalPath = sourcePath + pluginFileName;

    struct Context {
        QByteArray pluginPath;
        std::function<void (PreparePluginResult)> prepareComplete;
    };
    Context *context = new Context {
        pluginFinalPath.toUtf8(),
        std::move(prepareComplete)
    };

    // Async fetch the plugin; Qt plugin loading code expects to find the
    // plugin on the file system, and will not call dlopen() directly.
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.userData = context;
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = [](emscripten_fetch_t *fetch) {
        Context *context = static_cast<Context *>(fetch->userData);

        QByteArray data = QByteArray::fromRawData(fetch->data, fetch->numBytes);
        QFile file(context->pluginPath.constData());
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.close();
        emscripten_fetch_close(fetch);

        // Async dlopen() the plugin; Chrome refuses to sync compile wasm
        // binaries larger than ~50kb. By pre-loading it the dlopen() call
        // made by QPluginLoader will hit the already loaded library in the
        // cache and complete sucessfully on all browsers.
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
    };
    attr.onerror = [](emscripten_fetch_t *fetch) {
        Context *context = static_cast<Context *>(fetch->userData);
        emscripten_fetch_close(fetch);
        context->prepareComplete(PreparePluginResult::FetchError);
        delete context;
    };
    emscripten_fetch_t *fetch = emscripten_fetch(&attr, context->pluginPath.constData());
}
