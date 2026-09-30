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

#ifndef POP_UP_WINDOW
#define POP_UP_WINDOW

#include "LisaGui.h"
#include "KeysBindingMap.h"

namespace LisaApp
{
    class PopUpWindow
    {
    public:
        PopUpWindow() = default;
        ~PopUpWindow() = default;

        void SetPerspectiveHwnd(HWND hwnd)
        {
            m_perspectiveHwnd = hwnd;
        }

        // Accessors.

        //auto GetCallWnd() const noexcept { return m_callWnd; };
        
        // More and More2 for test
        
        HWND More(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
        HWND More2(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
        HWND HelpFeedback(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
        HWND SelectAllType(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
        HWND SelectAllComponents(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
        HWND CreatePolyPrimitives(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
        HWND CreateLights(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
        HWND CreateCamera(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
        HWND MainTopBarFile(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
        HWND MainTopBarEdit(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
        HWND MainTopBarCreate(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
        HWND MainTopBarSelect(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
        HWND MainTopBarHelp(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);

        LisaGui::CallableFunctions<wnd_keys, UI::CallWindow> m_callWnd{};
        LisaGui::CallableFunctions<cmd_keys, UI::CallCommand> m_callCmd{};

    private:
        HWND m_perspectiveHwnd{ nullptr };  
        
    };
}

#endif // !POP_UP_WINDOW