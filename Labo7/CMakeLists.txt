cmake_minimum_required(VERSION 3.16)
project(Labo7 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Activar automatizaciones esenciales de Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(Labo7
    main.cpp
    Labo7.cpp
    Labo7.h
    Labo7.ui  
    Labo7.qrc
    ArchivosContactos.h 
    ArchivosContactos.cpp)

target_link_libraries(Labo7 PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)