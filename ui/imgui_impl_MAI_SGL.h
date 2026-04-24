#pragma once

#include "../imgui/imgui.h"
#include "../MAI_SGL/math/math.h"

struct MaiImGuiVertex
{
    mai::vec4f pos;
    mai::vec4f color;
    mai::vec2f uv;
};

struct ImGuiSubmittedStats
{
    uint64_t _draw_lists{ 0 };
    uint64_t _draw_calls{ 0 };
    uint64_t _vertices{ 0 };
    uint64_t _indices{ 0 };
    uint64_t _triangles{ 0 };
};

bool ImGui_Impl_MAI_SGL_Init();
void ImGui_Impl_MAI_SGL_Shutdown();
void ImGui_Impl_MAI_SGL_NewFrame();
void ImGui_Impl_MAI_SGL_RenderDrawData(ImDrawData* data);
ImGuiSubmittedStats ImGui_Impl_MAI_SGL_GetSubmittedStats();
