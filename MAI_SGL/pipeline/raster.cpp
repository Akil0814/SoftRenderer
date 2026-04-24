#include "raster.h"
#include "../math/math.h"
#include "../core/gpu.h"

#include <cmath>

namespace
{

float edge_function(const mai::VertexShaderOutput& a, const mai::VertexShaderOutput& b, float px, float py) noexcept
{
    return (px - a._position.x) * (b._position.y - a._position.y) -
        (py - a._position.y) * (b._position.x - a._position.x);
}

bool is_top_left_edge(const mai::VertexShaderOutput& a, const mai::VertexShaderOutput& b) noexcept
{
    const float dy = b._position.y - a._position.y;
    const float dx = b._position.x - a._position.x;
    return dy < 0.0f || (dy == 0.0f && dx < 0.0f);
}

}

namespace mai
{

    void Raster::rasterize(
        const DrawContext& context, uint8_t draw_mode, const std::vector<VertexShaderOutput>& inputs,
        std::vector<VertexShaderOutput>& results)
    {
        if (draw_mode == MAI_DRAW_LINES)
        {
            for (uint32_t i = 0; i < inputs.size(); i += 2)
                rasterize_line(context, inputs[i], inputs[i + 1], results);
        }

        if (draw_mode == MAI_DRAW_TRIANGLES)
        {
            for (uint32_t i = 0; i < inputs.size(); i += 3)
                rasterize_triangle(context, inputs[i], inputs[i + 1], inputs[i + 2], results);
        }
    }

    void Raster::rasterize_line(
        const DrawContext& context, const VertexShaderOutput& v0, const VertexShaderOutput& v1,
        std::vector<VertexShaderOutput>& results)
    {
        uint64_t rasterized_pixels = 0;
        VsOutput start = v0;
        VsOutput end = v1;

        if (start._position.x > end._position.x)
            std::swap(start, end);

        auto in_scissor_rect = [&context](const VsOutput& point) noexcept
        {
            if (!context._state._enable_scissor_test)
                return true;

            const auto& rect = context._state._scissor_clip_rect;
            return rect.contains(
                static_cast<int>(point._position.x),
                static_cast<int>(point._position.y));
        };

        if (in_scissor_rect(start))
        {
            results.push_back(start);
            ++rasterized_pixels;
        }

        bool flip_y = false;
        if (start._position.y > end._position.y)
        {
            start._position.y = -start._position.y;
            end._position.y = -end._position.y;
            flip_y = true;
        }

        int delta_x = static_cast<int>(end._position.x - start._position.x);
        int delta_y = static_cast<int>(end._position.y - start._position.y);

        bool swap_xy = false;
        if (delta_x < delta_y)
        {
            std::swap(start._position.x, start._position.y);
            std::swap(end._position.x, end._position.y);
            std::swap(delta_x, delta_y);
            swap_xy = true;
        }

        int current_x = static_cast<int>(start._position.x);
        int current_y = static_cast<int>(start._position.y);

        int result_x = 0;
        int result_y = 0;

        VsOutput current_point;
        int p = 2 * delta_y - delta_x;

        for (int i = 0; i < delta_x; ++i)
        {
            if (p >= 0)
            {
                current_y += 1;
                p -= 2 * delta_x;
            }

            current_x += 1;
            p += 2 * delta_y;

            result_x = current_x;
            result_y = current_y;
            if (swap_xy)
                std::swap(result_x, result_y);

            if (flip_y)
                result_y = -result_y;

            current_point._position.x = result_x;
            current_point._position.y = result_y;

            interpolant_line(v0, v1, current_point);

            if (in_scissor_rect(current_point))
            {
                results.push_back(current_point);
                ++rasterized_pixels;
            }
        }

        MAI_SGL->add_rasterized_pixels(rasterized_pixels);
    }

    void Raster::rasterize_triangle(
        const DrawContext& context, const VertexShaderOutput& v0, const VertexShaderOutput& v1, const VertexShaderOutput& v2,
        std::vector<VertexShaderOutput>& results)
    {
        constexpr float edge_epsilon = 1e-6f;
        uint64_t rasterized_pixels = 0;

        int max_X = static_cast<int>(std::max(v0._position.x, std::max(v1._position.x, v2._position.x)));
        int min_X = static_cast<int>(std::min(v0._position.x, std::min(v1._position.x, v2._position.x)));
        int max_Y = static_cast<int>(std::max(v0._position.y, std::max(v1._position.y, v2._position.y)));
        int min_Y = static_cast<int>(std::min(v0._position.y, std::min(v1._position.y, v2._position.y)));

        if (context._state._enable_scissor_test)
        {
            const auto& rect = context._state._scissor_clip_rect;
            min_X = std::max(min_X, rect._min_x);
            min_Y = std::max(min_Y, rect._min_y);
            max_X = std::min(max_X, rect._max_x - 1);
            max_Y = std::min(max_Y, rect._max_y - 1);
        }

        if (min_X > max_X || min_Y > max_Y)
            return;

        float area = edge_function(v0, v1, v2._position.x, v2._position.y);
        if (std::abs(area) <= edge_epsilon)
            return;

        const bool flip_winding = area < 0.0f;
        if (flip_winding)
            area = -area;

        VsOutput result;

        for (int i = min_X; i <= max_X; ++i)
        {
            for (int j = min_Y; j <= max_Y; ++j)
            {
                const float px = i + 0.5f;
                const float py = j + 0.5f;

                float w0 = edge_function(v1, v2, px, py);
                float w1 = edge_function(v2, v0, px, py);
                float w2 = edge_function(v0, v1, px, py);

                if (flip_winding)
                {
                    w0 = -w0;
                    w1 = -w1;
                    w2 = -w2;
                }

                const bool inside0 = (w0 > edge_epsilon) || (std::abs(w0) <= edge_epsilon && is_top_left_edge(v1, v2));
                const bool inside1 = (w1 > edge_epsilon) || (std::abs(w1) <= edge_epsilon && is_top_left_edge(v2, v0));
                const bool inside2 = (w2 > edge_epsilon) || (std::abs(w2) <= edge_epsilon && is_top_left_edge(v0, v1));

                if (!inside0 || !inside1 || !inside2)
                    continue;

                interpolant_triangle(v0, v1, v2, px, py, result);
                result._position.x = i;
                result._position.y = j;
                results.push_back(result);
                ++rasterized_pixels;
            }
        }

        MAI_SGL->add_rasterized_pixels(rasterized_pixels);
    }

    void Raster::interpolant_line(const VertexShaderOutput& v0, const VertexShaderOutput& v1,
        VertexShaderOutput& target) noexcept
    {
        if (v0._position.x == v1._position.x && v0._position.y == v1._position.y)
        {
            target._color = v0._color;
            return;
        }

        float weight = 1.0f;
        if (v1._position.x != v0._position.x)
            weight = static_cast<float>(target._position.x - v0._position.x) / static_cast<float>(v1._position.x - v0._position.x);
        else if (v1._position.y != v0._position.y)
            weight = static_cast<float>(target._position.y - v0._position.y) / static_cast<float>(v1._position.y - v0._position.y);

        target._color = mai::lerp(v0._color, v1._color, weight);
        target._UV = mai::lerp(v0._UV, v1._UV, weight);
    }

    void Raster::interpolant_triangle(const VertexShaderOutput& v0, const VertexShaderOutput& v1, const VertexShaderOutput& v2,
        float sample_x, float sample_y, VertexShaderOutput& target) noexcept
    {
        constexpr float edge_epsilon = 1e-6f;

        float area = edge_function(v0, v1, v2._position.x, v2._position.y);
        if (std::abs(area) <= edge_epsilon)
            return;

        float weight0 = edge_function(v1, v2, sample_x, sample_y) / area;
        float weight1 = edge_function(v2, v0, sample_x, sample_y) / area;
        float weight2 = edge_function(v0, v1, sample_x, sample_y) / area;

        target._inv_w = mai::lerp(v0._inv_w, v1._inv_w, v2._inv_w, weight0, weight1, weight2);
        target._position.z = mai::lerp(v0._position.z, v1._position.z, v2._position.z, weight0, weight1, weight2);
        target._color = mai::lerp(v0._color, v1._color, v2._color, weight0, weight1, weight2);
        target._UV = mai::lerp(v0._UV, v1._UV, v2._UV, weight0, weight1, weight2);
    }

}
