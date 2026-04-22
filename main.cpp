#include <iostream>
#include <Windows.h>

#include "application/application.h"
#include "application/image.h"

#include "gpu/data_structures.h"
#include "gpu/shader/default_shader.h"
#include "gpu/shader/texture_shader.h"
#include "gpu/gpu.h"
#include "gpu/texture.h"

#include "math/math.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )//更改main入口


uint32_t WIDTH = 1080;
uint32_t HEIGHT = 720;

mai::Image* image = nullptr;
uint32_t texture = 0;

//三个属性对应vbo
uint32_t positionVbo = 0;
uint32_t colorVbo = 0;
uint32_t uvVbo = 0;

//三角形的indices
uint32_t ebo = 0;

//本三角形专属vao
uint32_t vao = 0;

mai::TextureShader* textureShader = nullptr;
mai::DefaultShader* defaultShader = nullptr;

mai::mat4f modelMatrix;
mai::mat4f viewMatrix;
mai::mat4f perspectiveMatrix;

float angle = 0.0f;
float cameraPos = 5.0f;
float speed = 0.01;

float camera_z = 3.0f;

void transform()
{
	angle += 0.01f;
	camera_z -= 0.01f;
	//模型变换
	modelMatrix = mai::rotate(mai::mat4f(1.0f), angle, mai::vec3f{ 0.0f, 1.0f, 0.0f });
}

void on_render()
{
	transform();

	textureShader->_model_matrix = modelMatrix;
	textureShader->_view_matrix = viewMatrix;
	textureShader->_projection_matrix = perspectiveMatrix;
	textureShader->_diffuse_texture = texture;

	defaultShader->_model_matrix = modelMatrix;
	defaultShader->_view_matrix = viewMatrix;
	defaultShader->_projection_matrix = perspectiveMatrix;

	MAI_SGL->clear();

	MAI_SGL->bind_vertex_array(vao);
	MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, ebo);

	MAI_SGL->use_program(textureShader);
	MAI_SGL->draw_element(MAI_DRAW_TRIANGLES, 0, 6);

	MAI_SGL->use_program(defaultShader);
	MAI_SGL->draw_element(MAI_DRAW_TRIANGLES, 6, 3);
}

void prepare()
{
	textureShader = new mai::TextureShader();
	defaultShader = new mai::DefaultShader();


	image = mai::Image::create_image("assets/textures/mai.png");
	if (image == nullptr)
		std::cerr << "false" << std::endl;

	if (image != nullptr)
	{
		texture = MAI_SGL->get_texture();
		MAI_SGL->bind_texture(texture);
		MAI_SGL->tex_image_2D(image->_width, image->_height, image->_data);
		MAI_SGL->tex_parameter(MAI_TEXTURE_FILTER, MAI_TEXTURE_FILTER_LINEAR);
		MAI_SGL->tex_parameter(MAI_TEXTURE_WRAP_U, MAI_TEXTURE_WRAP_REPEAT);
		MAI_SGL->tex_parameter(MAI_TEXTURE_WRAP_V, MAI_TEXTURE_WRAP_REPEAT);
		MAI_SGL->bind_texture(0);
	}


	perspectiveMatrix = mai::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	auto cameraModelMatrix = mai::translate(mai::mat4f(1.0f), mai::vec3f{ 0.0f, 0.0f, 3.0f });
	viewMatrix = mai::inverse(cameraModelMatrix);



	MAI_SGL->disable(MAI_CULL_FACE);
	MAI_SGL->enable(MAI_BLENDING);
	//MAI_SGL->disable(MAI_DEPTH_TEST);

	float positions[] =
	{
		-0.70f, -0.60f, 0.25f,
		-0.70f,  0.60f, 0.25f,
		 0.50f,  0.60f, 0.25f,
		 0.50f, -0.60f, 0.25f,

		-0.35f, -0.55f, -0.25f,
		0.15f, 0.45f, -0.25f,
		0.70f, -0.25f, -0.25f,
	};

	float colors[] =
	{
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		0.2f, 0.4f, 1.0f, 0.3f,
		0.2f, 0.8f, 1.0f, 0.3f,
		0.5f, 1.0f, 0.9f, 0.3f,
	};

	float uvs[] =
	{
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6 };

	//生成indices对应ebo
	ebo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, ebo);
	MAI_SGL->buffer_data(MAI_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 9, indices);
	MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, 0);

	//生成vao并且绑定
	vao = MAI_SGL->gen_vertex_array();
	MAI_SGL->bind_vertex_array(vao);

	//生成每个vbo，绑定后，设置属性ID及读取参数
	positionVbo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, positionVbo);
	MAI_SGL->buffer_data(MAI_ARRAY_BUFFER, sizeof(float) * 21, positions);
	MAI_SGL->vertex_attribute_pointer(0, 3, 3 * sizeof(float), 0);

	colorVbo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, colorVbo);
	MAI_SGL->buffer_data(MAI_ARRAY_BUFFER, sizeof(float) * 28, colors);
	MAI_SGL->vertex_attribute_pointer(1, 4, 4 * sizeof(float), 0);

	uvVbo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, uvVbo);
	MAI_SGL->buffer_data(MAI_ARRAY_BUFFER, sizeof(float) * 14, uvs);
	MAI_SGL->vertex_attribute_pointer(2, 2, 2 * sizeof(float), 0);

	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, 0);
	MAI_SGL->bind_vertex_array(0);

	MAI_SGL->printVAO(vao);
}


int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance, //本应用程序实例句柄，唯一指代当前程序
	_In_opt_ HINSTANCE hPrevInstance, //本程序前一个实例，一般是null
	_In_ LPWSTR lpCmdLine, //应用程序运行参数
	_In_ int nCmdShow) //窗口如何显示（最大化、最小化、隐藏），不需理会
{
	if (!MAI_APP->init(hInstance, 1080, 720))
		return -1;

	//将bmp指向的内存配置到sgl当中
	MAI_SGL->init_surface(MAI_APP->get_width(), MAI_APP->get_height(), MAI_APP->get_canvas());

	prepare();


	bool active = true;
	while (active)
	{
		active = MAI_APP->peek_message();
		on_render();
		MAI_APP->show();
	}

	MAI_SGL->delete_texture(texture);
	mai::Image::destroy_image(image);

	return 0;
}
