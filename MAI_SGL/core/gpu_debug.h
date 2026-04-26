#pragma once

#include <cstdint>

namespace mai
{

class VertexArrayObject;
struct RenderStats;

namespace gpu_debug
{

enum class ErrorCode : std::uint32_t
{
	NoError = 0,
	InvalidEnum = 0x0500,
	InvalidValue = 0x0501,
	InvalidOperation = 0x0502,
	OutOfMemory = 0x0505
};

const char* to_string(ErrorCode error) noexcept;
const char* describe_error(ErrorCode error) noexcept;
void print_error(ErrorCode error);
void print_vao(VertexArrayObject& vao);

}

}
