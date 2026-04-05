#需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

#本工程的名字
project(Graphic)

#本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)

#把需要拷贝的资源路径都放到ASSETS里
file(GLOB ASSETS "./assets")
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(application)
add_subdirectory(gpu)
add_subdirectory(imgui)

#本工程所有cpp文件编译链接，生成exe
add_executable(SoftRenderer "main.cpp")

target_link_libraries(SoftRenderer applicationLib gpuLib imguilib)