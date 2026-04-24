# SoftwareRendering

SoftwareRendering 是一个基于 C++17 和 CMake 的软件渲染器项目，主要用于学习和实现基础图形渲染管线。项目核心模块 `MAI_SGL` 提供了类似简化 GPU 的绘制接口，包含顶点处理、裁剪、光栅化、片元处理、纹理采样、深度测试和混合等功能。

项目同时包含一个简单的 Windows 应用框架和 ImGui 调试界面，当前示例会创建窗口并渲染带纹理的几何图形，方便观察和调试渲染结果。

## 主要目录

- `MAI_SGL/`：软件渲染核心、数学库、资源对象和 shader 实现
- `application/`：窗口、消息循环和相机控制
- `ui/`：ImGui 接入与调试界面
- `assets/`：纹理等运行资源
- `doc/`：更新记录和优化计划

## 构建

项目使用 CMake 管理，生成工程后编译 `SoftRenderer` 即可运行。

```bash
cmake -S . -B build
cmake --build build
```
