#include "helloplugin.h"
#include <QtCore>

class HelloPlugin : public QObject, public HelloInterface
{
    Q_OBJECT
    Q_INTERFACES(HelloInterface)
    Q_PLUGIN_METADATA(IID "com.example.hello" FILE "hello.json")
    void hello(const QString &name) const
    {
        qDebug() << "Hello" << name << "from a on-demand loaded plugin";
    }
};

#include "helloplugin.moc"
