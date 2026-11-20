cmake_minimum_required(VERSION 3.16)
project(LibraryManagementApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tự động xử lý Signals/Slots (MOC), file thiết kế Qt Designer (UIC) và tài nguyên (RCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
    main.cpp
    LibraryModels.h
    DatabaseManager.h
    DatabaseManager.cpp
    Dialogs.h
    Dialogs.cpp
    MainWindow.h
    MainWindow.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(LibraryManagementApp MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(LibraryManagementApp ${PROJECT_SOURCES})
endif()

target_link_libraries(LibraryManagementApp PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LibraryManagementApp)
endif()