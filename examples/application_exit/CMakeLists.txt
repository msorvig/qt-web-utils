cmake_minimum_required(VERSION 3.16)

project(application_exit)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.6)

qt_add_executable(application_exit
    main.cpp
)

qt_add_qml_module(application_exit
    URI applicationexit
    VERSION 1.0
    QML_FILES Main.qml
)

target_link_libraries(application_exit
    PRIVATE Qt6::Quick
)

