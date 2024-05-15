project(dynamicwidgets)
cmake_minimum_required(VERSION 3.19)

find_package(Qt6 COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

qt_add_executable(dynamicwidgets
    main.cpp
)

target_link_libraries(dynamicwidgets PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

install(TARGETS dynamicwidgets
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION .
)

qt_generate_deploy_app_script(
    TARGET dynamicwidgets
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# Copy plugin preload files (empty qml_imports in this case) to build directory.
configure_file(qt_plugins.json qt_plugins.json COPYONLY)
configure_file(qt_qml_imports.json qt_qml_imports.json COPYONLY)
configure_file(qt.conf qt.conf COPYONLY)
