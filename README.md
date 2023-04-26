# qt-web-utils

Utilities and Helper functions for Qt on the Web

# Utility API

The API is declared in qtwebutils.h and implemented in qtwebutils.cpp. The API is provided in
two flavours: as static functions in the qtwebutils namespace for use from C++, and as Q_INVOKABLE
functions on QtWebUtils singleton for use from QML

* toggleFullscreen()

  Toggles the browser in and out of fullscreen mode.
  
* enableTabCloseConfirmation(bool enable)

  Eables "unsaved data" confirmation dialog on tab close.

* loadFileToFileSystem() / saveFileFromFileSystem()

  Loads file from the local file system via file dialog; saves to the in-memory file system
  provided by Emscripten. See also QFileDialog::getOpenFileContents() in Qt.

# Examples

The examples are located in the exampels/ directory and demonstrates how to integrate with the web
platform, beyond what the default Qt build does.

* testapp

  Test application, used by by the examples.

* background_loading

  Shows how to load a Qt applicaiton in the background, without delaying page loading.

* multiple_instances

  Shows how to create multiple application instances from a single wasm module.

* multiple_windows_simple

  Shows how to display multiple application windows on a single html document.

* multiple_windows

  Shows how to add and reomove application windows on an html document.

* popup_window

  Shows how to create new tabs or popup windows, and how to render to them from a single WebAssembly instance.
