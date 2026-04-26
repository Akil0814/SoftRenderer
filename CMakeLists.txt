#需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

#本工程的名字
project(Graphic)

#本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/utf-8)
endif()

#把需要拷贝的资源路径都放到ASSETS里
file(GLOB ASSETS "./assets")
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(application)
add_subdirectory(MAI_SGL)
add_subdirectory(MAIframework)
add_subdirectory(imgui)
add_subdirectory(ui)

#本工程所有cpp文件编译链接，生成exe
add_executable(SoftRenderer "main.cpp")

target_compile_definitions(SoftRenderer PRIVATE SOFTWARE_RENDERING_ROOT_DIR="${CMAKE_SOURCE_DIR}")

target_link_libraries(SoftRenderer applicationLib fw ui MAI_SGL_Lib imguilib)
