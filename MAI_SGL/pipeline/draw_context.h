#pragma once

#include <map>

#include "../core/render_state.h"
#include "../resource/buffer_object.h"
#include "../resource/frame_buffer.h"
#include "../resource/texture.h"
#include "../resource/VAO.h"
#include "../shader/shader.h"

namespace mai
{

struct DrawContext
{
	FrameBuffer& _frame_buffer;
	Shader& _shader;
	const RenderState& _state;
	const std::map<uint32_t, BufferObject*>& _buffer_map;
	const std::map<uint32_t, Texture*>& _texture_map;
	const VertexArrayObject& _vao;
	const BufferObject& _ebo;
	const mat4f& _screen_matrix;
};

}
