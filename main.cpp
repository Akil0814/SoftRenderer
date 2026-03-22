#include <iostream>
#include <Windows.h>

#include "application/application.h"
#include "gpu/gpu.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )//更改main入口

void on_render()
{
	sgl->clear();

	//for (uint32_t i = 0; i < App->get_width(); ++i)
	//{
	//	sgl->draw_point(i, 100, RGBA(i, i, i));
	//}

	sgl->clear();
	sgl->draw_point(100, 100, RGBA(255, 255, 255));
	App->show();

	sgl->draw_point(200, 200, RGBA(255, 255, 255));
	sgl->draw_point(201, 200, RGBA(255, 255, 255));
	sgl->draw_point(200, 201, RGBA(255, 255, 255));
	sgl->draw_point(201, 201, RGBA(255, 255, 255));
}

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance, //本应用程序实例句柄，唯一指代当前程序
	_In_opt_ HINSTANCE hPrevInstance, //本程序前一个实例，一般是null
	_In_ LPWSTR lpCmdLine, //应用程序运行参数
	_In_ int nCmdShow) //窗口如何显示（最大化、最小化、隐藏），不需理会
{
	if (!App->init(hInstance, 800, 600))
		return -1;

	//将bmp指向的内存配置到sgl当中
	sgl->init_surface(App->get_width(), App->get_height(),App->get_canvas());

	bool active = true;
	while (active)
	{
		active = App->peek_message();
		on_render();
		App->show();
	}

	return 0;
}