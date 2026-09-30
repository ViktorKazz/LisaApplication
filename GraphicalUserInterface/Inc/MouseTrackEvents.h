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

#ifndef MOUSE_TRACK_EVENTS_CLASS_H
#define MOUSE_TRACK_EVENTS_CLASS_H

#include <windows.h>
#include "DPIScale_class.h"

// Below is the class which is used to manage mouse tracking events.

class MouseTrackEvents
{
public:

	MouseTrackEvents();

	bool OnButtonDown(HWND hwnd, RECT rc);
	bool OnButtonUp(HWND hwnd, RECT rc);
	bool OnMouseMove(this MouseTrackEvents& object, HWND hwnd, RECT rc, int pixelX, int pixelY);
	bool OnMouseMoveOut(this MouseTrackEvents& object, HWND hwnd);

	bool Reset(this MouseTrackEvents& object, HWND hwnd);

	RECT controlElementArea(
		HWND hwnd,
		unsigned int positionX,
		unsigned int positionY,
		unsigned int widthControl,
		unsigned int heightControl
	);

	bool ElementSelected(const HWND& hwnd, const RECT& rect, const LPARAM& lParam);

private:
	bool m_bMouseTracking;
};

#endif // !MOUSE_TRACK_EVENTS_CLASS_H