#include <QtCore>

#include "helloplugin.h"
#include "prepareplugin.h"

// This example demonstrates how to use QPluginLoader to load a plugin
// on demand on Qt for WebAsssembly. The plugin will be downloaded from
// the serverat the time the loadPlugin() function is called.

// This is an alternative to preloading pluigns, and adds a requirement
// that the application must make a call to the asynchronous preparePlugin()
// function.


void loadPlugin(const QString &pluginName)
{
    // Prepare the plugin; this initiates an async download (using emscripten_dlopen()),
    // and calls the completion handler when ready.
    preparePlugin(pluginName, [pluginName](PreparePluginResult result) {

        if (result != PreparePluginResult::Ok) {
            qDebug() << "Error: plugin prepare filed for plugin" << pluginName << "with error" << result;
            return;
        }

        // Call standard Qt loading code
        QPluginLoader loader(pluginName);
        QObject *plugin = loader.instance();
        if (plugin) {
            HelloInterface *helloPlugin = qobject_cast<HelloInterface *>(plugin);
            if (helloPlugin)
                helloPlugin->hello("Qt");
            else
                qDebug() << "Plugin does not export the HelloInterface interface.";
        } else {
            qDebug() << "Failed to load the plugin.";
        }

    });
}

int main(int argc, char *argv[])
{
    // Optionall enable some logging output
    QStringList loggingRules = {
        "qt.core.plugin.*.debug=true",
        "qt.core.library.debug=true",
    };
    //QLoggingCategory::setFilterRules(loggingRules.join("\n"));

    QCoreApplication app(argc, argv);

    qDebug() << "main()";

    loadPlugin("helloplugin");

    app.exec();
}
