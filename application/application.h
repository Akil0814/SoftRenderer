#pragma once
#include "../MAI_SGL/base.h"
#include<Windows.h>

#define MAI_APP mai::Application::instance()


namespace mai
{

class Application
{
public:
	static Application* instance();

	//注册窗体类，创建一个窗体，显示
	bool init(HINSTANCE hInstance, uint32_t width = 800, uint32_t height = 600);

	//托管了wndProc捕获的消息，并且进行处理
	LRESULT handle_message(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//每一帧/每一次循环，都会调用，捕获以及分发窗体消息
	bool peek_message();

	void show();

	uint32_t get_width() const { return _width; }
	uint32_t get_height() const { return _height; }
	void* get_canvas() const { return _canvas_buffer; }

private:
	Application() = default;
	~Application();

private:
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

	HDC _hDC;//当前窗口主dc
	HDC	_CanvasDC;//创建的于mhDC相兼容的绘图用的dc
	HBITMAP _hBmp;//_CanvasDC内部生成的bitmap
	void* _canvas_buffer{ nullptr };//_hBmp对应的内存起始位置指针

};

}
