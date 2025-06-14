cmake_minimum_required(VERSION 3.5)
project(plot2D)

if(WIN32)
# Configuración específica para Windows
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPLATFORM_WINDOWS")
    set(LIBRARIES raylib winmm gdi32)
elseif(UNIX)
# Configuración específica para Linux
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPLATFORM_LINUX")
    find_package(raylib REQUIRED)
    set(LIBRARIES raylib pthread m)
endif()

# Archivo fuente principal
add_executable(plot2D src/main.c)

# Vinculación de bibliotecas
target_link_libraries(plot2D ${LIBRARIES})

