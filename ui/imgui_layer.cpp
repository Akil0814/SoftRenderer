#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "../application/application.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../MAI_SGL/core/gpu.h"

#include "imgui_impl_MAI_SGL.h"
#include "imgui_layer.h"

bool init_imgui_for_MAI_SGL()
{
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    if (!ImGui_ImplWin32_Init(MAI_APP->get_hwnd()))
        return false;

    if (!ImGui_Impl_MAI_SGL_Init())
        return false;

    return true;
}

void shutdown_imgui_for_MAI_SGL()
{
    if (ImGui::GetCurrentContext() == nullptr)
        return;

    ImGui_Impl_MAI_SGL_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void rend_imgui()
{
    static bool show_demo_window = false;
    static bool show_metrics_window = false;
    static bool show_style_editor = false;

    ImGui_Impl_MAI_SGL_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("MAI SGL");
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Text("Dear ImGui is rendering through MAI_SGL.");
    ImGui::Separator();

    ImGui::Text("Window: %u x %u", MAI_APP->get_width(), MAI_APP->get_height());
    ImGui::Text("Display: %.0f x %.0f", io.DisplaySize.x, io.DisplaySize.y);
    ImGui::Text("Framebuffer scale: %.2f, %.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::Text("Mouse: %.1f, %.1f", io.MousePos.x, io.MousePos.y);

    ImGui::Separator();
    ImGui::Text("Platform: %s", io.BackendPlatformName ? io.BackendPlatformName : "none");
    ImGui::Text("Renderer: %s", io.BackendRendererName ? io.BackendRendererName : "none");

    const mai::RenderStats stats = MAI_SGL->get_render_stats();
    ImGui::Separator();
    ImGui::Text("Renderer Stats");
    ImGui::Text("Draw Calls: %llu", static_cast<unsigned long long>(stats._frame_draw_calls));
    ImGui::Text("Triangles: %llu", static_cast<unsigned long long>(stats._frame_triangles));
    ImGui::Text("Vertices: %llu", static_cast<unsigned long long>(stats._frame_vertices));

    ImGui::End();

    ImGui::Render();
    ImGui_Impl_MAI_SGL_RenderDrawData(ImGui::GetDrawData());
}
