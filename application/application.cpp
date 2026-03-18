#include "application.h"

Application* Application::mInstance = nullptr;

Application* Application::instance()
{
	if (_instance == nullptr)
		_instance = new Application();

	return _instance;
}

LRESULT CALLBACK Wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Application::instance()->handle_message(hWnd, message, wParam, lParam);
	return(DefWindowProc(hWnd, message, wParam, lParam));
}

bool Application::init(HINSTANCE hInstance, const uint32_t& width, const uint32_t& height) {
	_width = width;
	_height = height;

	//初始化窗体类型，并且注册
	register_window_class(hInstance);

	//生成一个窗体，并且显示
	if (!create_window(hInstance))
		return false;

	return true;
}

ATOM Application::register_window_class(HINSTANCE hInstance)
{
	WNDCLASSEXW wndClass;

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//水平/垂直大小发生变化重绘窗口
	wndClass.lpfnWndProc = Wndproc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;		//应用程序句柄
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);//应用程序图标,即任务栏的大图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);//鼠标图标
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//窗口背景色
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = mWindowClassName;//窗口类名
	wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);//窗口标题图标

	return RegisterClassExW(&wndClass);
}

BOOL Application::create_window(HINSTANCE hInstance)
{
	_window_inst = hInstance;

	/*
	* WS_POPUP:不需要标题栏，则不需要边框
	* WS_OVERLAPPEDWINDOW：拥有普通程序主窗口的所有特点，必须有标题且有边框
	*
	* WS_CLIPSIBLINGS:被兄弟窗口挡住区域不绘制
	* WS_CLIPCHILDREN:被子窗口遮挡住的区域不绘制
	*/

	auto dwExStyle = WS_EX_APPWINDOW;
	auto dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	//由于存在标题栏等，所以需要计算中间显示区域的大小,比如PopUp的窗体，就没有标题栏，则不会改变
	RECT windowRect;
	windowRect.left = 0L;
	windowRect.top = 0L;
	windowRect.right = (long)_width;
	windowRect.bottom = (long)_height;
	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	_Hwnd = CreateWindowW(
		_window_class_name,
		(LPCWSTR)"GraphicLearning",	//窗体标题
		dwStyle,
		500,//x位置，相对左上角
		500,//y位置，相对左上角
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,//父窗体
		nullptr,//菜单栏
		hInstance,//程序实例
		nullptr);//额外参数


	if (!_Hwnd)
	{
		return FALSE;
	}

	ShowWindow(_Hwnd, true);
	UpdateWindow(_Hwnd);

	return TRUE;
}

bool Application::peek_message() {
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return _active;
}

void Application::handle_message(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_CLOSE: {
		DestroyWindow(hWnd);//此处销毁窗体,会自动发出WM_DESTROY
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY: {
		PostQuitMessage(0);//发出线程终止请求
		_active = false;
		break;
	}
	}
}