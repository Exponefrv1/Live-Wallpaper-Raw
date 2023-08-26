#include <Windows.h>
#include <Dshow.h>
#include "main.h"
#pragma comment(lib, "Quartz")
#pragma comment(lib, "Strmiids")

// When you change the wallpaper a new WorkerW window is created.
// This window is behind the desktop icons, so we need to create such a window for our purpose.
// To do this, we send a message to a Program Manager.
// Next, we need to get the handler of the created window, we use EnumWindows.
// From there, we can check if the current window contains a child named "SHELLDLL_DefView"
// which indicates that the current window represents the desktop icons.
// We then take the next sibling of that window.
// After we got the handler, we can display whatever we want in this window.
// In my case, I created a new window that is a child of WorkerW and rendered a video file in it.

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
) {

	// Find Program Manager window
	HWND progman = FindWindow(L"Progman", NULL);
	
	// Send message to Program Manager to create a new WorkerW window
	SendMessageTimeout(
		progman,
		0x052C,
		NULL,
		NULL,
		SMTO_NORMAL,
		1000,
		NULL
	);
	
	// Obtain handler for this WorkerW window
	EnumWindows(EnumWindowsProc, 0);
	
	if (!workerw) {
		return 1;
	}

	// Create child window
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


	// Initialize DirectShow for rendering
	if (InitializeDirectShow(wallpapers) != S_OK) {
		return 1;
	}

	// Get duration of videofile for seeking
	LONGLONG targetPosition;
	pMediaSeeking->GetDuration(&targetPosition);

	// Run render
	pControl->Run();

	// Message loop
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		// Get current position of playing video and set it to 0
		// if current position > video duration (loop video)
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
		}
	}
	return 0;
}

// DirectShow init function (see DirectShow API)
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

// Process window messages
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

// Find our WorkerW window
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
