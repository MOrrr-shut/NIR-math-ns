cmake_minimum_required(VERSION 3.19)
project(NIR2 LANGUAGES CXX)

# Включаем автоматические системы Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Ищем компоненты Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Charts)

qt_standard_project_setup()

# Список исходных файлов (заголовочные файлы не нужно добавлять в qt_add_executable)
qt_add_executable(NIR2
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    basedata.cpp
    parametr.cpp
    readcsv.cpp
    regressia.cpp
    Bitcoin_history_data.csv
    ai.h
    ai.cpp
    mainwindow.h
    basedata.h
    parametr.h
    readcsv.h
    regressia.h
    ../../institut/work/NIR6/lin_regression.cpp ../../institut/work/NIR6/lin_regression.h ../../institut/work/NIR6/regression.cpp ../../institut/work/NIR6/regression.h
    comparison.h comparison.cpp
)

# Правильное подключение библиотек Qt6
target_link_libraries(NIR2
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Charts
)

include(GNUInstallDirs)
install(TARGETS NIR2
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET NIR2
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
