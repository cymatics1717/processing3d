cmake_minimum_required(VERSION 3.1)

project(processing3d LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
find_package(Qt5 COMPONENTS Core Quick REQUIRED)
find_package(OpenCV REQUIRED)

#set (VTK_DIR "/home/wayne/github/build-VTK-master" CACHE PATH "VTK directory override" FORCE)


find_package(VTK ${VTKVERSION} EXACT REQUIRED
    COMPONENTS
    vtkInteractionImage
    vtkRenderingExternal
    vtkGUISupportQt
    vtkIOPLY
    vtkRenderingLOD
    vtkIOGeometry
    vtkFiltersTexture
#    vtkRenderingContextOpenGL2
#    vtkRenderingVolumeOpenGL2
   )

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

find_package(PDAL REQUIRED)
add_definitions(${PDAL_DEFINITIONS})

include_directories(
    ${PCL_INCLUDE_DIRS}
    ${VTK_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${PDAL_INCLUDE_DIRS}
    )
#add_definitions(${PCL_DEFINITIONS})
#link_directories(${PDAL_LIBRARY_DIRS})

file(GLOB SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/eventbus.cpp
    ${PROJECT_SOURCE_DIR}/src/vtk/cellpickinteractorstyle.cpp
    ${PROJECT_SOURCE_DIR}/src/vtk/modelmanager.cpp
    ${PROJECT_SOURCE_DIR}/src/vtk/qvtkfboitem.cpp
    ${PROJECT_SOURCE_DIR}/src/vtk/model.cpp
    ${PROJECT_SOURCE_DIR}/src/vtk/proppickinteractorstyle.cpp
    ${PROJECT_SOURCE_DIR}/src/vtk/qvtkfborenderer.cpp
    ${PROJECT_SOURCE_DIR}/src/opengl/openglobject.cpp
    ${PROJECT_SOURCE_DIR}/src/opengl/openglquickitem.cpp
    ${PROJECT_SOURCE_DIR}/src/opengl/pclobject.cpp
    ${PROJECT_SOURCE_DIR}/src/opengl/rendererproxy.cpp
    ${PROJECT_SOURCE_DIR}/src/video/videoitem.cpp
    ${PROJECT_SOURCE_DIR}/src/video/videograbber.cpp
    ${PROJECT_SOURCE_DIR}/src/core/master.cpp
    ${PROJECT_SOURCE_DIR}/src/core/worker.cpp
    )

add_executable(${PROJECT_NAME} ${SRC_FILES} "qml.qrc")

#target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core Qt5::Quick
    ${OpenCV_LIBRARIES}
    ${VTK_LIBRARIES}
    ${PCL_LIBRARIES}
    ${PDAL_LIBRARIES}
    ${Boost_LIBRARIES}
    )
