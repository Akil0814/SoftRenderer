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

mai::Point p1{ 500,500,mai::Color::Red , mai::vec2f(0.5f,1.0f)};
mai::Point p2{ 0,0,mai::Color::Blue , mai::vec2f(0.0f,0.0f) };
mai::Point p3{ 1000,0, mai::Color::Green , mai::vec2f(1.0f,0.0f) };

mai::vec4f pos1;
mai::vec4f pos2;
mai::vec4f pos3;

mai::mat4f modelMatrix;
mai::mat4f viewMatrix;
mai::mat4f perspectiveMatrix;
mai::mat4f screenMatrix;

float angle = 0.0f;
float cameraPos = 5.0f;

float speed = 0.01;

void transform()
{
	angle += 0.01f;
	cameraPos += 0.01f;

	//模型变换
	modelMatrix = mai::rotate(mai::mat4f(1.0f), angle, mai::vec3f{ 0.0f, 1.0f, 0.0f });

	//模拟摄像机往后退
	auto cameraModelMatrix = mai::translate(mai::mat4f(1.0f), mai::vec3f{ 0.0f, 0.0f, cameraPos });
	viewMatrix = mai::inverse(cameraModelMatrix);

	//mvp矩阵相乘
	auto sp1 = perspectiveMatrix * viewMatrix * modelMatrix * pos1;
	auto sp2 = perspectiveMatrix * viewMatrix * modelMatrix * pos2;
	auto sp3 = perspectiveMatrix * viewMatrix * modelMatrix * pos3;

	//透视除法（此处故意设计z！=0）
	sp1 /= sp1.w;
	sp2 /= sp2.w;
	sp3 /= sp3.w;

	//屏幕空间
	sp1 = screenMatrix * sp1;
	sp2 = screenMatrix * sp2;
	sp3 = screenMatrix * sp3;

	p1.x = sp1.x;
	p1.y = sp1.y;

	p2.x = sp2.x;
	p2.y = sp2.y;

	p3.x = sp3.x;
	p3.y = sp3.y;
}

void on_render()
{
	MAI_SGL->clear();
}

void prepare()
{
	image01 = mai::Image::create_image("assets/textures/Arcueid_morning_low.png");

	if (image01 == nullptr)
		std::cerr << "false" << std::endl;

	p1.color = mai::RGBA(255, 0, 0, 255);
	p1.uv = mai::vec2f(0.0f, 0.0f);

	p2.color = mai::RGBA(0, 255, 0, 255);
	p2.uv = mai::vec2f(1.0f, 1.0f);

	p3.color = mai::RGBA(0, 0, 255, 255);
	p3.uv = mai::vec2f(1.0f, 0.0f);

	pos1 = { -1.5f, 0.0f, 0.0f, 1.0f };
	pos2 = { 1.5f, 0.0f, 0.0f, 1.0f };
	pos3 = { 0.0f, 2.0f, 0.0f, 1.0f };

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
		//on_render();
		transform();
		MAI_APP->show();
	}

	mai::Image::destroy_image(image01);

	return 0;
}
