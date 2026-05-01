cmake_minimum_required(VERSION 4.1)
project(mownit-approx)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        PrintSupport
        REQUIRED)


add_executable(main main.cpp
        window/main_window.cpp
        window/main_window.h
        window/main_window.ui
        qcustomplot-source/qcustomplot.cpp)
target_link_libraries(main
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::PrintSupport
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot-source)
add_executable(analyze analyze.cpp)
