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

#include "ApplicationContent.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE /* hPrevInstance */,
    _In_ LPSTR /* lpCmdLine */,
    _In_ int /* nCmdShow */)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    UI::ThrowIfFailed(RoInitialize(RO_INIT_MULTITHREADED));
    {
        LisaApp::AppContent app(L"main", L"Lisa Application", LisaApp::HelperPath(L"Resources\\Icon\\app_logo.png"), 1480, 920);
        if (app.Initialize())
            app.Content(hInstance);
    }
    RoUninitialize();

    return 0;
}