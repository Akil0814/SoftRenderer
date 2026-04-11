#include <iostream>
#include <Windows.h>

#include "application/application.h"
#include "application/image.h"

#include "gpu/gpu.h"
#include "math/math.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )//更改main入口


mai::Image* image01 = nullptr;

//mai::Point p1{ 500,500,mai::Color::Red , mai::vec2f(0.5f,1.0f)};
//mai::Point p2{ 0,0,mai::Color::Blue , mai::vec2f(0.0f,0.0f) };
//mai::Point p3{ 1000,0, mai::Color::Green , mai::vec2f(1.0f,0.0f) };

mai::Point p1;
mai::Point p2;
mai::Point p3;

mai::Point q1;
mai::Point q2;
mai::Point q3;

float speed = 0.01;

void test()
{
	mai::Mat4f m4;
	m4.print_matrix();
	mai::vec3f v3(13.0f,21.0f,11.0f);
	m4=mai::rotate(m4, 90, v3);
	m4.print_matrix();
}


void changeUV() {
	p1.uv.x += speed;
	p2.uv.x += speed;
	p3.uv.x += speed;
	q1.uv.x += speed;
	q2.uv.x += speed;
	q3.uv.x += speed;

	p1.uv.y += speed;
	p2.uv.y += speed;
	p3.uv.y += speed;
	q1.uv.y += speed;
	q2.uv.y += speed;
	q3.uv.y += speed;
}

void on_render()
{
	changeUV();

	MAI_SGL->clear();
	MAI_SGL->set_texture(image01);

	MAI_SGL->set_bilinear(MAI_TRUE);
	MAI_SGL->draw_triangle(p1, p2, p3);

	MAI_SGL->set_bilinear(MAI_FALSE);
	MAI_SGL->draw_triangle(q1, q2, q3);
}

void prepare()
{
	image01 = mai::Image::create_image("assets/textures/Arcueid_morning_low.png");
	test();

	if (image01 == nullptr)
		std::cerr << "false" << std::endl;

	p1.x = 0;
	p1.y = 0;
	p1.color = mai::RGBA(255, 0, 0, 255);
	p1.uv = mai::vec2f(0.0f, 0.0f);

	p2.x = 400;
	p2.y = 300;
	p2.color = mai::RGBA(0, 255, 0, 255);
	p2.uv = mai::vec2f(1.0f, 1.0f);

	p3.x = 400;
	p3.y = 0;
	p3.color = mai::RGBA(0, 0, 255, 255);
	p3.uv = mai::vec2f(1.0f, 0.0f);

	q1.x = 0;
	q1.y = 0;
	q1.color = mai::RGBA(255, 0, 0, 255);
	q1.uv = mai::vec2f(0.0f, 0.0f);

	q2.x = 0;
	q2.y = 300;
	q2.color = mai::RGBA(0, 255, 0, 255);
	q2.uv = mai::vec2f(0.0f, 1.0f);

	q3.x = 400;
	q3.y = 300;
	q3.color = mai::RGBA(0, 0, 255, 255);
	q3.uv = mai::vec2f(1.0f, 1.0f);

	//MAI_SGL->set_texture_wrap(MAI_TEXTURE_WRAP_MIRROR);
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

	return 0;
}
