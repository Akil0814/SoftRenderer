#include "gpu.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "gpu_debug.h"

namespace
{

std::string make_timestamp()
{
	const auto now = std::chrono::system_clock::now();
	const std::time_t now_time = std::chrono::system_clock::to_time_t(now);

	std::tm local_time{};
	localtime_s(&local_time, &now_time);

	std::ostringstream stream;
	stream << std::put_time(&local_time, "%Y%m%d_%H%M%S");
	return stream.str();
}

double frame_count_inverse(const mai::RenderStats& stats)
{
	const double frame_count = static_cast<double>(stats._frame_count);
	return frame_count > 0.0 ? 1.0 / frame_count : 0.0;
}

void write_summary_text(std::ostream& out, const mai::RenderStats& stats)
{
	const double inv_frame_count = frame_count_inverse(stats);

	out << "-----------------------------summary-----------------------------\n";
	out
		<< "Total Frames: " << stats._frame_count << '\n'
		<< "Total Draw Calls: " << stats._frame_draw_calls << '\n'
		<< "Total Triangles: " << stats._frame_triangles << '\n'
		<< "Total Vertices: " << stats._frame_vertices << '\n'
		<< "Total Rasterized Pixels: " << stats._frame_rasterized_pixels << '\n'
		<< "Total Fragments: " << stats._frame_fragments << '\n'
		<< "Total Texture Samples: " << stats._frame_texture_samples << '\n';
	out
		<< "Total Triangle Flow:\n"
		<< " Input: " << stats._frame_input_triangles
		<< " | Clipped: " << stats._frame_clipped_triangles
		<< " | Clip Discarded: " << stats._frame_clip_discarded_triangles
		<< " | Clip Output: " << stats._frame_clip_output_triangles
		<< " | Culled: " << stats._frame_culled_triangles
		<< " | Raster Input: " << stats._frame_raster_input_triangles
		<< " | Degenerate: " << stats._frame_degenerate_triangles
		<< '\n';

	out
		<< std::fixed << std::setprecision(3)
		<< "Total Timing(ms):\n"
		<< " Vertex: " << stats._vertex_ms
		<< " | Primitive: " << stats._primitive_ms
		<< " | Clip: " << stats._clip_ms
		<< " | PerspDiv: " << stats._perspective_division_ms
		<< " | Cull: " << stats._cull_ms
		<< " | Screen: " << stats._screen_mapping_ms
		<< " | Raster: " << stats._raster_ms
		<< " | Recover: " << stats._perspective_recover_ms
		<< " | Fragment: " << stats._fragment_ms
		<< '\n';
	out
		<< "Avg Timing(ms/frame):\n"
		<< " Vertex: " << stats._vertex_ms * inv_frame_count
		<< " | Primitive: " << stats._primitive_ms * inv_frame_count
		<< " | Clip: " << stats._clip_ms * inv_frame_count
		<< " | PerspDiv: " << stats._perspective_division_ms * inv_frame_count
		<< " | Cull: " << stats._cull_ms * inv_frame_count
		<< " | Screen: " << stats._screen_mapping_ms * inv_frame_count
		<< " | Raster: " << stats._raster_ms * inv_frame_count
		<< " | Recover: " << stats._perspective_recover_ms * inv_frame_count
		<< " | Fragment: " << stats._fragment_ms * inv_frame_count
		<< '\n';
}

}

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

bool GPU::set_stats_output_path(const std::string& output_path_prefix)
{
	_frame_stats_file.close();
	_summary_stats_file.close();
	_stats_output_frame_index = 0;

	const std::filesystem::path prefix_path(output_path_prefix);
	if (prefix_path.empty())
	{
		set_error(gpu_debug::ErrorCode::InvalidValue);
		return false;
	}

	try
	{
		const std::filesystem::path parent_path = prefix_path.parent_path();
		if (!parent_path.empty())
			std::filesystem::create_directories(parent_path);

		const std::string timestamp = make_timestamp();
		const std::filesystem::path frame_path =
			prefix_path.parent_path() / (prefix_path.filename().string() + "_frames_" + timestamp + ".csv");
		const std::filesystem::path summary_path =
			prefix_path.parent_path() / (prefix_path.filename().string() + "_summary_" + timestamp + ".txt");

		_frame_stats_file.open(frame_path, std::ios::out | std::ios::trunc);
		_summary_stats_file.open(summary_path, std::ios::out | std::ios::trunc);
	}
	catch (...)
	{
		_frame_stats_file.close();
		_summary_stats_file.close();
		set_error(gpu_debug::ErrorCode::InvalidOperation);
		return false;
	}

	if (!_frame_stats_file.is_open() || !_summary_stats_file.is_open())
	{
		_frame_stats_file.close();
		_summary_stats_file.close();
		set_error(gpu_debug::ErrorCode::InvalidOperation);
		return false;
	}

	_frame_stats_file
		<< "frame,draw_calls,triangles,vertices,rasterized_pixels,fragments,texture_samples,"
		<< "input_triangles,clipped_triangles,clip_discarded_triangles,clip_output_triangles,"
		<< "culled_triangles,raster_input_triangles,degenerate_triangles,"
		<< "vertex_ms,primitive_ms,clip_ms,perspective_division_ms,cull_ms,"
		<< "screen_mapping_ms,raster_ms,perspective_recover_ms,fragment_ms\n";
	return true;
}

void GPU::print_frame_stats()
{
	std::cout
		<< "Drawing Data:\n"
		<< "Draw Calls:" << _render_stats._frame_draw_calls
		<< " | Triangles: " << _render_stats._frame_triangles
		<< " | Vertices: " << _render_stats._frame_vertices
		<< " | Rasterized Pixels: " << _render_stats._frame_rasterized_pixels
		<< " | Fragments: " << _render_stats._frame_fragments
		<< " | Texture Samples: " << _render_stats._frame_texture_samples
		<< '\n';
	std::cout
		<< "Triangle Flow:\n"
		<< " Input: " << _render_stats._frame_input_triangles
		<< " | Clipped: " << _render_stats._frame_clipped_triangles
		<< " | Clip Discarded: " << _render_stats._frame_clip_discarded_triangles
		<< " | Clip Output: " << _render_stats._frame_clip_output_triangles
		<< " | Culled: " << _render_stats._frame_culled_triangles
		<< " | Raster Input: " << _render_stats._frame_raster_input_triangles
		<< " | Degenerate: " << _render_stats._frame_degenerate_triangles
		<< '\n';

	std::cout
		<< std::fixed << std::setprecision(3)
		<< "Timing(ms):\n"
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

	write_frame_stats_csv();
}

void GPU::print_summary_stats()
{
	write_summary_text(std::cout, _summary_render_stats);
	write_summary_stats_txt();
}

void GPU::write_frame_stats_csv()
{
	if (!_frame_stats_file.is_open())
		return;

	_frame_stats_file
		<< _stats_output_frame_index++ << ','
		<< _render_stats._frame_draw_calls << ','
		<< _render_stats._frame_triangles << ','
		<< _render_stats._frame_vertices << ','
		<< _render_stats._frame_rasterized_pixels << ','
		<< _render_stats._frame_fragments << ','
		<< _render_stats._frame_texture_samples << ','
		<< _render_stats._frame_input_triangles << ','
		<< _render_stats._frame_clipped_triangles << ','
		<< _render_stats._frame_clip_discarded_triangles << ','
		<< _render_stats._frame_clip_output_triangles << ','
		<< _render_stats._frame_culled_triangles << ','
		<< _render_stats._frame_raster_input_triangles << ','
		<< _render_stats._frame_degenerate_triangles << ','
		<< std::fixed << std::setprecision(6)
		<< _render_stats._vertex_ms << ','
		<< _render_stats._primitive_ms << ','
		<< _render_stats._clip_ms << ','
		<< _render_stats._perspective_division_ms << ','
		<< _render_stats._cull_ms << ','
		<< _render_stats._screen_mapping_ms << ','
		<< _render_stats._raster_ms << ','
		<< _render_stats._perspective_recover_ms << ','
		<< _render_stats._fragment_ms
		<< '\n';
}

void GPU::write_summary_stats_txt()
{
	if (!_summary_stats_file.is_open())
		return;

	_summary_stats_file.seekp(0);
	write_summary_text(_summary_stats_file, _summary_render_stats);
}

}
