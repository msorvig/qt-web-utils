# qt-web-utils

Utilities and Helper functions for Qt on the Web

# Content

* toggleFullscreen()

  Toggles the browser in and out of fullscreen mode.
  
* enableTabCloseConfirmation(bool enable)

  Eables "unsaved data" confirmation dialog on tab close.

* loadFileToFileSystem() / saveFileFromFileSystem()

  Loads file from the local file system via file dialog; saves to the in-memory file system
  provided by Emscripten. See also QFileDialog::getOpenFileContents() in Qt.
