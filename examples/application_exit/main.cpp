#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <emscripten.h>

class Deleter;

QGuiApplication *g_app = nullptr;
QQmlApplicationEngine *g_engine = nullptr;

class Deleter : public QObject
{
    Q_OBJECT
public:
    explicit Deleter(QObject *parent = nullptr)
        : QObject(parent) {}

    Q_INVOKABLE void quit() {
        
        // QCoreApplication does not support deleting itself from inside
        // an event callback. Make a native zero-timer callback to make sure
        // we are deleting without Qt on the stack.
        emscripten_async_call([](void *deleter){
            
            // Delete self
            delete static_cast<Deleter *>(deleter);

            // Delete the QQmlApplicationEngine and QGuiApplication instances.
            delete g_engine; g_engine = nullptr;
            delete g_app; g_app = nullptr;

            // Optinally trigger runtime exit as well.
            emscripten_force_exit(0);
        }, this, 0);
    }
};

int main(int argc, char *argv[])
{
    // This example demonstrates how to cleanly exit a Qt application.
    //
    // This is done by creating the QML application engine and Qt application
    // on the heap, and then returning from main() without calling exec().
    //
    // Qt for WebAssembly supports omitting the exec() call, since the native 
    // event loop is already running by the time main() is called. Emscripten
    // supports returning from main() without exiting the runtime, see the
    // EXIT_RUNTIME linker option.
    
    g_app = new QGuiApplication(argc, argv);
    g_engine = new QQmlApplicationEngine();
    g_engine->loadFromModule("applicationexit", "Main");
    g_engine->rootContext()->setContextProperty("deleter", new Deleter);
}

#include "main.moc"
