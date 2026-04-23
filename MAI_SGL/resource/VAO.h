#pragma once
#include "../base.h"
#include "../data_structures.h"
#include <map>

namespace mai {


class VertexArrayObject
{
public:

	VertexArrayObject();
	~VertexArrayObject();

	void set(uint32_t binding, uint32_t VBO_ID, size_t item_size, size_t stride, size_t offset);

	std::map<uint32_t, BindingDescription> get_binding_map() const;

	void print();

private:
	//key:bindingId - value:bindingDescription
	std::map<uint32_t, BindingDescription> _binding_map;

};

}
