project(dynamicquick)
cmake_minimum_required(VERSION 3.19)

find_package(Qt6 COMPONENTS Core Gui Quick)

qt_standard_project_setup()

qt_add_executable(dynamicquick
    main.cpp
)

target_link_libraries(dynamicquick PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Quick
)

qt_add_resources(dynamicquick "qml"
    PREFIX "/"
    FILES
        hello.qml
)

install(TARGETS dynamicquick
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION .
)

qt_generate_deploy_app_script(
    TARGET dynamicquick
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# Copy plugin preload files to the build directory.
configure_file(qt_plugins.json qt_plugins.json COPYONLY)
configure_file(qt_qml_imports.json qt_qml_imports.json COPYONLY)
configure_file(qt.conf qt.conf COPYONLY)
