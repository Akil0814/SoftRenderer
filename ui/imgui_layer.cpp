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
    const float main_scale = ImGui_ImplWin32_GetDpiScaleForHwnd(MAI_APP->get_hwnd());

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
    static mai::RenderStats last_completed_frame_stats{};
    static ImGuiSubmittedStats last_imgui_submitted_stats{};

    ImGui_Impl_MAI_SGL_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(MAI_APP->get_width()), static_cast<float>(MAI_APP->get_height()));
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    ImGui::NewFrame();

    ImGui::Begin("MAI SGL");
    const ImVec2 window_pos = ImGui::GetWindowPos();
    const ImVec2 window_size = ImGui::GetWindowSize();


    ImGui::Text("Window: %u x %u", MAI_APP->get_width(), MAI_APP->get_height());
    ImGui::Text("Display: %.0f x %.0f", io.DisplaySize.x, io.DisplaySize.y);
    ImGui::Text("Framebuffer scale: %.2f, %.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    ImGui::Text("FPS: %.1f", io.Framerate);

    ImGui::Separator();
    ImGui::Text("Platform: %s", io.BackendPlatformName ? io.BackendPlatformName : "none");
    ImGui::Text("Renderer: %s", io.BackendRendererName ? io.BackendRendererName : "none");

    bool cull_face = MAI_SGL->get_render_state()._enable_cull_face;

    if (ImGui::Checkbox("Cull Face", &cull_face))
    {
        if (cull_face)
            MAI_SGL->enable(MAI_CULL_FACE);
        else
            MAI_SGL->disable(MAI_CULL_FACE);
    }


    ImGui::End();

    ImGui::Render();
    ImGui_Impl_MAI_SGL_RenderDrawData(ImGui::GetDrawData());
    last_imgui_submitted_stats = ImGui_Impl_MAI_SGL_GetSubmittedStats();
    last_completed_frame_stats = MAI_SGL->get_render_stats();
}
