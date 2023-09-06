#include <QtCore>
#include <algorithm>
#include <emscripten/bind.h>
#include <emscripten/val.h>

// This example demonstrates how to interop with JavaScript code
// and data from C++ using Emscripten bind.h and val.h.

// Sample C++ function. This function is exported to JavaScript using
// EMSCRIPTEN_BINDINGS below.
void helloCpp(std::string name)
{
    qDebug() << "Hello" << QString::fromStdString(name) << "from helloCpp";

    // Call JS function. See defenition in ../examples/jsinterop/jsinterop.cpp
    // The function is accessed via the global "window" object.
    emscripten::val window = emscripten::val::global("window");
    window.call<void>("helloJs", name);

    // Copy data from a JavaScript Array. This copies from the
    // JavaScript heap to the C++ heap.
    QByteArray data = QByteArray::fromEcmaUint8Array(window["data"]);
    qDebug() << "Got data bytes" << QString::number(data[0]) << QString::number(data[1])
                                 << QString::number(data[2]) << QString::number(data[3]);

    // Clear Js-side data, if we don't want to hold a copy there
    window.set("data", emscripten::val::null());
    
    // Or assign back to it
    std::reverse(data.begin(), data.end());
    window.set("data", data.toEcmaUint8Array());
}

EMSCRIPTEN_BINDINGS(qt_webutils) {
    emscripten::function("helloCpp", &helloCpp);
}
