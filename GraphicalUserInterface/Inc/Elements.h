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

#ifndef ELEMENTS_CLASS_H
#define ELEMENTS_CLASS_H

#include "Window.h"
#include "Button.h"
#include "Separator.h"
#include "Text.h"
#include "Image.h"
#include "Field.h"

namespace UI
{
    template<typename T>
    inline RECT CenterscreenRECT(const std::shared_ptr<Caching>& pCaching, const ElementConfig<T>& config)
    {
        //Returns the width and height of the screen(monitor)
        LONG xres = GetSystemMetrics(SM_CXSCREEN);
        LONG yres = GetSystemMetrics(SM_CYSCREEN);

        if (pCaching->ContainsCachingRECT(config.ClassName))
        {
            return pCaching->GetCachingRECT(config.ClassName);
        }
        else
        {
            const RECT rect{
                (xres / 2) - ((config.Rect.right + gNonClientAreaSizeMultTwo) / 2),
                (yres / 2) - ((config.Rect.bottom + gNonClientAreaSizeMultTwo) / 2),
                config.Rect.right + gNonClientAreaSizeMultTwo,
                config.Rect.bottom + gNonClientAreaSizeMultTwo
            };
            pCaching->SetCachingRECT(config.ClassName, rect);
            return rect;
        }
    }

    template<typename T>
    inline RECT CustompositionRECT(const std::shared_ptr<Caching>& pCaching, const ElementConfig<T>& config)
    {
        if (pCaching->ContainsCachingRECT(config.ClassName))
        {
            return pCaching->GetCachingRECT(config.ClassName);
        }
        else
        {
            const RECT rect{ 
                config.Rect.left, 
                config.Rect.top, 
                config.Rect.right + gNonClientAreaSizeMultTwo, 
                config.Rect.bottom + gNonClientAreaSizeMultTwo 
            };
            pCaching->SetCachingRECT(config.ClassName, rect);
            return rect;
        }
    }

    template<typename T>
    inline RECT MrmcRECT(const std::shared_ptr<Caching>& pCaching, const ElementConfig<T>& config)
    {
        if (pCaching->ContainsCachingRECT(config.ClassName))
        {
            return pCaching->GetCachingRECT(config.ClassName);
        }
        else
        {
            RECT rc; GetClientRect(config.Parent, &rc);

            LONG parentWindowWidth{ static_cast<LONG>(rc.right) };

            INT nButton{};// close, restore, minimize -> 3

            if (config.Flags.Modes & ui_modes::mrmc1)
                nButton = 1;
            if (config.Flags.Modes & ui_modes::mrmc2)
                nButton = 2;
            if (config.Flags.Modes & ui_modes::mrmc3)
                nButton = 3;

            const RECT rect{ 
                parentWindowWidth - ((gButtonWidth * nButton) + gAdditionalMRMCLeft),
                gAdditionalMRMCTop,
                gButtonWidth * nButton,
                gButtonHeight 
            };
            pCaching->SetCachingRECT(config.ClassName, rect);
            return rect;
        }
    }

    template<typename T>
    inline RECT BottombarSimpleRECT(const ElementConfig<T>& config)
    {
        RECT rc; GetClientRect(config.Parent, &rc);

        LONG parentWindowWidth{ static_cast<LONG>(rc.right) };
        LONG parentWindowHeight{ static_cast<LONG>(rc.bottom) };

        const RECT rect{
            gAdditionalBottombarSimpleLeft,
            parentWindowHeight - gAdditionalBottombarSimpleTop,
            parentWindowWidth - gAdditionalBottombarSimpleRight,
            gButtonHeight 
        };
        return rect;
    }

    template<typename T>
    inline RECT BottombarInbuiltRECT(const ElementConfig<T>& config)
    {
        RECT rc; GetClientRect(config.Parent, &rc);

        LONG parentWindowWidth{ static_cast<LONG>(rc.right) };
        LONG parentWindowHeight{ static_cast<LONG>(rc.bottom) };

        const RECT rect{ 0, parentWindowHeight - gButtonHeight, parentWindowWidth, gButtonHeight };
        return rect;
    }

    template<typename T>
    inline RECT TopbarRECT(const std::shared_ptr<Caching>& pCaching, const ElementConfig<T>& config)
    {
        if (pCaching->ContainsCachingRECT(config.ClassName))
        {
            return pCaching->GetCachingRECT(config.ClassName);
        }
        else
        {
            pCaching->SetCachingRECT(config.ClassName, gCachingTopbarRect);
            return gCachingTopbarRect;
        }
    }

    template<typename T>
    inline RECT CustompositionInRECT(const ElementConfig<T>& config)
    {
        RECT rc; GetClientRect(config.Parent, &rc);

        if (config.Rect.left == 0 && config.Rect.top == 0 && config.Rect.right == 0 && config.Rect.bottom == 0)
            return rc;
        else
            return config.Rect;
    }

    // Resize the window along the X axis according to the sum of the widths of all elements.  
    template<typename T>
    inline void ModifiableNoframe(
        const std::shared_ptr<D11DeviceResources>& pD11Device, 
        const std::shared_ptr<Caching>& pCaching, 
        const ElementConfig<T>& config,
        RECT& rect
    )
    {
        ui_draw::button typeBtn{};
        if (std::holds_alternative<ui_draw::button>(config.Flags.Draw))
            typeBtn = std::get<ui_draw::button>(config.Flags.Draw);

        if (typeBtn & ui_draw::button::noframe)
        {
            if (config.Flags.Transform & ui_transform::modifiable_x)
            {
                HelperWTools tools;
                RECT rc; GetClientRect(config.Parent, &rc);

                // Change the width of the element rectangle to match the length of the name.
                LONG mod = tools.FindTextLength<LONG>(pD11Device->GetIDWriteFactory(), pCaching->GetButtonTextFormat(), config.TitleName.c_str());
                
                rect.left = rc.right; rect.right = mod;
                LONG lng{ rc.right + mod };

                SendMessageW(config.Parent, WM_COMMAND, GET_MODIFIABLE_X, (LONG(lng)));
            }
        }
    }

    template<typename T>
    class Element
    {
    public:
        Element() = default;
        ~Element() {};

        template<typename Extra>
        static HWND __cdecl Create(
            const std::shared_ptr<D11DeviceResources>& pD11Device,
            const std::shared_ptr<Caching>& pCaching,
            const ElementConfig<Extra>& config
        ) 
        {
            DWORD dwExStyle{ WS_EX_NOREDIRECTIONBITMAP | WS_EX_APPWINDOW };
            DWORD dwStyle{ WS_POPUP | WS_CLIPCHILDREN };

            RECT rect{};

            switch (config.Flags.Type)
            {
            case ui_type::simple:
            {
                if (config.Flags.Modes & ui_modes::centerscreen)
                    rect = CenterscreenRECT(pCaching, config);                   
                
                else if (config.Flags.Modes & ui_modes::customposition)
                    rect = CustompositionRECT(pCaching, config);
            }
            break;
            case ui_type::inbuilt:
            {
                if (config.Flags.Modes & ui_modes::mrmc1 || config.Flags.Modes & ui_modes::mrmc2 || config.Flags.Modes & ui_modes::mrmc3)
                    rect = MrmcRECT(pCaching, config);
                
                if (config.Flags.Modes & ui_modes::bottombar_simple)
                    rect = BottombarSimpleRECT(config);
                
                if (config.Flags.Modes & ui_modes::bottombar_inbuilt)
                    rect = BottombarInbuiltRECT(config);
                
                if (config.Flags.Modes & ui_modes::topbar)
                    rect = TopbarRECT(pCaching, config);

                if (config.Flags.Modes & ui_modes::customposition)
                    rect = CustompositionInRECT(config);
                
                dwExStyle = { NULL };
                dwStyle = { WS_VISIBLE | WS_CHILD };
            }
            break;
            case ui_type::popUp:
            {
                rect = config.Rect;
            }
            break;
            case ui_type::separator:
            {
                rect = config.Rect;

                dwExStyle = { NULL };
                dwStyle = { WS_VISIBLE | WS_CHILD };
            }
            break;
            default:
                break;
            }

            // Resize the window along the X axis according to the sum of the widths of all elements.           
            if (std::is_same_v<T, Button>)
                ModifiableNoframe(pD11Device, pCaching, config, rect);


            HelperWTools tools;
            std::wstring wClass{ tools.CreateClass(config.Flags.Type, config.ClassName) };

            // Using WS_CLIPCHILDREN.
            // Excludes the area occupied by child windows when drawing occurs within the parent window. 
            // This style is used when creating the parent window.

            UINT wndClassStyle{};
            if (config.Flags.Modes & ui_modes::dblclks)
                wndClassStyle = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
            else
                wndClassStyle = CS_HREDRAW | CS_VREDRAW;

            std::unique_ptr<T> element = std::make_unique<T>(
                rect.left, rect.top, rect.right, rect.bottom, config.hInstance, wClass, config.TitleName, wndClassStyle, dwExStyle, dwStyle);

            // Initialising 2D/3D resources for window.
            element->GetComposition()->SetDeviceResources(pD11Device);
            element->SetD11Device(pD11Device);
            element->SetCaching(pCaching);
            element->GetConfigure(config);

            HWND hwnd = element->Initialize(
                *element, element->WindowProcL<T>, config.Parent, (config.Flags.Modes & ui_modes::hide) ? false : true);

            if (!hwnd)
                return nullptr;

            // Release the unique pointer from resource ownership.
            // Now the owner of the resource will be the raw pointer located in the Windows Procedure.
            // This way, the resource will be preserved for the entire lifetime of the window 
            // without storing it, for example, in a separate container.
            element.release();

            return hwnd;
        }

    private:

    };

}

#endif // !ELEMENTS_CLASS_H