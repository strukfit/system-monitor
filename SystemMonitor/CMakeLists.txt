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

file(GLOB_RECURSE SOURCES "src/*.cpp"  "include/*.h" "main.cpp")
set(adlx_files "")

if(WIN32)
    set(CUDA_PATH "D:/CUDA_Toolkit_v12.5")
    set(NVML_NAMES nvml)
    set(NVML_LIB_DIR "${CUDA_PATH}/lib/x64")
    set(NVML_INCLUDE_DIR "${CUDA_PATH}/include")

    # .lib import library full path
    find_file(NVML_LIB_PATH
    NO_DEFAULT_PATH
    NAMES nvml.lib
    PATHS ${NVML_LIB_DIR})

    # .dll full path
    find_file(NVML_DLL_PATH
    NO_DEFAULT_PATH
    NAMES nvml.dll
    PATHS "C:/Windows/System32")

    find_path(NVML_INCLUDE_PATH
    NO_DEFAULT_PATH
    NAMES nvml.h
    PATHS ${NVML_INCLUDE_DIR})

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(NVML DEFAULT_MSG NVML_LIB_PATH NVML_INCLUDE_PATH)

    if(NVML_FOUND)
        list(APPEND NVML_INCLUDE_DIRS ${NVML_INCLUDE_PATH})
        list(APPEND NVML_LIBRARIES ${NVML_LIB_PATH})
    else()
        message(FATAL_ERROR "NVML library not found")
    endif()

    set(ADLX_PATH "D:/ADLX")

    include_directories(${ADLX_PATH})

    list(APPEND adlx_files
        ${ADLX_PATH}/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h
        ${ADLX_PATH}/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.cpp
        ${ADLX_PATH}/SDK/platform/Windows/WinAPIs.cpp)
endif()

# exe
qt_add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${adlx_files})

if(WIN32)
    target_include_directories(SystemMonitor PUBLIC ${NVML_INCLUDE_DIRS} ${ADLX_PATH})
    target_link_libraries(SystemMonitor PUBLIC ${NVML_LIBRARIES})
endif()

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

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE ${SUBSYSTEM_LINKER_OPTIONS})

# Resources copy
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/Resources)
set(DESTINATION_DIR ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME})
file(COPY ${SOURCE_DIR} DESTINATION ${DESTINATION_DIR})