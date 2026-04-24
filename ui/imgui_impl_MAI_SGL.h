#include "../imgui/imgui.h"
#include"../imgui/imgui_impl_win32.h"

#include "../MAI_SGL/math/math.h"


struct MaiImGuiVertex
{
    mai::vec4f pos;
    mai::vec4f color;
    mai::vec2f uv;
};

bool ImGui_Impl_MAI_SGL_Init();
void ImGui_Impl_MAI_SGL_NewFrame();
void ImGui_Impl_MAI_SGL_RenderDrawData(ImDrawData* data);

