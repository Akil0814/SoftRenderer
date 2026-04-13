#pragma once
#include "../global/base.h"
#include "data_structures.h"

namespace mai {


class VertexArrayObject
{
public:

	VertexArrayObject();
	~VertexArrayObject();

	void set(uint32_t binding, uint32_t vboId, size_t itemSize, size_t stride, size_t offset);

	std::map<uint32_t, BindingDescription> get_binding_map() const;

private:
	//key:bindingId - value:bindingDescription
	std::map<uint32_t, BindingDescription> _binding_map;

};

}
