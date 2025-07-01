cmake_minimum_required(VERSION 3.16)

project(mpb VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Quick Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Quick Widgets Multimedia MultimediaWidgets)

add_subdirectory(src)

install(FILES mpb.desktop DESTINATION share/applications)
install(FILES data/icons/hicolor/512x512/apps/mpb.png DESTINATION share/icons/hicolor/512x512/apps)

