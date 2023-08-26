#pragma once
#ifndef MAIN_H
#define MAIN_H

// Width and height of wallpapers
#define WIDTH 1920
#define HEIGHT 1080

// Path to your wallpapers
// File must be in .avi format
#define WP_PATH L"C:\\wallpapers\\example.avi"

// Function prototypes
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
HRESULT InitializeDirectShow(HWND hwnd);

// Global variables
static HWND workerw = NULL;
IGraphBuilder* pGraph = NULL;
IMediaControl* pControl = NULL;
IVideoWindow* pVidWin = NULL;
IMediaSeeking* pMediaSeeking = NULL;
#endif