#pragma once

#include <vector>

#include "../MAI_SGL/base.h"

namespace mai
{

class Geometry
{
public:
	Geometry() = default;
	Geometry(
		const std::vector<float>& positions,
		const std::vector<float>& uvs,
		const std::vector<uint32_t>& indices,
		const std::vector<float>& colors = {},
		const std::vector<float>& normals = {}
	);
	~Geometry();

	Geometry(const Geometry&) = delete;
	Geometry& operator=(const Geometry&) = delete;
	Geometry(Geometry&& other) noexcept;
	Geometry& operator=(Geometry&& other) noexcept;

	static Geometry* create_box(float size);
	static Geometry* create_sphere(
		float radius,
		uint32_t latitude_segments = 32,
		uint32_t longitude_segments = 32
	);
	static Geometry* create_square(float size);
	static Geometry* create_triangular_pyramid(float size);

	uint32_t get_VAO() const { return _vao; }
	uint32_t get_EBO() const { return _ebo; }
	uint32_t get_indices_count() const { return _index_count; }
	bool valid() const { return _vao != 0 && _ebo != 0 && _index_count != 0; }

private:
	void initialize(
		const std::vector<float>& positions,
		const std::vector<float>& uvs,
		const std::vector<uint32_t>& indices,
		const std::vector<float>& colors,
		const std::vector<float>& normals
	);
	void release() noexcept;
	uint32_t create_attribute_buffer(uint32_t binding, uint32_t item_size, const std::vector<float>& data);

	uint32_t _vao = 0;

	uint32_t _position_vbo = 0;
	uint32_t _color_vbo = 0;
	uint32_t _uv_vbo = 0;
	uint32_t _normal_vbo = 0;

	uint32_t _ebo = 0;
	uint32_t _index_count = 0;
};

}
