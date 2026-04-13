#include "application.h"

namespace mai
{

Application* Application::_instance = nullptr;

Application* Application::instance()
{
	if (_instance == nullptr)
		_instance = new Application();

	return _instance;
}

Application::~Application()
{
	if (_hBmp)
		DeleteObject(_hBmp);
	if (_CanvasDC)
		DeleteDC(_CanvasDC);
	if (_hwnd && _hDC)
		ReleaseDC(_hwnd, _hDC);
}

//HWND hWnd:窗口句柄  UINT message:消息类型  WPARAM wParam:补充参数1  LPARAM lParam:补充参数 2
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)//消息中转站
{	return Application::instance()->handle_message(hWnd, message, wParam, lParam);	}

//HINSTANCE hInstance 本应用程序实例句柄，唯一指代当前程序
bool Application::init(HINSTANCE hInstance, const uint32_t& width, const uint32_t& height)
{
	_width = width;
	_height = height;

	//初始化窗体类型，并且注册
	if (!register_window_class(hInstance))
		return false;

	//生成一个窗体，并且显示
	if (!create_window(hInstance))
		return false;

//初始化画布
/*
* DC：Device Context 设备上下文描述对象
* 每个窗口都有自己对应的设备区域映射，即hDC
* 这里创建一个与本窗口兼容的DC，CanvasDC
* 可以从mCanvasDC向hDC拷贝绘图数据内容
*/
	_hDC = GetDC(_hwnd);//获取窗口 DC
	_CanvasDC = CreateCompatibleDC(_hDC);//创建兼容的内存 DC// CanvasDC->后台画板

	BITMAPINFO  bmpInfo{};
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = _width;
	bmpInfo.bmiHeader.biHeight = _height;//倒置的 左下角是0
	//bmpInfo.bmiHeader.biHeight = -_height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB; //实际上存储方式为bgra

	//创建与hMem兼容的位图,其实实在hMem指代的设备上划拨了一块内存，让CanvasBuffer指向它
	_hBmp = CreateDIBSection(_CanvasDC, &bmpInfo, DIB_RGB_COLORS, (void**)&_canvas_buffer, 0, 0);//在这里创建buffer的内存

	//一个设备可以创建多个位图，本设备使用_hBmp作为激活位图，对CanvasDC的内存拷出，其实就是拷出了hBmp的数据
	SelectObject(_CanvasDC, _hBmp);

	memset(_canvas_buffer, 0, _width * _height * 4); //清空buffer为0


	return true;
}

//HINSTANCE hInstance 本应用程序实例句柄，唯一指代当前程序
ATOM Application::register_window_class(HINSTANCE hInstance)
{
	WNDCLASSEXW wnd_class = {};

	wnd_class.cbSize = sizeof(WNDCLASSEXW);//设置结构体大小
	wnd_class.style = CS_HREDRAW | CS_VREDRAW;//水平/垂直大小发生变化重绘窗口
	wnd_class.lpfnWndProc = WndProc;//这个窗口类创建出来的窗口，收到系统消息时，都交给 WndProc 处理
	wnd_class.cbClsExtra = 0;//类额外内存:窗口类本身
	wnd_class.cbWndExtra = 0;//类额外内存:每个窗口实例->额外预留多少字节内存
	wnd_class.hInstance = hInstance;//应用程序句柄
	wnd_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);//应用程序图标,即任务栏的大图标
	wnd_class.hCursor = LoadCursor(NULL, IDC_ARROW);//鼠标图标
	wnd_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//窗口背景色
	wnd_class.lpszMenuName = NULL;//菜单名
	wnd_class.lpszClassName = _window_class_name;//窗口类名
	wnd_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);//窗口标题小图标

	ATOM result = RegisterClassExW(&wnd_class);
	if (result == 0)
	{
		DWORD err = GetLastError();
		std::cerr << "RegisterClassExW failed. Error = " << err << '\n';
		return 0;
	}

	return result;
}


//按照前面注册好的窗口类，真正创建一个 Windows 窗口，并把它显示出来
BOOL Application::create_window(HINSTANCE hInstance)
{
	_window_inst = hInstance;

	/*
	 WS_POPUP:不需要标题栏，则不需要边框
	 WS_OVERLAPPEDWINDOW：拥有普通程序主窗口的所有特点，必须有标题且有边框
	 WS_CLIPSIBLINGS:被兄弟窗口挡住区域不绘制
	 WS_CLIPCHILDREN:被子窗口遮挡住的区域不绘制
	 */

	DWORD dw_style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;//普通窗口样式
	DWORD dw_ex_style = WS_EX_APPWINDOW;//扩展样式

	//由于存在标题栏等，所以需要计算中间显示区域的大小,比如PopUp的窗体，就没有标题栏，则不会改变
	RECT window_rect = {};//定义目标客户区矩形
	window_rect.left = 0L;
	window_rect.top = 0L;
	window_rect.right = (long)_width;
	window_rect.bottom = (long)_height;

	if (!AdjustWindowRectEx(&window_rect, dw_style, FALSE, dw_ex_style))//把客户区大小换算成整个窗口大小
	{
		DWORD err = GetLastError();
		std::cerr << "AdjustWindowRectEx failed. Error = " << err << '\n';
		return FALSE;
	}

	//创建窗口
	_hwnd = CreateWindowW(
		_window_class_name,
		L"Graphic",//窗体标题
		dw_style,
		50,//x位置，相对左上角
		50,//y位置，相对左上角
		window_rect.right - window_rect.left,//窗口总宽高
		window_rect.bottom - window_rect.top,
		nullptr,//父窗体
		nullptr,//菜单栏
		_window_inst,//程序实例
		nullptr);//额外参数


	if (!_hwnd)
		return FALSE;

	ShowWindow(_hwnd, SW_SHOW);
	UpdateWindow(_hwnd);

	return TRUE;
}


// 从消息队列里取一条消息出来，然后交给系统分发处理
bool Application::peek_message()//取消息
{
	MSG msg;

	//从消息队列里取一条消息出来，然后交给系统分发处理
	//如果有，就取出一条，放进 msg, PM_REMOVE 表示取出来后顺便从队列里移除
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);//键盘字符输入辅助处理
		DispatchMessage(&msg);//把消息真正派发给对应窗口的 WndProc//把消息转发到窗口
	}

	return _active;
}

//真正处理窗口消息
LRESULT Application::handle_message(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)//处理消息
{
	switch (message)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);//销毁窗口 -> WM_DESTROY
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// 在这里绘制

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY://
		PostQuitMessage(0);//向线程消息队列里放一个退出消息，告诉整个消息循环该结束了
		_active = false;
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);//如果不是特别处理的消息，就让 Windows 默认逻辑处理
}

void Application::show()
{
	BitBlt(_hDC, 0, 0, _width, _height, _CanvasDC, 0, 0, SRCCOPY);
}

}
