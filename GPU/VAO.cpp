#include "VAO.h"

namespace mai
{

VertexArrayObject::VertexArrayObject() {}

VertexArrayObject::~VertexArrayObject() {}

void VertexArrayObject::set(uint32_t binding, uint32_t VBO_ID, size_t item_size, size_t stride, size_t offset)
{
	auto iter = _binding_map.find(binding);

	if (iter == _binding_map.end())
		iter = _binding_map.insert(std::make_pair(binding, BindingDescription())).first;

	BindingDescription& des = iter->second;
	des._VBO_id = VBO_ID;
	des._item_size = item_size;
	des._stride = stride;
	des._offset = offset;
}

std::map<uint32_t, BindingDescription> VertexArrayObject::get_binding_map() const
{
	return _binding_map;
}

}
