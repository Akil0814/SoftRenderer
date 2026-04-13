#include <iostream>
#include <Windows.h>

#include "application/application.h"
#include "application/image.h"

#include "gpu/gpu.h"
#include "math/math.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )//更改main入口


uint32_t WIDTH = 1080;
uint32_t HEIGHT = 720;

mai::Image* image01 = nullptr;

//三个属性对应vbo
uint32_t positionVbo = 0;
uint32_t colorVbo = 0;
uint32_t uvVbo = 0;

//三角形的indices
uint32_t ebo = 0;

//本三角形专属vao
uint32_t vao = 0;

mai::mat4f modelMatrix;
mai::mat4f viewMatrix;
mai::mat4f perspectiveMatrix;
mai::mat4f screenMatrix;

float angle = 0.0f;
float cameraPos = 5.0f;

float speed = 0.01;

void on_render()
{
	MAI_SGL->clear();
}

void prepare()
{
	image01 = mai::Image::create_image("assets/textures/Arcueid_morning_low.png");

	if (image01 == nullptr)
		std::cerr << "false" << std::endl;

	float positions[] = {
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
	};

	float colors[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
	};

	float uvs[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	uint32_t indices[] = { 0, 1, 2 };

	//生成indices对应ebo
	ebo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, ebo);
	MAI_SGL->buffer_data(MAI_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
	MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, 0);

	//生成vao并且绑定
	vao = MAI_SGL->gen_vertex_array();
	MAI_SGL->bind_vertex_array(vao);

	//生成每个vbo，绑定后，设置属性ID及读取参数
	positionVbo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, positionVbo);
	MAI_SGL->buffer_data(MAI_ARRAY_BUFFER, sizeof(float) * 9, positions);
	MAI_SGL->vertex_attribute_pointer(0, 3, 3 * sizeof(float), 0);

	colorVbo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, colorVbo);
	MAI_SGL->buffer_data(MAI_ARRAY_BUFFER, sizeof(float) * 12, colors);
	MAI_SGL->vertex_attribute_pointer(1, 4, 4 * sizeof(float), 0);

	uvVbo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, uvVbo);
	MAI_SGL->buffer_data(MAI_ARRAY_BUFFER, sizeof(float) * 6, uvs);
	MAI_SGL->vertex_attribute_pointer(2, 2, 2 * sizeof(float), 0);

	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, 0);
	MAI_SGL->bind_vertex_array(0);

	MAI_SGL->printVAO(vao);

	perspectiveMatrix = mai::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	screenMatrix = mai::screen_matrix<float>(WIDTH, HEIGHT);
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

	mai::Image::destroy_image(image01);

	return 0;
}
