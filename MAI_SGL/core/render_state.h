#pragma once

#include "../base.h"

namespace mai
{

struct ScissorRect
{
    int _min_x = 0;
    int _min_y = 0;
    int _max_x = 0;
    int _max_y = 0;

	bool empty() const noexcept
    	{ return _min_x >= _max_x || _min_y >= _max_y; }

    int width() const noexcept
    	{ return _max_x - _min_x; }

    int height() const noexcept
    	{ return _max_y - _min_y; }


	bool contains(int x, int y) const noexcept
    {
        return x >= _min_x && x < _max_x &&
               y >= _min_y && y < _max_y;
    }
};

struct RenderState
{
	ScissorRect _scissor_clip_rect{};

	uint8_t _front_face{ MAI_FRONT_FACE_CCW };
	uint8_t _cull_face{ MAI_BACK_FACE };
	uint8_t _depth_function{ MAI_DEPTH_LESS };
	uint8_t _draw_dimension{ MAI_DRAW_3D };

	bool _enable_cull_face{ false };
	bool _enable_blending{ true };
	bool _enable_depth_test{ true };
	bool _enable_scissor_test{ false };

	void reset() noexcept { *this = RenderState{}; }
};

}
