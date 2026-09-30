// -------------------------------------------------------------------------------------- 
// 
// LisaApplication. Creating 3D primitives and editing their attributes. 
// Copyright (C) 18.8.2024 - 30.9.2026 Deputatov Viktor Maxwellrender@yandex.ru 
// 
// This program is free software: you can redistribute it and/or modify 
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version. 
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU General Public License for more details. 
// 
// You should have received a copy of the GNU General Public License 
// along with this program. If not, see https://www.gnu.org/licenses/. 
// 
// Project blog https://lisaapplicationblog.blogspot.com/. 
// 
// --------------------------------------------------------------------------------------

#include "..//Inc/WndInitialize.h"

UI::WndInitialize::WndInitialize()
{
}

UI::WndInitialize::~WndInitialize()
{
}

// Creates the application window and initializes.
HWND UI::WndInitialize::Initialize(
	const WndInitialize& object,
	WNDPROC wndProc,
	HWND parentHwnd,
	BOOL showWindow
)
{
	HWND hwnd{};
	
	// Search by window class.
	// It can be useful to prevent different objects 
	// from creating multiple windows with the same purpose.
	HWND findhwnd = FindWindowW(object.m_windowClass.c_str(), NULL);
	
	if (findhwnd != NULL) 
	{
		SetForegroundWindow(findhwnd);
		return nullptr;
	}

	// Register the window class.
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = object.m_windowClassStyle;
	wcex.lpfnWndProc = wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = object.m_hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursorW(object.m_hInstance, IDC_ARROW);
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = object.m_windowClass.c_str();
	wcex.hIconSm = NULL;

	RegisterClassEx(&wcex);

	// Because the CreateWindow function takes its size in pixels, we
	// obtain the system DPI and use it to scale the window size.
	int dpiX{};
	int dpiY{};

	HDC hdc = GetDC(NULL);
	if (hdc)
	{
		dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
		dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
		ReleaseDC(NULL, hdc);
	}

	hwnd = CreateWindowExW(
		object.m_dwExStyle,
		object.m_windowClass.c_str(),
		object.m_windowTitle.c_str(),
		object.m_dwStyle,
		object.m_left,
		object.m_top,
		static_cast<UINT>(ceil(object.m_right * dpiX / 96.0f)),
		static_cast<UINT>(ceil(object.m_bottom * dpiY / 96.0f)),
		parentHwnd,
		NULL,
		object.m_hInstance,
		this
	);

	if (hwnd)
	{
		if (showWindow)
		{
			ShowWindow(hwnd, SW_SHOWNORMAL);
		}
		else
		{
			ShowWindow(hwnd, SW_HIDE);
		}

		// The SetForegroundWindow function puts the thread that created 
		// the specified window into foreground mode and activates the window.
		// Keyboard input is directed to the window and various visual cues 
		// for the user are changed.The system assigns a slightly higher priority 
		// to the thread that created the priority window than it does to other threads.
		//
		SetForegroundWindow(hwnd);
		UpdateWindow(hwnd);
	}
	else
	{
		MessageBox(NULL,
			L"Call to CreateWindowExW failed!",
			object.m_windowTitle.c_str(),
			NULL);

		return nullptr;
	}

	return hwnd;
}

std::vector<HWND> UI::WndInitialize::FindChild(HWND hwnd)
{
	HWND hwndChild{};
	std::vector<HWND> child{};

	do
	{
		hwndChild = FindWindowExA(hwnd, hwndChild, NULL, NULL);

		if (hwndChild)
		{
			child.push_back(hwndChild);
		}

	} while (hwndChild);

	return child;
}