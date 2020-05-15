cmake_minimum_required(VERSION 3.10.1)

project(trojan-qt5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 5.13 COMPONENTS Core Gui Network Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(DEFINED ENV{_TROJAN_QT5_BUILD_INFO_})
    set(_TROJAN_QT5_BUILD_INFO_STR_ "$ENV{_TROJAN_QT5_BUILD_INFO_STR_}")
else()
    set(_TROJAN_QT5_BUILD_INFO_STR_ "Trojan-Qt5 build from local machine")
endif()

add_definitions(-D_TROJAN_QT5_BUILD_INFO_STR_="${_TROJAN_QT5_BUILD_INFO_STR_}")

set(TROJAN-QT5_SOURCES
)

if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x600)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32 userenv iphlpapi)
    target_link_libraries(${PROJECT_NAME} Crypt32 kernel32 psapi user32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

if(APPLE)
    find_library(COCOA NAMES Cocoa)
    find_library(SECURITY NAMES Security)
    find_library(APPKIT NAMES AppKit)
    find_library(CARBON NAMES Carbon)
    find_library(FOUNDATION NAMES Foundation)
endif()

if(UNIX AND NOT APPLE AND NOT WIN32)
    find_package(Qt5 5.13 COMPONENTS dbus REQUIRED)
endif()