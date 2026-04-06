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


void on_render()
{
	MAI_SGL->clear();

	mai::Point p1{ 200,200,mai::Color::Red };
	mai::Point p2{ 100,100,mai::Color::Blue };
	mai::Point p3{ 300,100, mai::Color::Green };

	mai::Point p4{ 300,300,mai::Color::Red };
	mai::Point p5{ 100,100,mai::Color::Blue };
	mai::Point p6{ 300,100, mai::Color::Green };



	MAI_SGL->draw_triangle(p1, p2, p3);
	MAI_SGL->draw_triangle(p1, p3, p4);

	MAI_SGL->draw_image_with_alpha(image01, 230);
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

	mai::Vector2<int> test;
	mai::Vector3<int> test2;
	mai::Vector4<int> test3;

	image01 = mai::Image::create_image("assets/textures/Arcueid_morning.png");
	MAI_SGL->set_blending(MAI_TRUE);

	if (image01 == nullptr)
		std::cerr << "false" << std::endl;

	bool active = true;
	while (active)
	{
		active = MAI_APP->peek_message();
		on_render();
		MAI_APP->show();
	}

	return 0;
}
