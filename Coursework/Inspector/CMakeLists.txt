cmake_minimum_required(VERSION 3.16)
project(AirportInspector)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Charts)


qt_add_executable(AirportInspector
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    DatabaseManager.cpp
    DatabaseManager.h
    StatisticsDialog.cpp
    StatisticsDialog.h
    StatisticsDialog.ui
)


target_link_libraries(AirportInspector PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
)


set_target_properties(AirportInspector PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
)
