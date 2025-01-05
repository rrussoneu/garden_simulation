cmake_minimum_required(VERSION 3.20)
project(garden_simulation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

set(SOURCES
        src/main.cpp
        src/view/mainwindow.cpp
        src/view/gardenglwidget.cpp
        src/renderer/shader.cpp
        src/renderer/camera.cpp
        src/model/model.cpp

)

set(HEADERS
        src/view/mainwindow.h
        src/view/gardenglwidget.h
        src/renderer/camera.h
        src/renderer/shader.h
        src/model/model.h
        src/plant.cpp
        src/plant.h
        src/view/plantdragbutton.cpp
        src/view/plantdragbutton.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::OpenGLWidgets
        OpenGL::GL
        assimp::assimp
)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
endif()

# Copy shader files to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})