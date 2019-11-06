cmake_minimum_required(VERSION 3.1)

project(processing3d LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
find_package(Qt5 COMPONENTS Core Quick REQUIRED)
find_package(OpenCV REQUIRED)

#set (VTK_DIR "/home/wayne/github/build-VTK" CACHE PATH "VTK directory override" FORCE)
#set (VTK_DIR "/home/wayne/vtk/build-vtk-8.1.2" CACHE PATH "VTK directory override" FORCE)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set (PCLVERSION 1.9)
if(PCLVERSION VERSION_LESS 1.9)
    set(CMAKE_CXX_STANDARD 11)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
else()
    set(CMAKE_CXX_STANDARD 14)
endif()


pkg_check_modules(PCL
    pcl_io-${PCLVERSION}
    pcl_common-${PCLVERSION}
    pcl_octree-${PCLVERSION}
    pcl_filters-${PCLVERSION}
    pcl_outofcore-${PCLVERSION}
    pcl_visualization-${PCLVERSION}
    )
pkg_check_modules(Assimp assimp REQUIRED)
#pkg_check_modules(OpenCV opencv REQUIRED)

find_package(Boost REQUIRED COMPONENTS filesystem system)
#find_package(PCL REQUIRED)

include_directories(
    ${PCL_INCLUDE_DIRS}
    ${VTK_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    )
#add_definitions(${PCL_DEFINITIONS})
find_package(PDAL REQUIRED)
#include_directories(${PDAL_INCLUDE_DIRS})
#link_directories(${PDAL_LIBRARY_DIRS})
add_definitions(${PDAL_DEFINITIONS})


file(GLOB SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/vtk/*.cpp
    ${PROJECT_SOURCE_DIR}/src/opengl/*.cpp
    ${PROJECT_SOURCE_DIR}/src/video/*.cpp
    ${PROJECT_SOURCE_DIR}/src/core/*.cpp
    ${PROJECT_SOURCE_DIR}/*.cpp
    )

add_executable(${PROJECT_NAME} ${SRC_FILES} "qml.qrc")

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core Qt5::Quick
    ${OpenCV_LIBRARIES}
    ${VTK_LIBRARIES}
    ${PCL_LIBRARIES}
    ${PDAL_LIBRARIES}
    ${Boost_LIBRARIES}
    )
