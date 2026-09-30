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

#ifndef LISA_GUI_PCH_H
#define LISA_GUI_PCH_H

// These "#pragma comment" lines only work in Visual Studio; 
// otherwise include these libraries with the linker.
//
#pragma comment(lib, "runtimeobject.lib")  // For using with roapi.h
#pragma comment(lib, "dwrite.lib")         // For using with dwrite_3.h
#pragma comment(lib, "D3D11")              // For using with d3d11_4.h

#pragma comment(lib, "d3d12.lib")         
//#pragma comment(lib,"d3dcompiler.lib")

#pragma comment(lib, "D2d1")               // For using with d2d1.h
#pragma comment(lib, "d2d1") 
#pragma comment(lib, "dcomp")              // For using with dcomp.h
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")

//#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxguid.lib")


// Windows Header Files:
#include <windows.h>
#include <windowsx.h>

// RoInitialize
#include <roapi.h> 

// C RunTime Header Files:
#include <stdlib.h>
#include <vector>
#include <memory.h>
#include <wchar.h>
#include <tchar.h>
#include <math.h>
#include <memory>
#include <wincodec.h>
#include <shlobj.h>
#include <strsafe.h>
#include <wrl.h> 
#include <unordered_map>
#include <algorithm>
#include <variant>
#include <array>
#include <functional>
#include <numeric>
#include <expected>
#include <ranges>
#include <any>

// Read and write file:
#include <iostream>
#include <fstream>

// Text
#include <dwrite_3.h>

//Diretx 3D11
#include <d3d11_4.h>
#include <dxgi1_5.h>

//Diretx 2D
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_3.h>
#include <d2d1effects.h>
#include <dcomp.h>

#include "ErrorHandling.h"
#include "FlagsUI.h"
#include "GlobalValue.h"
#include "MouseTrackEvents.h"
#include "HelperWindowTools.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE

// This function frees the ppT pointer and assigns it the value NULL.
//
// Another option is to use a smart pointer class such as ComPtr, 
// which is defined in the Windows Runtime C++ Template Library (WRL).

template <class T>
void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = nullptr;
    }
}
#endif

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#endif // !LISA_GUI_PCH_H
