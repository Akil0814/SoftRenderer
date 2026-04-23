#pragma once

#include "../base.h"

namespace mai
{

struct RenderState
{
	uint8_t _front_face{ MAI_FRONT_FACE_CCW };
	uint8_t _cull_face{ MAI_BACK_FACE };
	uint8_t _depth_function{ MAI_DEPTH_LESS };
	uint8_t _draw_dimension{ MAI_DRAW_3D };

	bool _enable_cull_face{ true };
	bool _enable_blending{ true };
	bool _enable_depth_test{ true };

	void reset() noexcept { *this = RenderState{}; }
};

}
