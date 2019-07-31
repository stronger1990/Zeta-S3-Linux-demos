#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <sys/time.h>
#include <ZetaDisplayInterface.h>
#include <ZetaCameraInterface.h>
#include <ZetaMediaPlayInterface.h>

#define DEVICE_WIDTH 854
#define DEVICE_HEIGHT 480

using namespace zeta;

static int RZMainWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc;
	switch (message)
	{
		case MSG_PAINT: // 窗口绘制开始事件
			hdc = BeginPaint(hWnd);
			TextOut(hdc, DEVICE_WIDTH / 2 - 50, DEVICE_HEIGHT / 2 - 10, "Hello world!");
			EndPaint(hWnd, hdc);
			return 0;
		case MSG_CLOSE:
			DestroyMainWindow(hWnd);
			PostQuitMessage(hWnd);
			return 0;
	}
	return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain(int argc, const char *argv[])
{
	MSG msg;
	HWND hMainWnd; // 窗口句柄
	MAINWINCREATE CreateInfo; // 窗口属性
	CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
	CreateInfo.dwExStyle = WS_EX_NONE;
	CreateInfo.spCaption = "Title";
	CreateInfo.hMenu = 0;
	CreateInfo.hCursor = GetSystemCursor(0);
	CreateInfo.hIcon = 0;
	CreateInfo.MainWindowProc = RZMainWinProc;
	CreateInfo.lx = 0;
	CreateInfo.ty = 0;
	CreateInfo.rx = DEVICE_WIDTH;
	CreateInfo.by = DEVICE_HEIGHT;
	// 这里一定要透明才能看到摄像头画面，因为如果使用
	CreateInfo.iBkColor = RGBA2Pixel(HDC_SCREEN, 0x00, 0x00, 0x00, 0x00);
	CreateInfo.dwAddData = 0; // 通过这个传递参数，默认是0，也可以传递指针
	CreateInfo.hHosting = HWND_DESKTOP;
	hMainWnd = CreateMainWindow(&CreateInfo);

	if (hMainWnd == HWND_INVALID)
		return -1;
	ShowWindow(hMainWnd, SW_SHOWNORMAL);

	ZETA_RECT rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = DEVICE_HEIGHT;
	rect.h = DEVICE_WIDTH;

	zeta::ZetaCamera *mZetaCamera = new zeta::ZetaCamera(rect, 0);
	//mZetaCamera->initCamera(DEVICE_WIDTH, DEVICE_HEIGHT, DEVICE_WIDTH, DEVICE_HEIGHT, 30, 10, 1, 0/*90*/);
	// 如果是0，则图像变形，左侧有时间也被旋转了，所以这里应是90°才正确，看到的不是横的图像，和窗口相反，这个是摄像头的原因
	// 也可能是图像尺寸缩放比例不对而被拉伸了
	mZetaCamera->initCamera(1920/*输出图像宽*/, 1080/*输出图像高*/, DEVICE_WIDTH/*预览图像宽*/, DEVICE_HEIGHT/*预览图像高*/, 30/*帧率*/, 10/*VideoCaptureBufferNum*/, 1/*VideoBufferFrameNum*/, 0/*旋转角度*/); 
	mZetaCamera->startPreview();

	while (GetMessage(&msg, hMainWnd))
	{
		// 这个可以直接在CRT串口打印，如果用ZLOG则要在窗口输入zlog命令进入才能查看调试信息
		//fprintf(stderr, "msg\n"); 
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	MainWindowThreadCleanup(hMainWnd);
	return 0;
}
#ifndef _MGRM_PROCESSES
#include <minigui/dti.c>
#endif
