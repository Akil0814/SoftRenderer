#pragma once
#include "../global/base.h"
#include<Windows.h>

#define App Application::instance()

class Application
{
public:
	static Application* instance();

	//注册窗体类，创建一个窗体，显示
	bool init(HINSTANCE hInstance, const uint32_t& width = 800, const uint32_t& height = 600);

	//托管了wndProc捕获的消息，并且进行处理
	LRESULT handle_message(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//每一帧/每一次循环，都会调用，捕获以及分发窗体消息
	bool peek_message();

private:
	Application() = default;
	~Application() = default;

	BOOL create_window(HINSTANCE hInstance);
	ATOM register_window_class(HINSTANCE hInstance);

private:
	static Application* _instance;

	bool _active = {true};

	HINSTANCE _window_inst;
	WCHAR _window_class_name[100] = L"AppWindow";
	HWND _hwnd;

	int _width = 800;
	int _height = 600;

};