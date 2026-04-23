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

		BindingDescription& description = iter->second;
		description._VBO_ID = VBO_ID;
		description._item_size = item_size;
		description._stride = stride;
		description._offset = offset;
	}

	std::map<uint32_t, BindingDescription> VertexArrayObject::get_binding_map() const
	{
		return _binding_map;
	}

	void VertexArrayObject::print()
	{
		std::cout << "VAO Attribute Map is:" << std::endl;
		for (auto& item : _binding_map)
		{
			std::cout << "Binding ID:" << item.first << std::endl;
			std::cout << "----VBO ID:" << item.second._VBO_ID << std::endl;
			std::cout << "----ItemSize:" << item.second._item_size << std::endl;
			std::cout << "----Stride:" << item.second._stride << std::endl;
			std::cout << "----Offset:" << item.second._offset << std::endl;
		}
	}


}
