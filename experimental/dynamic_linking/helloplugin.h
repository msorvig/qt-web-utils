#ifndef HELLOPLUGIN_H
#define HELLOPLUGIN_H

#include <QtCore>

class HelloInterface
{
public:
    virtual void hello(const QString &name) const = 0;
};
Q_DECLARE_INTERFACE(HelloInterface, "hello/1.0");

#endif // HELLOPLUGIN_H
