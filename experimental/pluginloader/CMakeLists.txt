project(pluginloader)
cmake_minimum_required(VERSION 3.19)

find_package(Qt6 COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(pluginloader
    main.cpp
    helloplugin.cpp
    prepareplugin.cpp
)

target_link_libraries(pluginloader PUBLIC
    Qt::Core
)

install(TARGETS pluginloader
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION .
)

qt_generate_deploy_app_script(
    TARGET pluginloader
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# Copy plugin preload files (empty files in this case) to build directory.
configure_file(qt_plugins.json qt_plugins.json COPYONLY)
configure_file(qt_qml_imports.json qt_qml_imports.json COPYONLY)

# 'hello' plugin
qt_add_plugin(helloplugin SHARED
    CLASS_NAME HelloPlugin
    helloplugin.cpp helloplugin.h
)
target_link_libraries(helloplugin PUBLIC
    Qt::Core
)
target_compile_definitions(helloplugin PRIVATE BUILD_HELLO_PLUGIN)

