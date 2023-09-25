project(qtquick)
cmake_minimum_required(VERSION 3.19)

find_package(Qt6 COMPONENTS Core Gui Quick)

qt_standard_project_setup()

qt_add_executable(dynamic_linking
    main.cpp
    helloplugin.h
    prepareplugin.cpp prepareplugin.h
)

target_link_libraries(dynamic_linking PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Quick
)

qt_add_resources(dynamic_linking "qml"
    PREFIX "/"
    FILES
        hello.qml
)

install(TARGETS dynamic_linking
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION .
)

qt_generate_deploy_app_script(
    TARGET dynamic_linking
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# 'hello' plugin
qt_add_plugin(helloplugin SHARED
    CLASS_NAME HelloPlugin
    helloplugin.cpp helloplugin.h
)
target_link_libraries(helloplugin PUBLIC
    Qt::Core
)
target_compile_definitions(helloplugin PRIVATE BUILD_HELLO_PLUGIN)

