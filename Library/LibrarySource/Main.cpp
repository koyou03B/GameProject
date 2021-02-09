#include <windows.h>
#include <memory>
#include <assert.h>
#include <tchar.h>

#include "Framework.h"

LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Framework* f = reinterpret_cast<Framework*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	//int  i;
	//HDROP hDrop;
	//UINT uFileNo;
	//char szFileName[256];

	switch (msg) {
	case WM_CREATE:
		DragAcceptFiles(hwnd, TRUE);
		break;
	case WM_DROPFILES:
		//hDrop = (HDROP)wparam;
		//uFileNo = DragQueryFile((HDROP)wparam, 0xFFFFFFFF, NULL, 0);
		//for (i = 0; i < (int)uFileNo; i++) {
		//	DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
		//}
		//DragFinish(hDrop);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return f ? f->HandleMessage(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
}

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
	//マウスカーソル削除
	ShowCursor(true);
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(5369);
#endif
	HRESULT hr = CoInitializeEx(0, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		assert(!"Could not Initialize  COMLibrary");


	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = fnWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("REGION MATE");
	wcex.hIconSm = 0;
	RegisterClassEx(&wcex);

	RECT rc = { 0, 0, 1980, 1070 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hwnd = CreateWindow(_T("REGION MATE"), _T("REGION MATE"), WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, instance, NULL);
	ShowWindow(hwnd, cmd_show);



	Framework& f = Framework::GetInstance(hwnd, instance);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&f));
	return f.Run();
}
