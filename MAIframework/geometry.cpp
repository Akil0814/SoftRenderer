#include "geometry.h"

#include <cmath>
#include <utility>

#include "../MAI_SGL/core/gpu.h"

namespace mai
{
namespace
{

	std::vector<float> make_default_colors(size_t vertex_count)
	{
		std::vector<float> colors(vertex_count * 4, 1.0f);
		return colors;
	}

	std::vector<float> make_default_uvs(size_t vertex_count)
	{
		std::vector<float> uvs(vertex_count * 2, 0.0f);
		return uvs;
	}

}

Geometry::Geometry(
	const std::vector<float>& positions,
	const std::vector<float>& uvs,
	const std::vector<uint32_t>& indices,
	const std::vector<float>& colors,
	const std::vector<float>& normals
)
{
	initialize(positions, uvs, indices, colors, normals);
}

Geometry::~Geometry()
{
	release();
}

Geometry::Geometry(Geometry&& other) noexcept
{
	*this = std::move(other);
}

Geometry& Geometry::operator=(Geometry&& other) noexcept
{
	if (this == &other)
		return *this;

	release();

	_vao = other._vao;
	_position_vbo = other._position_vbo;
	_color_vbo = other._color_vbo;
	_uv_vbo = other._uv_vbo;
	_normal_vbo = other._normal_vbo;
	_ebo = other._ebo;
	_index_count = other._index_count;

	other._vao = 0;
	other._position_vbo = 0;
	other._color_vbo = 0;
	other._uv_vbo = 0;
	other._normal_vbo = 0;
	other._ebo = 0;
	other._index_count = 0;

	return *this;
}

Geometry* Geometry::create_box(float size)
{
	const float half_size = size * 0.5f;

	const std::vector<float> positions{
		-half_size, -half_size,  half_size,  half_size, -half_size,  half_size,  half_size,  half_size,  half_size, -half_size,  half_size,  half_size,
		 half_size, -half_size, -half_size, -half_size, -half_size, -half_size, -half_size,  half_size, -half_size,  half_size,  half_size, -half_size,
		-half_size, -half_size, -half_size, -half_size, -half_size,  half_size, -half_size,  half_size,  half_size, -half_size,  half_size, -half_size,
		 half_size, -half_size,  half_size,  half_size, -half_size, -half_size,  half_size,  half_size, -half_size,  half_size,  half_size,  half_size,
		-half_size,  half_size,  half_size,  half_size,  half_size,  half_size,  half_size,  half_size, -half_size, -half_size,  half_size, -half_size,
		-half_size, -half_size, -half_size,  half_size, -half_size, -half_size,  half_size, -half_size,  half_size, -half_size, -half_size,  half_size
	};

	const std::vector<float> uvs{
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	const std::vector<float> normals{
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f,-1.0f, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f,-1.0f,
		-1.0f,0.0f,0.0f, -1.0f,0.0f,0.0f, -1.0f,0.0f,0.0f, -1.0f,0.0f,0.0f,
		 1.0f,0.0f,0.0f,  1.0f,0.0f,0.0f,  1.0f,0.0f,0.0f,  1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f, 0.0f,1.0f,0.0f, 0.0f,1.0f,0.0f, 0.0f,1.0f,0.0f,
		0.0f,-1.0f,0.0f, 0.0f,-1.0f,0.0f, 0.0f,-1.0f,0.0f, 0.0f,-1.0f,0.0f
	};

	const std::vector<uint32_t> indices{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
	};

	return new Geometry(positions, uvs, indices, {}, normals);
}

Geometry* Geometry::create_sphere(float radius, uint32_t latitude_segments, uint32_t longitude_segments)
{
	assert(latitude_segments > 0);
	assert(longitude_segments > 0);

	std::vector<float> positions;
	std::vector<float> uvs;
	std::vector<float> normals;
	std::vector<uint32_t> indices;

	const size_t vertex_count = static_cast<size_t>(latitude_segments + 1) * static_cast<size_t>(longitude_segments + 1);
	positions.reserve(vertex_count * 3);
	uvs.reserve(vertex_count * 2);
	normals.reserve(vertex_count * 3);
	indices.reserve(static_cast<size_t>(latitude_segments) * static_cast<size_t>(longitude_segments) * 6);

	const float inv_radius = radius != 0.0f ? 1.0f / radius : 0.0f;

	for (uint32_t lat = 0; lat <= latitude_segments; ++lat)
	{
		const float phi = static_cast<float>(lat) * MAI_PI / static_cast<float>(latitude_segments);

		for (uint32_t lon = 0; lon <= longitude_segments; ++lon)
		{
			const float theta = static_cast<float>(lon) * 2.0f * MAI_PI / static_cast<float>(longitude_segments);

			const float y = radius * std::cos(phi);
			const float x = radius * std::sin(phi) * std::cos(theta);
			const float z = radius * std::sin(phi) * std::sin(theta);

			positions.push_back(x);
			positions.push_back(y);
			positions.push_back(z);

			uvs.push_back(1.0f - static_cast<float>(lon) / static_cast<float>(longitude_segments));
			uvs.push_back(1.0f - static_cast<float>(lat) / static_cast<float>(latitude_segments));

			normals.push_back(x * inv_radius);
			normals.push_back(y * inv_radius);
			normals.push_back(z * inv_radius);
		}
	}

	for (uint32_t lat = 0; lat < latitude_segments; ++lat)
	{
		for (uint32_t lon = 0; lon < longitude_segments; ++lon)
		{
			const uint32_t p1 = lat * (longitude_segments + 1) + lon;
			const uint32_t p2 = p1 + longitude_segments + 1;
			const uint32_t p3 = p1 + 1;
			const uint32_t p4 = p2 + 1;

			indices.push_back(p1);
			indices.push_back(p2);
			indices.push_back(p3);

			indices.push_back(p3);
			indices.push_back(p2);
			indices.push_back(p4);
		}
	}

	return new Geometry(positions, uvs, indices, {}, normals);
}

Geometry* Geometry::create_square(float size)
{
	const float half_size = size * 0.5f;

	const std::vector<float> positions{
		 half_size,  half_size, 0.0f,
		 half_size, -half_size, 0.0f,
		-half_size,  half_size, 0.0f,
		-half_size, -half_size, 0.0f
	};

	const std::vector<float> uvs{
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
	};

	const std::vector<float> normals{
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};

	const std::vector<uint32_t> indices{
		0, 1, 2,
		1, 3, 2
	};

	return new Geometry(positions, uvs, indices, {}, normals);
}

Geometry* Geometry::create_triangular_pyramid(float size)
{
	const float h = std::sqrt(2.0f / 3.0f) * size;
	const float r = size / std::sqrt(3.0f);

	const std::vector<float> positions{
		 0.0f,         0.0f,  r,
		-size * 0.5f,  0.0f, -r * 0.5f,
		 size * 0.5f,  0.0f, -r * 0.5f,

		 0.0f,         0.0f,  r,
		-size * 0.5f,  0.0f, -r * 0.5f,
		 0.0f,         h,     0.0f,

		-size * 0.5f,  0.0f, -r * 0.5f,
		 size * 0.5f,  0.0f, -r * 0.5f,
		 0.0f,         h,     0.0f,

		 size * 0.5f,  0.0f, -r * 0.5f,
		 0.0f,         0.0f,  r,
		 0.0f,         h,     0.0f
	};

	const std::vector<float> uvs{
		0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};

	const std::vector<float> normals{
		0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.4472136f, 0.8944272f, 0.0f, 0.4472136f, 0.8944272f, 0.0f, 0.4472136f, 0.8944272f,
		-0.7745967f, 0.4472136f, -0.4472136f, -0.7745967f, 0.4472136f, -0.4472136f, -0.7745967f, 0.4472136f, -0.4472136f,
		0.7745967f, 0.4472136f, -0.4472136f, 0.7745967f, 0.4472136f, -0.4472136f, 0.7745967f, 0.4472136f, -0.4472136f
	};

	const std::vector<uint32_t> indices{
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11
	};

	return new Geometry(positions, uvs, indices, {}, normals);
}

void Geometry::initialize(
	const std::vector<float>& positions,
	const std::vector<float>& uvs,
	const std::vector<uint32_t>& indices,
	const std::vector<float>& colors,
	const std::vector<float>& normals
)
{
	release();

	if (positions.empty() || indices.empty())
		return;

	assert(positions.size() % 3 == 0);

	const size_t vertex_count = positions.size() / 3;
	const std::vector<float> resolved_colors = colors.empty() ? make_default_colors(vertex_count) : colors;
	const std::vector<float> resolved_uvs = uvs.empty() ? make_default_uvs(vertex_count) : uvs;

	assert(resolved_colors.size() == vertex_count * 4);
	assert(resolved_uvs.size() == vertex_count * 2);
	assert(normals.empty() || normals.size() == vertex_count * 3);

	_vao = MAI_SGL->gen_vertex_array();
	MAI_SGL->bind_vertex_array(_vao);

	_position_vbo = create_attribute_buffer(0, 3, positions);
	_color_vbo = create_attribute_buffer(1, 4, resolved_colors);
	_uv_vbo = create_attribute_buffer(2, 2, resolved_uvs);

	// Binding 3 is reserved for normals. Current shaders do not consume it yet.
	if (!normals.empty())
		_normal_vbo = create_attribute_buffer(3, 3, normals);

	_ebo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, _ebo);
	MAI_SGL->buffer_data(
		MAI_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(uint32_t),
		const_cast<uint32_t*>(indices.data())
	);

	_index_count = static_cast<uint32_t>(indices.size());

	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, 0);
	MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, 0);
	MAI_SGL->bind_vertex_array(0);
}

void Geometry::release() noexcept
{
	if (_vao != 0)
		MAI_SGL->delete_vertex_array(_vao);

	if (_position_vbo != 0)
		MAI_SGL->delete_buffer(_position_vbo);

	if (_color_vbo != 0)
		MAI_SGL->delete_buffer(_color_vbo);

	if (_uv_vbo != 0)
		MAI_SGL->delete_buffer(_uv_vbo);

	if (_normal_vbo != 0)
		MAI_SGL->delete_buffer(_normal_vbo);

	if (_ebo != 0)
		MAI_SGL->delete_buffer(_ebo);

	_vao = 0;
	_position_vbo = 0;
	_color_vbo = 0;
	_uv_vbo = 0;
	_normal_vbo = 0;
	_ebo = 0;
	_index_count = 0;
}

uint32_t Geometry::create_attribute_buffer(uint32_t binding, uint32_t item_size, const std::vector<float>& data)
{
	if (data.empty())
		return 0;

	const uint32_t buffer = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, buffer);
	MAI_SGL->buffer_data(
		MAI_ARRAY_BUFFER,
		data.size() * sizeof(float),
		const_cast<float*>(data.data())
	);
	MAI_SGL->vertex_attribute_pointer(binding, item_size, item_size * sizeof(float), 0);

	return buffer;
}

}
