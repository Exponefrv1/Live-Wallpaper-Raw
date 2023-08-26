#include <Windows.h>
#include <Dshow.h>
#include "main.h"
#pragma comment(lib, "Quartz")
#pragma comment(lib, "Strmiids")

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
) {

	HWND progman = FindWindow(L"Progman", NULL);
	
	SendMessageTimeout(
		progman,
		0x052C,
		NULL,
		NULL,
		SMTO_NORMAL,
		1000,
		NULL
	);
	
	EnumWindows(EnumWindowsProc, 0);
	
	if (!workerw) {
		return 1;
	}

	const wchar_t CLASS_NAME[] = L"Wallpapers Class";

	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	HWND wallpapers = CreateWindowEx(
		WS_EX_NOACTIVATE,
		CLASS_NAME,
		L"Live Wallpapers",
		WS_CHILD | WS_VISIBLE,
		0, 0,
		WIDTH, HEIGHT,
		workerw,
		NULL,
		NULL,
		NULL
	);

	if (InitializeDirectShow(wallpapers) != S_OK) {
		return 1;
	}

	LONGLONG targetPosition;
	pMediaSeeking->GetDuration(&targetPosition);

	pControl->Run();

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		LONGLONG currentPosition;
		pMediaSeeking->GetCurrentPosition(&currentPosition);
		if (currentPosition >= targetPosition) {
			pControl->Stop();
			LONGLONG startPosition = 0;
			pMediaSeeking->SetPositions(
				&startPosition,
				AM_SEEKING_AbsolutePositioning,
				NULL,
				AM_SEEKING_NoPositioning
			);
			pControl->Run();
			OutputDebugString(L"Position is set to 0\n");
		}
	}
	return 0;
}

HRESULT InitializeDirectShow(HWND hwnd) {
	CoInitialize(NULL);
	CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
	pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
	pGraph->QueryInterface(IID_IVideoWindow, (void**)&pVidWin);
	pGraph->QueryInterface(IID_IMediaSeeking, (void**)&pMediaSeeking);
	pMediaSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
	pGraph->RenderFile(WP_PATH, NULL);
	pVidWin->put_Owner((OAHWND)hwnd);
	pVidWin->put_WindowStyle(WS_CHILD | WS_VISIBLE);
	pVidWin->SetWindowPosition(0, 0, WIDTH, HEIGHT);
	return S_OK;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT:
			break;
		case WM_DESTROY:
			pControl->Stop();
			pVidWin->put_Visible(OAFALSE);
			pGraph->Release();
			pControl->Release();
			pVidWin->Release();
			pMediaSeeking->Release();
			CoUninitialize();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
	HWND window = FindWindowEx(
		hWnd,
		NULL,
		L"SHELLDLL_DefView",
		NULL
	);
	if (window) {
		workerw = FindWindowEx(
			NULL,
			hWnd,
			L"WorkerW",
			NULL
		);
	}
	return true;
}
