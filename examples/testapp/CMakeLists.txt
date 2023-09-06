cmake_minimum_required(VERSION 3.16)
project(testapp LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui)

qt_add_executable(testapp
    main.cpp
    rasterwindow.cpp
    popup.cpp
    jsinterop.cpp
    ../../qtwebutils.cpp
)

target_link_libraries(testapp PUBLIC
    Qt::Core
    Qt::Gui
    Qt::GuiPrivate # requied by QWasmLocalFileAccess in qtwebutils.cpp
)

include_directories("../../")
