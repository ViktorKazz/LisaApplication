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

#ifndef MESSAGE_ONLY_WINDOW_CLASS_H
#define MESSAGE_ONLY_WINDOW_CLASS_H

#include "WndInitialize.h"
#include <FlagsUI.h>

namespace LisaApp
{
    class MessageOnlyWindow : public UI::WndInitialize
    {
    public:
        using WndInitialize::WndInitialize;
        ~MessageOnlyWindow() = default;

        // Accessors.


        // Helper function for processing incoming data.
        template<typename T>
        T GetFieldData(const T previousData, const UI::GetFieldData& data, UINT belongingToAGroup, UINT purpose)
        {
            T t{};
            //extra.FieldData.BelongingToAGroup, extra.FieldData.Purpose
            if (belongingToAGroup == data.BelongingToAGroup && purpose == data.Purpose)
            {
                if constexpr (std::is_same_v<T, UINT>)
                {
                    t = std::stoul(data.Data);
                }
                else if constexpr (std::is_same_v<T, float>)
                {
                    t = std::stof(data.Data);
                }
                else if constexpr (std::is_same_v<T, std::wstring>)
                {
                    t = data.Data;
                }
            }
            else
            {
                t = previousData;
            }

            return t;
        };

        //LRESULT CALLBACK MessageHandled(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override { return 0; };

        // Creating a simple window.
        static std::unique_ptr<LisaApp::MessageOnlyWindow> __cdecl MessageWindow(HINSTANCE hInstance,
            const std::variant<std::wstring, std::pair<std::wstring, std::wstring>>& varName, UI::ui_type type);


        static LRESULT CALLBACK MessageOnlyWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    private:

    };
}

#endif // !MESSAGE_ONLY_WINDOW_CLASS_H