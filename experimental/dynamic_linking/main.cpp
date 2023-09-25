#include <QtGui>
#include <QtQml>

#include "helloplugin.h"
#include "prepareplugin.h"

// This file contains a WebAssembly shared libraries / dynamic linking example
// as well as debugging helpers .

// Basic debug window
class TestWindow : public QRasterWindow
{
public:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter p(this);
        p.fillRect(QRect(0, 0, 9999, 9999),  QColor::fromString("steelblue"));
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton)
            qDebug() << "Click at" << event->pos();
    }
};

// Test function which exercices the jpeg image format plugin
void saveJpeg()
{
    QImage image(320, 200, QImage::Format_RGB888);
    image.fill(QColor(Qt::green).rgb());
    bool saved = image.save("test.jpg", "JPEG");
    QFileInfo fileInfo("test.jpg");
    qDebug() << "Saved image using Jpeg plugin: status saved"  << saved << "file exists" << fileInfo.exists() << "file size" << fileInfo.size();
}

// Debug helper which prints the contents of a directory, recursively.
void printDirectoryContentsRecursively(const QString &dirPath) {
    QDirIterator iterator(dirPath, QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs | QDir::Hidden, QDirIterator::Subdirectories);

    while (iterator.hasNext()) {
        iterator.next();
        QFileInfo fileInfo = iterator.fileInfo();

        if (fileInfo.isDir()) {
            qDebug() << "Directory:" << fileInfo.absoluteFilePath();
        } else {
            qDebug() << "File:" << fileInfo.absoluteFilePath();
        }
    }
}

void loadPlugin(const QString &pluginName)
{
    QPluginLoader loader(pluginName);
    QObject *plugin = loader.instance();
    if (plugin) {
        HelloInterface *helloPlugin = qobject_cast<HelloInterface *>(plugin);
        if (helloPlugin)
            helloPlugin->hello("Bob");
        else
            qDebug() << "Plugin does not export the HelloInterface interface.";
    } else {
        qDebug() << "Failed to load the plugin.";
    }
}

// PluginLoader: exports loadPlugin() to QML
class PluginLoader : public QObject
{
    Q_OBJECT
public:
    explicit PluginLoader(QObject *parent = nullptr)
    : QObject(parent) { }

public slots:
    void loadPlugin();
};

void PluginLoader::loadPlugin()
{
    QString pluginName = "helloplugin";
    qDebug() << "Loading plugin" << pluginName;

    QString sourcePath; // empty (load relative to .html file)
    preparePlugin(sourcePath, pluginName, [pluginName](PreparePluginResult result){
        if (result == PreparePluginResult::Ok) {
            ::loadPlugin(pluginName);
        } else {
            qDebug() << "Error: plugin prepare filed for plugin" << pluginName << "with error" << result;
        }
    });
}

int main(int argc, char *argv[])
{

    QStringList loggingRules = {
        "qt.core.plugin.*.debug=true",
//        "qt.qpa.*.debug=true",
        "qt.qml.import.debug=true",
    };

//    QLoggingCategory::setFilterRules(loggingRules.join("\n"));

    qDebug() << "Hello world from main()";

    QGuiApplication *app = new QGuiApplication(argc, argv);

    //printDirectoryContentsRecursively("/");

    saveJpeg();

#if 0
    TestWindow *testWidnow = new TestWindow();
    testWidnow->show();
#endif

#if 1
    QQmlApplicationEngine *engine = new QQmlApplicationEngine("qrc:/hello.qml");
    PluginLoader *pluginLoader = new PluginLoader(); ;
    engine->rootContext()->setContextProperty("pluginLoader", pluginLoader);
#endif    
}

#include "main.moc"
