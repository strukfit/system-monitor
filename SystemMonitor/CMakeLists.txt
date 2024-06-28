cmake_minimum_required(VERSION 3.16)
project(${CMAKE_PROJECT_NAME} LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Widgets
)
qt_standard_project_setup()

# exe
file(GLOB_RECURSE SOURCES "src/*.cpp"  "include/*.h" "main.cpp")
qt_add_executable(${CMAKE_PROJECT_NAME}  ${SOURCES})
# exe

#set_target_properties(${CMAKE_PROJECT_NAME}
#    PROPERTIES
#        WIN32_EXECUTABLE TRUE
#)


target_link_libraries(${CMAKE_PROJECT_NAME}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "include")

# Resources copy
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/Resources)
set(DESTINATION_DIR ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME})
file(COPY ${SOURCE_DIR} DESTINATION ${DESTINATION_DIR})
# Resources copy