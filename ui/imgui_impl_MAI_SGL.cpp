#include "imgui_impl_MAI_SGL.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "../MAI_SGL/base.h"
#include "../MAI_SGL/core/gpu.h"
#include "../MAI_SGL/core/render_state.h"
#include "../MAI_SGL/shader/texture_shader_2D.h"

namespace
{

struct ImGuiMaiSglBackendData
{
    mai::TextureShader2D* shader{ nullptr };
    uint32_t vao{ 0 };
    uint32_t vertex_vbo{ 0 };
    uint32_t index_ebo{ 0 };
};

ImGuiMaiSglBackendData* get_backend_data()
{
    return static_cast<ImGuiMaiSglBackendData*>(ImGui::GetIO().BackendRendererUserData);
}

mai::RGBA make_rgba(byte r, byte g, byte b, byte a)
{
    return mai::RGBA(r, g, b, a);
}

std::vector<mai::RGBA> convert_texture_pixels(ImTextureData* texture)
{
    std::vector<mai::RGBA> pixels(static_cast<size_t>(texture->Width) * static_cast<size_t>(texture->Height));

    if (texture->Format == ImTextureFormat_RGBA32)
    {
        const unsigned char* src = static_cast<const unsigned char*>(texture->GetPixels());
        for (size_t i = 0; i < pixels.size(); ++i)
        {
            const unsigned char* p = src + i * 4;
            pixels[i] = make_rgba(p[0], p[1], p[2], p[3]);
        }
    }
    else if (texture->Format == ImTextureFormat_Alpha8)
    {
        const unsigned char* src = static_cast<const unsigned char*>(texture->GetPixels());
        for (size_t i = 0; i < pixels.size(); ++i)
            pixels[i] = make_rgba(255, 255, 255, src[i]);
    }

    return pixels;
}

void update_texture(ImTextureData* texture)
{
    if (texture->Status == ImTextureStatus_WantCreate)
    {
        uint32_t texture_id = MAI_SGL->get_texture();
        std::vector<mai::RGBA> pixels = convert_texture_pixels(texture);

        MAI_SGL->bind_texture(texture_id);
        MAI_SGL->tex_image_2D(texture->Width, texture->Height, pixels.data());
        MAI_SGL->tex_parameter(MAI_TEXTURE_FILTER, MAI_TEXTURE_FILTER_LINEAR);
        MAI_SGL->tex_parameter(MAI_TEXTURE_WRAP_U, MAI_TEXTURE_WRAP_REPEAT);
        MAI_SGL->tex_parameter(MAI_TEXTURE_WRAP_V, MAI_TEXTURE_WRAP_REPEAT);
        MAI_SGL->bind_texture(0);

        texture->SetTexID(static_cast<ImTextureID>(texture_id));
        texture->SetStatus(ImTextureStatus_OK);
    }
    else if (texture->Status == ImTextureStatus_WantUpdates)
    {
        uint32_t texture_id = static_cast<uint32_t>(texture->GetTexID());
        if (texture_id == 0)
        {
            texture->SetStatus(ImTextureStatus_WantCreate);
            update_texture(texture);
            return;
        }

        std::vector<mai::RGBA> pixels = convert_texture_pixels(texture);
        MAI_SGL->bind_texture(texture_id);
        MAI_SGL->tex_image_2D(texture->Width, texture->Height, pixels.data());
        MAI_SGL->bind_texture(0);

        texture->SetStatus(ImTextureStatus_OK);
    }
    else if (texture->Status == ImTextureStatus_WantDestroy && texture->UnusedFrames > 0)
    {
        uint32_t texture_id = static_cast<uint32_t>(texture->GetTexID());
        if (texture_id != 0)
            MAI_SGL->delete_texture(texture_id);

        texture->SetTexID(ImTextureID_Invalid);
        texture->SetStatus(ImTextureStatus_Destroyed);
    }
}

ImTextureID make_texture_id(uint32_t id)
{
    return static_cast<ImTextureID>(static_cast<uintptr_t>(id));
}

uint32_t get_texture_id(ImTextureID id)
{
    return static_cast<uint32_t>(static_cast<uintptr_t>(id));
}

bool create_fonts_texture(ImGuiMaiSglBackendData* backend_data)
{
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels = nullptr;
    int width = 0;
    int height = 0;

    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    uint32_t tex = MAI_SGL->get_texture();
    MAI_SGL->bind_texture(tex);
    MAI_SGL->tex_image_2D(width, height, pixels);
    MAI_SGL->tex_parameter(MAI_TEXTURE_FILTER, MAI_TEXTURE_FILTER_LINEAR);
    MAI_SGL->tex_parameter(MAI_TEXTURE_WRAP_U, MAI_TEXTURE_WRAP_REPEAT);
    MAI_SGL->tex_parameter(MAI_TEXTURE_WRAP_V, MAI_TEXTURE_WRAP_REPEAT);
    MAI_SGL->bind_texture(0);

    io.Fonts->SetTexID(make_texture_id(tex));
    return true;
}


mai::vec4f unpack_color(ImU32 color)
{
    constexpr float inv_255 = 1.0f / 255.0f;
    return mai::vec4f(
        static_cast<float>((color >> IM_COL32_R_SHIFT) & 0xFF) * inv_255,
        static_cast<float>((color >> IM_COL32_G_SHIFT) & 0xFF) * inv_255,
        static_cast<float>((color >> IM_COL32_B_SHIFT) & 0xFF) * inv_255,
        static_cast<float>((color >> IM_COL32_A_SHIFT) & 0xFF) * inv_255);
}

std::vector<MaiImGuiVertex> convert_vertices(const ImDrawList* draw_list, const ImDrawData* draw_data, int framebuffer_height)
{
    const ImVec2 display_pos = draw_data->DisplayPos;
    const ImVec2 framebuffer_scale = draw_data->FramebufferScale;

    std::vector<MaiImGuiVertex> vertices;
    vertices.reserve(draw_list->VtxBuffer.Size);

    for (const ImDrawVert& src : draw_list->VtxBuffer)
    {
        const float x = (src.pos.x - display_pos.x) * framebuffer_scale.x;
        const float y_top = (src.pos.y - display_pos.y) * framebuffer_scale.y;

        MaiImGuiVertex dst;
        dst.pos = mai::vec4f(x, static_cast<float>(framebuffer_height) - y_top, 0.0f, 1.0f);
        dst.color = unpack_color(src.col);
        dst.uv = mai::vec2f(src.uv.x, src.uv.y);
        vertices.push_back(dst);
    }

    return vertices;
}

std::vector<uint32_t> convert_indices(const ImDrawList* draw_list, const ImDrawCmd* draw_cmd)
{
    std::vector<uint32_t> indices;
    indices.reserve(draw_cmd->ElemCount);

    for (unsigned int i = 0; i < draw_cmd->ElemCount; ++i)
    {
        const ImDrawIdx index = draw_list->IdxBuffer[draw_cmd->IdxOffset + i];
        indices.push_back(static_cast<uint32_t>(index) + static_cast<uint32_t>(draw_cmd->VtxOffset));
    }

    return indices;
}

mai::ScissorRect make_scissor_rect(const ImDrawCmd* draw_cmd, const ImDrawData* draw_data, int framebuffer_width, int framebuffer_height)
{
    const ImVec2 display_pos = draw_data->DisplayPos;
    const ImVec2 framebuffer_scale = draw_data->FramebufferScale;

    float clip_min_x = (draw_cmd->ClipRect.x - display_pos.x) * framebuffer_scale.x;
    float clip_min_y = (draw_cmd->ClipRect.y - display_pos.y) * framebuffer_scale.y;
    float clip_max_x = (draw_cmd->ClipRect.z - display_pos.x) * framebuffer_scale.x;
    float clip_max_y = (draw_cmd->ClipRect.w - display_pos.y) * framebuffer_scale.y;

    clip_min_x = std::clamp(clip_min_x, 0.0f, static_cast<float>(framebuffer_width));
    clip_max_x = std::clamp(clip_max_x, 0.0f, static_cast<float>(framebuffer_width));
    clip_min_y = std::clamp(clip_min_y, 0.0f, static_cast<float>(framebuffer_height));
    clip_max_y = std::clamp(clip_max_y, 0.0f, static_cast<float>(framebuffer_height));

    mai::ScissorRect rect;
    rect._min_x = static_cast<int>(clip_min_x);
    rect._max_x = static_cast<int>(clip_max_x);
    rect._min_y = static_cast<int>(static_cast<float>(framebuffer_height) - clip_max_y);
    rect._max_y = static_cast<int>(static_cast<float>(framebuffer_height) - clip_min_y);
    return rect;
}

void setup_render_state(ImGuiMaiSglBackendData* backend_data, const ImDrawData* draw_data, int framebuffer_width, int framebuffer_height)
{
    MAI_SGL->draw_dimension(MAI_DRAW_2D);
    MAI_SGL->disable(MAI_DEPTH_TEST);
    MAI_SGL->disable(MAI_CULL_FACE);
    MAI_SGL->enable(MAI_BLENDING);
    MAI_SGL->enable(MAI_SCISSOR_TEST);

    backend_data->shader->_transform_matrix = mai::orthographic(
        0.0f, static_cast<float>(framebuffer_width),
        0.0f, static_cast<float>(framebuffer_height),
        -1.0f, 1.0f);
    backend_data->shader->_modulate_vertex_color = true;

    (void)draw_data;
}

}

bool ImGui_Impl_MAI_SGL_Init()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.BackendRendererUserData != nullptr)
        return true;

    ImGuiMaiSglBackendData* backend_data = new ImGuiMaiSglBackendData();
    backend_data->shader = new mai::TextureShader2D();

    backend_data->vao = MAI_SGL->gen_vertex_array();
    backend_data->vertex_vbo = MAI_SGL->gen_buffer();
    backend_data->index_ebo = MAI_SGL->gen_buffer();

    MAI_SGL->bind_vertex_array(backend_data->vao);
    MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, backend_data->vertex_vbo);
    MAI_SGL->vertex_attribute_pointer(0, 4, sizeof(MaiImGuiVertex), offsetof(MaiImGuiVertex, pos));
    MAI_SGL->vertex_attribute_pointer(1, 4, sizeof(MaiImGuiVertex), offsetof(MaiImGuiVertex, color));
    MAI_SGL->vertex_attribute_pointer(2, 2, sizeof(MaiImGuiVertex), offsetof(MaiImGuiVertex, uv));
    MAI_SGL->bind_vertex_array(0);
    MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, 0);

    io.BackendRendererUserData = backend_data;
    io.BackendRendererName = "imgui_impl_MAI_SGL";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    create_fonts_texture(backend_data);

    return true;
}

void ImGui_Impl_MAI_SGL_Shutdown()
{
    ImGuiMaiSglBackendData* backend_data = get_backend_data();
    if (backend_data == nullptr)
        return;

    ImGuiIO& io = ImGui::GetIO();
    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasTextures);

    MAI_SGL->delete_buffer(backend_data->vertex_vbo);
    MAI_SGL->delete_buffer(backend_data->index_ebo);
    MAI_SGL->delete_vertex_array(backend_data->vao);

    delete backend_data->shader;
    delete backend_data;
}

void ImGui_Impl_MAI_SGL_NewFrame()
{
    IM_ASSERT(get_backend_data() != nullptr && "Context or backend not initialized! Did you call ImGui_Impl_MAI_SGL_Init()?");
}

void ImGui_Impl_MAI_SGL_RenderDrawData(ImDrawData* draw_data)
{
    const int framebuffer_width = static_cast<int>(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    const int framebuffer_height = static_cast<int>(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (framebuffer_width <= 0 || framebuffer_height <= 0)
        return;

    mai::RenderState old_state = MAI_SGL->get_render_state();

    ImGuiMaiSglBackendData* backend_data = get_backend_data();
    IM_ASSERT(backend_data != nullptr && "Context or backend not initialized! Did you call ImGui_Impl_MAI_SGL_Init()?");

    if (draw_data->Textures != nullptr)
    {
        for (ImTextureData* texture : *draw_data->Textures)
        {
            if (texture->Status != ImTextureStatus_OK)
                update_texture(texture);
        }
    }

    setup_render_state(backend_data, draw_data, framebuffer_width, framebuffer_height);

    MAI_SGL->use_program(backend_data->shader);
    MAI_SGL->bind_vertex_array(backend_data->vao);

    for (const ImDrawList* draw_list : draw_data->CmdLists)
    {
        std::vector<MaiImGuiVertex> vertices = convert_vertices(draw_list, draw_data, framebuffer_height);
        if (vertices.empty())
            continue;

        MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, backend_data->vertex_vbo);
        MAI_SGL->buffer_data(MAI_ARRAY_BUFFER, vertices.size() * sizeof(MaiImGuiVertex), vertices.data());

        for (int cmd_i = 0; cmd_i < draw_list->CmdBuffer.Size; ++cmd_i)
        {
            const ImDrawCmd* draw_cmd = &draw_list->CmdBuffer[cmd_i];
            if (draw_cmd->UserCallback != nullptr)
            {
                if (draw_cmd->UserCallback == ImDrawCallback_ResetRenderState)
                    setup_render_state(backend_data, draw_data, framebuffer_width, framebuffer_height);
                else
                    draw_cmd->UserCallback(draw_list, draw_cmd);
                continue;
            }

            mai::ScissorRect scissor_rect = make_scissor_rect(draw_cmd, draw_data, framebuffer_width, framebuffer_height);
            if (scissor_rect.empty())
                continue;

            std::vector<uint32_t> indices = convert_indices(draw_list, draw_cmd);
            if (indices.empty())
                continue;

            backend_data->shader->_diffuse_texture = static_cast<uint32_t>(draw_cmd->GetTexID());

            MAI_SGL->set_scissor_rect(scissor_rect);
            MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, backend_data->index_ebo);
            MAI_SGL->buffer_data(MAI_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data());
            MAI_SGL->draw_element(MAI_DRAW_TRIANGLES, 0, indices.size());
        }
    }

    MAI_SGL->bind_vertex_array(0);
    MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, 0);
    MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, 0);

    MAI_SGL->set_render_state(old_state);
}
