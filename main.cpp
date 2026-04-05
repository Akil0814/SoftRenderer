#include <iostream>
#include <Windows.h>
#include <cmath>

#include "application/application.h"
#include "gpu/gpu.h"
#include "math/math.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )//更改main入口


void on_render()
{
	MAI_sgl->clear();

	mai::Point p1{ 100,200,mai::Color::Red };
	mai::Point p2{ 200,100,mai::Color::Blue };
	mai::Point p3{ 200,200, mai::Color::Green };

	MAI_sgl->draw_line(p1, p2);
	MAI_sgl->draw_line(p2, p3);
	MAI_sgl->draw_line(p3, p1);

}


int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance, //本应用程序实例句柄，唯一指代当前程序
	_In_opt_ HINSTANCE hPrevInstance, //本程序前一个实例，一般是null
	_In_ LPWSTR lpCmdLine, //应用程序运行参数
	_In_ int nCmdShow) //窗口如何显示（最大化、最小化、隐藏），不需理会
{
	if (!MAI_App->init(hInstance, 800, 600))
		return -1;

	//将bmp指向的内存配置到sgl当中
	MAI_sgl->init_surface(MAI_App->get_width(), MAI_App->get_height(), MAI_App->get_canvas());

	mai::Vector2<int> test;
	mai::Vector3<int> test2;
	mai::Vector4<int> test3;
	

	bool active = true;
	while (active)
	{
		active = MAI_App->peek_message();
		on_render();
		MAI_App->show();
	}

	return 0;
}
