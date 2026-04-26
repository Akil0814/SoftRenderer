#include "gpu.h"

#include <iomanip>
#include <iostream>

#include "gpu_debug.h"

namespace mai
{
namespace gpu_debug
{

const char* to_string(ErrorCode error) noexcept
{
	switch (error)
	{
	case ErrorCode::NoError:
		return "NO_ERROR";
	case ErrorCode::InvalidEnum:
		return "INVALID_ENUM";
	case ErrorCode::InvalidValue:
		return "INVALID_VALUE";
	case ErrorCode::InvalidOperation:
		return "INVALID_OPERATION";
	case ErrorCode::OutOfMemory:
		return "OUT_OF_MEMORY";
	default:
		return "UNKNOWN_ERROR";
	}
}

const char* describe_error(ErrorCode error) noexcept
{
	switch (error)
	{
	case ErrorCode::NoError:
		return "No GPU debug error has been recorded.";
	case ErrorCode::InvalidEnum:
		return "An enum-like parameter used an unsupported value.";
	case ErrorCode::InvalidValue:
		return "A numeric value or object id was outside the accepted range.";
	case ErrorCode::InvalidOperation:
		return "The current GPU state does not allow this operation.";
	case ErrorCode::OutOfMemory:
		return "A resource allocation failed.";
	default:
		return "The GPU reported an unknown debug error.";
	}
}

void print_error(ErrorCode error)
{
	if (error == ErrorCode::NoError)
	{
		std::cout << to_string(error) << ": " << describe_error(error) << '\n';
		return;
	}

	std::cerr << to_string(error) << ": " << describe_error(error) << '\n';
}

void print_vao(VertexArrayObject& vao)
{
	vao.print();
}

}

gpu_debug::ErrorCode GPU::get_error() noexcept
{
	const gpu_debug::ErrorCode error = _error_code;
	_error_code = gpu_debug::ErrorCode::NoError;
	return error;
}

gpu_debug::ErrorCode GPU::peek_error() const noexcept
{
	return _error_code;
}

void GPU::set_error(gpu_debug::ErrorCode error) noexcept
{
	if (_error_code == gpu_debug::ErrorCode::NoError)
		_error_code = error;
}

void GPU::print_VAO(uint32_t VAO_ID)
{
	auto iter = _VAO_map.find(VAO_ID);
	if (iter == _VAO_map.end())
	{
		set_error(gpu_debug::ErrorCode::InvalidValue);
		gpu_debug::print_error(peek_error());
		return;
	}

	gpu_debug::print_vao(*iter->second);
}

void GPU::print_frame_stats()
{
	std::cout
		<< "Draw Calls: " << _render_stats._frame_draw_calls
		<< " | Triangles: " << _render_stats._frame_triangles
		<< " | Vertices: " << _render_stats._frame_vertices
		<< " | Rasterized Pixels: " << _render_stats._frame_rasterized_pixels
		<< " | Fragments: " << _render_stats._frame_fragments
		<< " | Texture Samples: " << _render_stats._frame_texture_samples
		<< '\n';

	std::cout
		<< std::fixed << std::setprecision(3)
		<< "Timing(ms)"
		<< " | Vertex: " << _render_stats._vertex_ms
		<< " | Primitive: " << _render_stats._primitive_ms
		<< " | Clip: " << _render_stats._clip_ms
		<< " | PerspDiv: " << _render_stats._perspective_division_ms
		<< " | Cull: " << _render_stats._cull_ms
		<< " | Screen: " << _render_stats._screen_mapping_ms
		<< " | Raster: " << _render_stats._raster_ms
		<< " | Recover: " << _render_stats._perspective_recover_ms
		<< " | Fragment: " << _render_stats._fragment_ms
		<< '\n';
}

void GPU::print_summary_stats()
{
	const double frame_count = static_cast<double>(_summary_render_stats._frame_count);
	const double inv_frame_count = frame_count > 0.0 ? 1.0 / frame_count : 0.0;

	std::cout << "-----------------------------summary-----------------------------\n";
	std::cout<< "Total Frames: " << _summary_render_stats._frame_count << '\n';
	std::cout<< "Total Draw Calls: " << _summary_render_stats._frame_draw_calls << '\n';
	std::cout<< "Total Triangles: " << _summary_render_stats._frame_triangles << '\n';
	std::cout<< "Total Vertices: " << _summary_render_stats._frame_vertices << '\n';
	std::cout<< "Total Rasterized Pixels: " << _summary_render_stats._frame_rasterized_pixels << '\n';
	std::cout<< "Total Fragments: " << _summary_render_stats._frame_fragments << '\n';
	std::cout<< "Total Texture Samples: " << _summary_render_stats._frame_texture_samples << '\n';
	std::cout
		<< std::fixed << std::setprecision(3)
		<< "Total Timing(ms)"
		<< " | Vertex: " << _summary_render_stats._vertex_ms
		<< " | Primitive: " << _summary_render_stats._primitive_ms
		<< " | Clip: " << _summary_render_stats._clip_ms
		<< " | PerspDiv: " << _summary_render_stats._perspective_division_ms
		<< " | Cull: " << _summary_render_stats._cull_ms
		<< " | Screen: " << _summary_render_stats._screen_mapping_ms
		<< " | Raster: " << _summary_render_stats._raster_ms
		<< " | Recover: " << _summary_render_stats._perspective_recover_ms
		<< " | Fragment: " << _summary_render_stats._fragment_ms
		<< '\n';
	std::cout
		<< "Avg Timing(ms/frame)"
		<< " | Vertex: " << _summary_render_stats._vertex_ms * inv_frame_count
		<< " | Primitive: " << _summary_render_stats._primitive_ms * inv_frame_count
		<< " | Clip: " << _summary_render_stats._clip_ms * inv_frame_count
		<< " | PerspDiv: " << _summary_render_stats._perspective_division_ms * inv_frame_count
		<< " | Cull: " << _summary_render_stats._cull_ms * inv_frame_count
		<< " | Screen: " << _summary_render_stats._screen_mapping_ms * inv_frame_count
		<< " | Raster: " << _summary_render_stats._raster_ms * inv_frame_count
		<< " | Recover: " << _summary_render_stats._perspective_recover_ms * inv_frame_count
		<< " | Fragment: " << _summary_render_stats._fragment_ms * inv_frame_count
		<< '\n';
}

}
