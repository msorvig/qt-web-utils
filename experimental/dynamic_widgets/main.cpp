#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QWidget widget;
    QLabel *label = new QLabel("Hello World from dynamically linked Qt Widgets");
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label);
    widget.setLayout(layout);
    widget.show();
    
    return app.exec();
}
