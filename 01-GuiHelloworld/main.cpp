#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <sys/time.h>
#include <ZetaDisplayInterface.h>
#include <ZetaCameraInterface.h>

#define DEVICE_WIDTH 854
#define DEVICE_HEIGHT 480

using namespace zeta;

static int HelloWinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc;
	static HWND hwnd;
	switch (message) {
		case MSG_PAINT:	// 窗口绘制开始事件
			hdc = BeginPaint (hWnd);
			TextOut (hdc, DEVICE_WIDTH / 2 - 50, DEVICE_HEIGHT / 2 - 10, "Hello world!"); // 输出文字
			EndPaint (hWnd, hdc);
			return 0;
		case MSG_CREATE:
			hwnd = CreateWindowEx("static", "", WS_CHILD | WS_VISIBLE, WS_EX_NONE, 123, 0, 0, 50, 50, hWnd, NULL);
			SetWindowBkColor(hwnd, RGBA2Pixel(HDC_SCREEN, 0x00, 0x00, 0xff, 0x10));
			break;
		case MSG_CLOSE:
			DestroyMainWindow (hWnd);
			PostQuitMessage (hWnd);
			return 0;
	}
	return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
	MSG msg;
	HWND hMainWnd; // 窗口句柄
	MAINWINCREATE CreateInfo; // 窗口属性
	CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION; // 可见 | 有边界 | 有标题栏
	CreateInfo.dwExStyle = WS_EX_NONE; // 无扩展窗口，如果要双缓存的话要用到
	CreateInfo.spCaption = "First Window";
	CreateInfo.hMenu = 0;
	CreateInfo.hCursor = GetSystemCursor (0);
	CreateInfo.hIcon = 0;
	CreateInfo.MainWindowProc = HelloWinProc; // 定义窗口子程序，后缀Proc一般是指子程序的意思
	CreateInfo.lx = 0;
	CreateInfo.ty = 0;
	CreateInfo.rx = DEVICE_WIDTH; // 窗口宽度
	CreateInfo.by = DEVICE_HEIGHT; // 窗口高度
	 // 背景颜色，如果设置透明的话即最后一个参数是0x00就会显示Logo图片，因为minigui窗口层在Logo层上面
	CreateInfo.iBkColor = RGBA2Pixel(HDC_SCREEN, 0xff, 0x00, 0xff, 0x00);
	CreateInfo.dwAddData = 0;
	CreateInfo.hHosting = HWND_DESKTOP;
	hMainWnd = CreateMainWindow (&CreateInfo); // 根据窗口属性创建窗口

	if (hMainWnd == HWND_INVALID)
	    return -1;
	// 显示窗口
	ShowWindow (hMainWnd, SW_SHOWNORMAL);

	while (GetMessage (&msg, hMainWnd)) {
		fprintf(stderr, "msg\n");
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}
	MainWindowThreadCleanup (hMainWnd);
	return 0;
}
#ifndef _MGRM_PROCESSES
#include <minigui/dti.c>
#endif

