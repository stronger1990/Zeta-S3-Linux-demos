#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <sys/time.h>

#include <ZetaDisplayInterface.h>
#include <ZetaCameraInterface.h>
#include <ZetaMediaPlayInterface.h>

#define DEVICE_WIDTH 854
#define DEVICE_HEIGHT 480
// ID，用于判断不同控件，这里表示拍照按钮ID
#define IDC_BUTTON_TAKEPIC 100
// 录像按钮ID
#define IDC_BUTTON_RECORD 101

using namespace zeta;

static BITMAP bmpTakePic;
static BITMAP bmpRecord;
static bool isSelected;

void clickTakePic(HWND hWnd) // 拍照处理函数
{
	// 插入串口就能看到打印信息了
	fprintf(stderr, "----------Take Picture----------\n");
}

void clickRecord(HWND hWnd) // 录像处理函数
{
	fprintf(stderr, "----------Record----------\n");
	if (isSelected)
	{
		isSelected = false;
		LoadBitmapFromFile(HDC_SCREEN, &bmpRecord, "/etc/res/topbar/recorder.png");
		UpdateWindow(GetDlgItem(hWnd, IDC_BUTTON_RECORD), TRUE);
		// UnloadBitmap(&bmpTakePic);
		InvalidateRect(hWnd, NULL, TRUE);
	}
	else
	{
		isSelected = true;
		LoadBitmapFromFile(HDC_SCREEN, &bmpRecord, "/etc/res/topbar/recording.png");
		UpdateWindow(GetDlgItem(hWnd, IDC_BUTTON_RECORD), TRUE);
		InvalidateRect(hWnd, NULL, TRUE);
	}
}

static int RZMainWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc;

	switch (message)
	{
	case MSG_CREATE:
		if (LoadBitmapFromFile(HDC_SCREEN, &bmpTakePic, "/etc/res/topbar/tack_pic.png"))
			return 0;
		// 创建拍照按钮
		CreateWindow(CTRL_BUTTON, "", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_BITMAP | BS_REALSIZEIMAGE | BS_NOTIFY, IDC_BUTTON_TAKEPIC, 16, DEVICE_HEIGHT - 80 - 16, 80, 80, hWnd, (DWORD)&bmpTakePic /*(DWORD) GetSystemBitmap (SYSBMP_CLOSE)*/);

		if (LoadBitmapFromFile(HDC_SCREEN, &bmpRecord, "/etc/res/topbar/recorder.png"))
			return 0;
		// 创建录像按钮
		CreateWindow(CTRL_BUTTON, "", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_BITMAP | BS_REALSIZEIMAGE | BS_NOTIFY, IDC_BUTTON_RECORD, 16 + 80 + 16, DEVICE_HEIGHT - 80 - 16, 80, 80, hWnd, (DWORD)&bmpRecord);
		break;
	case MSG_COMMAND:
	{
		int id = LOWORD(wParam);
		int nc = HIWORD(wParam);

		if (id == IDC_BUTTON_TAKEPIC && nc == BN_CLICKED)
		{ //控件的消息处理，具体拍照处理函数
			clickTakePic(hWnd);
		}
		else if (id == IDC_BUTTON_RECORD && nc == BN_CLICKED)
		{ //控件的消息处理，具体录像处理函数
			clickRecord(hWnd);
		}
	}
	break;
	case MSG_PAINT: // 窗口绘制开始事件
		hdc = BeginPaint(hWnd);
		TextOut(hdc, DEVICE_WIDTH - 200, 16, "Take Picture And Record");
		EndPaint(hWnd, hdc);
		return 0;
	case MSG_CLOSE:
		DestroyMainWindow(hWnd);
		DestroyMainWindow(hWnd);
		PostQuitMessage(hWnd);
		return 0;
	}
	return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain(int argc, const char *argv[])
{
	MSG msg;
	HWND hMainWnd;			  // 窗口句柄
	MAINWINCREATE CreateInfo; // 窗口属性
	CreateInfo.dwStyle = WS_VISIBLE /* | WS_BORDER | WS_CAPTION */;
	CreateInfo.dwExStyle = WS_EX_NONE; // 没有扩展窗口
	CreateInfo.spCaption = "";
	CreateInfo.hMenu = 0;
	CreateInfo.hCursor = GetSystemCursor(0);
	CreateInfo.hIcon = 0;
	CreateInfo.MainWindowProc = RZMainWinProc;
	CreateInfo.lx = 0;
	CreateInfo.ty = 0;
	CreateInfo.rx = DEVICE_WIDTH;										  // 宽
	CreateInfo.by = DEVICE_HEIGHT;										  // 高
	CreateInfo.iBkColor = RGBA2Pixel(HDC_SCREEN, 0x00, 0x00, 0x00, 0x00); // minigui窗口层透明
	CreateInfo.dwAddData = 0;											  // 通过这个传递参数，默认是0，也可以传递指针
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
	mZetaCamera->initCamera(1920 /*输出图像宽*/, 1080 /*输出图像高*/, DEVICE_WIDTH /*预览图像宽*/, DEVICE_HEIGHT /*预览图像高*/, 30 /*帧率*/, 10 /*VideoCaptureBufferNum*/, 1 /*VideoBufferFrameNum*/, 0 /*旋转角度*/);
	mZetaCamera->startPreview();

	while (GetMessage(&msg, hMainWnd))
	{
		// 这个可以直接在CRT串口打印，如果用ZLOG则要在窗口输入zlog命令进入才能查看调试信息
		//fprintf(stderr, "msg\n");
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	MainWindowThreadCleanup(hMainWnd);
	UnloadBitmap(&bmpTakePic);
	return 0;
}
#ifndef _MGRM_PROCESSES
#include <minigui/dti.c>
#endif
