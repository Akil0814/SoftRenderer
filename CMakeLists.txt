#需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

#本工程的名字
project(Graphic)

#本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(application)
add_subdirectory(gpu)

#本工程所有cpp文件编译链接，生成exe
add_executable(SoftRenderer "main.cpp" "GPU/gpu.h" "GPU/gpu.cpp" "GPU/frame_buffer.h" "GPU/frame_buffer.cpp")

target_link_libraries(SoftRenderer applicationLib gpuLib)