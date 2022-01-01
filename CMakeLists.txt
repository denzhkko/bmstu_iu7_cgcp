cmake_minimum_required(VERSION 3.5)

project(deniska VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS OFF)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

find_package(Boost REQUIRED COMPONENTS log)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
add_definitions(-DBOOST_LOG_DYN_LINK)

find_package(OpenMP REQUIRED)

set(PROJECT_SOURCES
        src/mainwindow.ui

        src/mainwindow.h
        src/mainwindow.cpp

        src/manager_draw.h
        src/manager_draw.cpp

        src/vec3.h
        src/color.h
        src/color.cpp
        src/ray.h

        src/material.h
        src/hittable.h
        src/hittable_list.h
        src/sphere.h
        src/rtweekend.h
        src/sphere.h

        src/camera.h

        src/aabb.h
        src/aarect.h
        src/bvh.h
        src/box.h

        src/texture.h
        src/settings_render.h
        src/scene.h
        src/util.h

        widgets/ColorPicker.h
        widgets/ColorPicker.cpp
        )

add_executable(${PROJECT_NAME}
        src/main.cpp

        ${PROJECT_SOURCES}
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        widgets/
        )

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Threads::Threads
        ${Boost_LOG_LIBRARY}
        OpenMP::OpenMP_CXX
        )

set(TIME_MEA time_mea)

add_executable(${TIME_MEA}
        test/time_mea.cpp

        ${PROJECT_SOURCES}
        )

target_include_directories(${TIME_MEA} PUBLIC
        src/
        widgets/
        )

target_link_libraries(${TIME_MEA} PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Threads::Threads
        ${Boost_LOG_LIBRARY}
        OpenMP::OpenMP_CXX
        )
