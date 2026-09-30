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

#ifndef WINDOW_TRANSFORMATION_CLASS_H
#define WINDOW_TRANSFORMATION_CLASS_H

#include "FlagsUI.h"
#include <windows.h>

namespace UI
{
	class WindowTransformation
	{
	protected:
		WindowTransformation() = default;
		virtual ~WindowTransformation() = default;

		INT RestoreX(HWND hwnd, const RECT& rc, LONG inLeft, LONG nonClientAreaSize, ui_transform transform, LONG parentWndWidth);
		INT RestoreY(HWND hwnd, const RECT& rc, LONG inTop, LONG nonClientAreaSize, ui_transform transform, LONG parentWndHeight);
		INT StretchingX(HWND hwnd, const RECT& rc, LONG inRight, LONG nonClientAreaSize, ui_transform transform, LONG parentWndWidth);
		INT StretchingY(HWND hwnd, const RECT& rc, LONG inBottom, LONG nonClientAreaSize, ui_transform transform, LONG parentWndHeight);

		void RestorePosition(
			this WindowTransformation& object,
			const HWND& hwnd,
			const UINT& parentWndWidth,
			const UINT& parentWndHeight,
			ui_transform transform,
			LONG nonClientAreaSize,
			LONG inLeft,
			LONG inTop,
			LONG inRight,
			LONG inBottom,
			INT& left,
			INT& top,
			INT& right,
			INT& bottom
		);

	private:

	};
}



#endif // !WINDOW_TRANSFORMATION_CLASS_H
