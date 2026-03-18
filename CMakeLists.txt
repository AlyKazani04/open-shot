cmake_minimum_required(VERSION 3.15)
project(open-shot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fdiagnostics-color=always -g)


find_package(OpenCV REQUIRED)
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)


set(SRC_ROOT "src")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${SRC_ROOT}/**.cpp")
add_executable(open-shot ${SOURCES})


target_include_directories(open-shot PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${SRC_ROOT}
)


if(UNIX)
    message(STATUS "Configuring for Linux...")
    target_link_libraries(open-shot PRIVATE 
        SFML::Graphics 
        SFML::Window 
        SFML::System 
        opencv_core      
        opencv_imgproc   
        opencv_highgui   
        opencv_imgcodecs
        GL
    )
endif()


set_target_properties(open-shot PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
