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

#include "..//Inc/LisaGui.h"

void LisaGui::CreateLabelText(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    const std::shared_ptr<UI::Caching>& pCaching,
    const std::vector<UI::FontConfig>& fontConfig,
    HWND parent,
    HINSTANCE hInstance,
    const std::wstring& className,
    const RECT& rect
)
{
    for (auto&& [indx, config] : fontConfig | std::views::enumerate)
    {
        const UI::ElementConfig<UI::TextConfig> configure{
            .hInstance = hInstance,
            .ClassName = className,
            .Rect = rect,
            .Parent = parent,
            .Flags = {.Type = UI::ui_type::inbuilt, .Modes = UI::ui_modes::customposition, .Draw = UI::ui_draw::text::label },
            .Extra = {.ImgTxt = config}
        };
        UI::Element<UI::Text>::Create(pDevice, pCaching, configure);
    }
}

void LisaGui::CreateSimpleField(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    const std::shared_ptr<UI::Caching>& pCaching,
    const std::vector<UI::FieldConfig>& fieldConfig,
    HWND parent,
    HINSTANCE hInstance,
    const std::wstring& className,
    const RECT& rect
)
{
    for (auto&& [indx, config] : fieldConfig | std::views::enumerate)
    {
        const UI::ElementConfig<UI::FieldConfig> configure{
                .hInstance = hInstance,
                .ClassName = className + std::to_wstring(indx),
                .Rect = rect,
                .Parent = parent,
                .Flags = {.Type = UI::ui_type::inbuilt, .Modes = UI::ui_modes::customposition, .Draw = UI::ui_draw::field::simple },
                .Extra = config
        };
        UI::Element<UI::Field>::Create(pDevice, pCaching, configure);
    }
}

void LisaGui::CreateTopLeftButtonBar(
    _In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    _In_ const std::shared_ptr<UI::Caching>& pCaching,
    _In_ HWND parent,
    _In_opt_ HINSTANCE hInstance,
    _In_ const std::wstring& className,
    _In_ const std::initializer_list<std::tuple<std::wstring, UI::CallWindow>>& noFrameButtons
)
{
    using namespace UI;

    const ElementConfig<WindowConfig> windowPlaneConfig{
        .hInstance = hInstance,
        .ClassName = className,
        .Parent = parent,
        .Flags = {
            .Type = ui_type::inbuilt, 
            .Modes = ui_modes::topbar, 
            .Draw = ui_draw::window::inbuilt | ui_draw::window::inbuilt_in,
            .Transform = ui_transform::restore_lx | ui_transform::restore_ty | ui_transform::modifiable_x
    }
    };
    HWND windowPlane = UI::Element<UI::Window>::Create(pDevice, pCaching, windowPlaneConfig);

    for (const auto& b : noFrameButtons)
    {
        const auto& [buttonName, callWindow] { b };

        const ElementConfig<ButtonConfig> topButtonFileConfig{
            .hInstance = hInstance,
            .ClassName = className + L"_" + buttonName,
            .TitleName = buttonName,
            .Parent = windowPlane,
            .Flags = {
                .Type = ui_type::inbuilt,
                .Modes = ui_modes::customposition,
                .Draw = ui_draw::button::noframe,
                .Transform = ui_transform::modifiable_x
        },
            .Extra = {.Window = callWindow}
        };
        UI::Element<UI::Button>::Create(pDevice, pCaching, topButtonFileConfig);
    }
}

void LisaGui::CreateTopRightButtonBar(
    _In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    _In_ const std::shared_ptr<UI::Caching>& pCaching,
    _In_ HWND parent,
    _In_opt_ HINSTANCE hInstance,
    _In_ const std::wstring& className,
    _In_ UI::ui_draw::button button
)
{
    using namespace UI;

    const ElementConfig<WindowConfig> windowPlaneConfig{
        .hInstance = hInstance,
        .ClassName = className,
        .Parent = parent,
        .Flags = {
            .Type = ui_type::inbuilt, 
            .Modes = ui_modes::mrmc3, 
            .Draw = ui_draw::window::inbuilt | ui_draw::window::inbuilt_in,
            .Transform = ui_transform::restore_rx | ui_transform::restore_ty 
    },
    };
    HWND windowPlane = Element<UI::Window>::Create(pDevice, pCaching, windowPlaneConfig);


    // Create buttons to minimize, maximize/restore, and close.

    if (button & ui_draw::button::minimize)
    {
        const ElementConfig<ButtonConfig> minimizeButtonConfig{
            .hInstance = hInstance,
            .ClassName = className + L"_minimize",
            .Rect = { 0, 0, UI::gButtonWidth, UI::gButtonHeight },
            .Parent = windowPlane,
            .Flags = {
                .Type = ui_type::inbuilt, 
                .Modes = ui_modes::customposition, 
                .Draw = ui_draw::button::minimize,
                .Command = ui_command::minimize | ui_command::up
        }
        };
        UI::Element<UI::Button>::Create(pDevice, pCaching, minimizeButtonConfig);
    }

    if (button & ui_draw::button::restore)
    {
        const ElementConfig<ButtonConfig> restoreButtonConfig{
            .hInstance = hInstance,
            .ClassName = className + L"_restore",
            .Rect = { UI::gButtonWidth, 0, UI::gButtonWidth, UI::gButtonHeight },
            .Parent = windowPlane,
            .Flags = {
                .Type = ui_type::inbuilt, 
                .Modes = ui_modes::customposition, 
                .Draw = ui_draw::button::restore,
                .Command = ui_command::restore | ui_command::up
        }
        };
        UI::Element<UI::Button>::Create(pDevice, pCaching, restoreButtonConfig);
    }

    if (button & ui_draw::button::close)
    {
        const ElementConfig<ButtonConfig> closeButtonConfig{
            .hInstance = hInstance,
            .ClassName = className + L"_close",
            .Rect = { UI::gButtonWidth * 2, 0, UI::gButtonWidth, UI::gButtonHeight },
            .Parent = windowPlane,
            .Flags = {
                .Type = ui_type::inbuilt, 
                .Modes = ui_modes::customposition, 
                .Draw = ui_draw::button::close,
                .Command = ui_command::close | ui_command::up
        }
        };
        UI::Element<UI::Button>::Create(pDevice, pCaching, closeButtonConfig);
    }
    
}

HWND LisaGui::CreateLeftButtonBar(
    _In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    _In_ const std::shared_ptr<UI::Caching>& pCaching,
    _In_ HWND parent,
    _In_opt_ HINSTANCE hInstance,
    _In_ const std::wstring& className,
    _In_ const RECT& rect
)
{
    using namespace UI;

    const ElementConfig<WindowConfig> leftBarConfig{
        .hInstance = hInstance,
        .ClassName = className,
        .Rect = rect,
        .Parent = parent,
        .Flags = {
            .Type = ui_type::inbuilt,
            .Modes = ui_modes::customposition,
            .Draw = ui_draw::window::inbuilt | ui_draw::window::inbuilt_border_r | ui_draw::window::inbuilt_border_b,
            .Transform = ui_transform::stretching_y
    },
    };
    return Element<UI::Window>::Create(pDevice, pCaching, leftBarConfig);
}

void LisaGui::CreateBottomButtonBar(
    _In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    _In_ const std::shared_ptr<UI::Caching>& pCaching,
    _In_ HWND parent,
    _In_opt_ HINSTANCE hInstance,
    _In_ const std::wstring& className,
    _In_ const std::initializer_list<std::tuple<std::wstring, UI::ui_command, UI::CallCommand>>& buttons
)
{
    using namespace UI;

    const ElementConfig<WindowConfig> bottomPlaneConfig{
        .hInstance = hInstance,
        .ClassName = className + L"_" + L"Bottom",
        .Parent = parent,
        .Flags = {
            .Type = ui_type::inbuilt,
            .Modes = ui_modes::bottombar_inbuilt,
            .Draw = ui_draw::window::inbuilt,
            .Transform = ui_transform::restore_by | ui_transform::stretching_x | ui_transform::split_x
    },
    };
    HWND bottomPlane = Element<UI::Window>::Create(pDevice, pCaching, bottomPlaneConfig);

    LONG nButton{ static_cast<LONG>(buttons.size()) };
    RECT rc; Error(GetClientRect(bottomPlane, &rc));
    LONG right{ rc.right / nButton };

    for (auto&& [indx, config] : buttons | std::views::enumerate)
    {
        const auto& [title, cmd, callCommand] { config };

        rc.left = (rc.right / nButton) * static_cast<LONG>(indx);
        const ElementConfig<ButtonConfig> buttonConfig{
            .hInstance = hInstance,
            .ClassName = className + L"_" + std::to_wstring(indx),
            .TitleName = title,
            .Rect = { rc.left, rc.top, right, rc.bottom },
            .Parent = bottomPlane,
            .Flags = {
                .Type = ui_type::inbuilt,
                .Modes = ui_modes::customposition,
                .Draw = ui_draw::button::frame,
                .Transform = ui_transform::stretching_x,
                .Command = cmd
        },
            .Extra = {.Command = callCommand}
        };
        Element<UI::Button>::Create(pDevice, pCaching, buttonConfig);
    }
}

HWND LisaGui::CreateFullResizablePlane(
    _In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    _In_ const std::shared_ptr<UI::Caching>& pCaching,
    _In_ HWND parent,
    _In_opt_ HINSTANCE hInstance,
    _In_ const std::wstring& className,
    _In_ LONG width,
    _In_ LONG height
)
{
    using namespace UI;

    const ElementConfig<WindowConfig> planeConfig{
            .hInstance = hInstance,
            .ClassName = className,
            .Rect = {
                gNonClientAreaSize + 4,
                gNonClientAreaSize + gMainMenuBarHeight,
                width - (4 * 2),
                height - (gMainMenuBarHeight + 4)
        },
            .Parent = parent,
            .Flags = {
                .Type = ui_type::inbuilt,
                .Modes = ui_modes::customposition,
                .Draw = ui_draw::window::inbuilt,
                .Transform = ui_transform::restore_lx | ui_transform::restore_ty | ui_transform::stretching_x | ui_transform::stretching_y
        },
    };

    return Element<UI::Window>::Create(pDevice, pCaching, planeConfig);
}

HWND LisaGui::CreateSimpleWindow(
    _In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    _In_ const std::shared_ptr<UI::Caching>& pCaching,
    _In_opt_ HWND parent,
    _In_opt_ HINSTANCE hInstance,
    _In_ const std::wstring& className,
    _In_ const std::wstring& titleName,
    _In_ const std::wstring& icon,
    _In_ LONG width,
    _In_ LONG height,
    _In_ UI::ui_draw::button topRightButton,
    _In_ UI::ui_draw::window windowDraw,
    _In_ const std::initializer_list<std::tuple<std::wstring, UI::CallWindow>>& noFrameButtons,
    _In_opt_ bool showTitle,
    _In_opt_ bool resizable
)
{
    using namespace UI;

    const ElementConfig<WindowConfig> windowConfig{
        .hInstance = hInstance,
        .ClassName = className,
        .TitleName = titleName,
        .Rect = { 0, 0, width, height },
        .Parent = parent,
        .Flags = {.Type = ui_type::simple, .Modes = ui_modes::centerscreen, .Draw = windowDraw },
        .Extra =
        {
            .ImgTxt = icon,
            .ShowTitle = showTitle,
            .Resizable = resizable
        }
    };
    HWND window = Element<UI::Window>::Create(pDevice, pCaching, windowConfig);

    // If the window did not exist before, 
    // it will be created and therefore we create elements for it.
    // Otherwise, there is no need for elements.
    if (window)
    {
        if (topRightButton != ui_draw::button::none)
            CreateTopRightButtonBar(pDevice, pCaching, window, hInstance, className + L"_" + L"mrmc", topRightButton);

        if (noFrameButtons.size())
            CreateTopLeftButtonBar(pDevice, pCaching, window, hInstance, className + L"_" + L"topBar", noFrameButtons);
    }

    return window;
}

void LisaGui::CreatePopUpElement(
    _In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    _In_ const std::shared_ptr<UI::Caching>& pCaching,
    _In_ const CallFunc& callFunc,
    _In_opt_ HINSTANCE hInstance,
    _In_ const std::wstring& className,
    _In_ const std::wstring& titleName,
    _In_ const std::wstring& middleName,
    _In_ const std::variant<UI::ui_draw::button, UI::ui_draw::separator>& draw,
    _In_ HWND parent,
    _In_ HWND root,
    _In_ LONG windowWidth,
    _Inout_ LONG& addTopValue
)
{
    using namespace UI;

    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!parent)
        throw std::runtime_error("Parent window not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (className.empty() || draw.index() == std::variant_npos)
        throw std::runtime_error("ClassName or ui_draw not specified.");


    constexpr LONG indentLeft{ 4 };
    constexpr LONG indentTop{ 4 };
    constexpr LONG doubleIndent = (UI::gNonClientAreaSize + ((indentLeft * 2) - 1));

    ui_draw::button typeBtn{};
    ui_draw::separator typeSep{};

    if (std::holds_alternative<ui_draw::button>(draw))
        typeBtn = std::get<ui_draw::button>(draw);
    else if (std::holds_alternative<ui_draw::separator>(draw))
        typeSep = std::get<ui_draw::separator>(draw);

    LONG value{ (typeSep & ui_draw::separator::popUp) ? UI::gSeparatorPopUpHeight : UI::gPopUpButtonHeight };

    if (typeSep & ui_draw::separator::popUp)
    {
        const ElementConfig<DrawnSeparatorConfig> separatorConfig{
            .hInstance = hInstance,
            .ClassName = className,
            .TitleName = titleName,
            .Rect = {indentLeft, indentTop + addTopValue, windowWidth - doubleIndent, value},
            .Parent = parent,
            .Flags = {.Type = ui_type::inbuilt, .Modes = ui_modes::customposition, .Draw = typeSep}
        };
        UI::Element<DrawnSeparator>::Create(pDevice, pCaching, separatorConfig);
    }
    else if (typeBtn & ui_draw::button::pup_simple || typeBtn & ui_draw::button::pup_dual || typeBtn & ui_draw::button::pup_transition)
    {
        const ElementConfig<ButtonConfig> buttonConfig{
            .hInstance = hInstance,
            .ClassName = className,
            .TitleName = titleName,
            .Rect = {indentLeft, indentTop + addTopValue, windowWidth - doubleIndent, value},
            .Parent = parent,
            .Flags = {.Type = ui_type::inbuilt, .Modes = ui_modes::customposition, .Draw = typeBtn},
            .Extra = {.Root = root, .MiddleName = middleName, .Window = ReadFuncVar<UI::CallWindow>(callFunc), .Command = ReadFuncVar<UI::CallCommand>(callFunc)}
        };
        UI::Element<Button>::Create(pDevice, pCaching, buttonConfig);
    }

    addTopValue += value;
}

LONG LisaGui::GetWindowMaxWidth(
    _In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    _In_ const std::shared_ptr<UI::Caching>& pCaching,
    _In_ const std::vector<PopUpConfig>& buttonElements
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (buttonElements.empty())
        throw std::runtime_error("ButtonElements not specified.");

    HelperWTools tools;
    const auto& writeFactory = pDevice->GetIDWriteFactory();
    const auto& textFormat = pCaching->GetButtonTextFormat();

    auto measureText = [&](const std::wstring& text)
        {
            return !text.empty() ? tools.FindTextLength<LONG>(writeFactory, textFormat, text.c_str()) : 0L;
        };

    constexpr LONG totalWidth = UI::gIconPlace + UI::gEmptyPlace + UI::gImagePlace;
    LONG widths{};

    for (const auto& [title, middleName, draw, callWnd] : buttonElements)
    {
        LONG textPlace{}, middlePlace{};
        if (!title.empty())
            textPlace = { measureText(title) };
        if (!middleName.empty())
            middlePlace = { measureText(title) };

        const LONG sum = totalWidth + textPlace + middlePlace;

        if (widths < sum)
            widths = sum;
    }

    return widths;
}

LONG LisaGui::CalculateWindowHeight(
    _In_ const std::vector<PopUpConfig>& buttonElements)
{
    if (buttonElements.empty())
        throw std::runtime_error("ButtonElements not specified.");

    // Lambda for calculating the height of an element.
    const auto getElementHeight = [](const std::variant<UI::ui_draw::button, UI::ui_draw::separator>& draw) -> LONG
        {
            using namespace UI;

            ui_draw::button typeBtn{};
            ui_draw::separator typeSep{};

            if (std::holds_alternative<ui_draw::button>(draw))
                typeBtn = std::get<ui_draw::button>(draw);
            else if (std::holds_alternative<ui_draw::separator>(draw))
                typeSep = std::get<ui_draw::separator>(draw);

            return (typeSep & ui_draw::separator::popUp) ? gSeparatorPopUpHeight : gPopUpButtonHeight;
        };

    // Accumulation of total height.
    //const LONG elementsHeight = std::accumulate(buttonElements.cbegin(), buttonElements.cend(), 0L,
    //    [&](LONG total, const auto& element) 
    //    {
    //        const auto& [title, name, drawType] = element; // Structured binding
    //        return total + getElementHeight(drawType);
    //    }
    //);

    LONG elementsHeight{};
    for (const auto& [title, middleName, draw, callWnd] : buttonElements)
    {
        elementsHeight += getElementHeight(draw);
    }

    LONG additionalPadding{ 7 };

    return elementsHeight + UI::gNonClientAreaSize + additionalPadding;
}

HWND LisaGui::CreatePopUpWindow(
    _In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    _In_ const std::shared_ptr<UI::Caching>& pCaching,
    _In_ const HWND& root,
    _In_opt_ const HINSTANCE& hInstance,
    _In_ const std::wstring& className,
    _In_ const std::vector<PopUpConfig>& buttonElements
)
{
    using namespace UI;

    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    if (className.empty())
        throw std::runtime_error("ClassName not specified.");

    if (buttonElements.empty())
        throw std::runtime_error("ButtonElements not specified.");

    const LONG windowWidth{ GetWindowMaxWidth(pDevice, pCaching, buttonElements) };
    const LONG windowHeight{ CalculateWindowHeight(buttonElements) };

    if (!windowWidth || !windowHeight)
        throw std::runtime_error("The value must be greater than zero.");

    const ElementConfig<WindowConfig> windowConfig{
        .hInstance = hInstance,
        .ClassName = className,
        .Rect = { 0, 0, windowWidth, windowHeight },
        .Parent = root,
        .Flags = {.Type = ui_type::popUp, .Modes = ui_modes::pup_simple | ui_modes::hide, .Draw = ui_draw::window::popUp },
        .Extra = {.Root = root, .ShowTitle = false, .Resizable = false}
    };
    HWND windowPopUp = Element<UI::Window>::Create(pDevice, pCaching, windowConfig);

    LONG addTopValue{};

    // If the window did not exist before, 
    // it will be created and therefore we create elements for it.
    // Otherwise, there is no need for elements.
    if (windowPopUp)
    {
        for (auto&& [indx, sepConfig] : buttonElements | std::views::enumerate)
        {
            const auto& [title, middleName, draw, callWnd] { sepConfig };

            CreatePopUpElement(
                pDevice,
                pCaching,
                callWnd,
                hInstance,
                className + std::to_wstring(indx),
                title,
                middleName,
                draw,
                windowPopUp,
                root,
                windowWidth,
                addTopValue
            );
        }
    }

    return windowPopUp;
}

std::vector<HWND> LisaGui::CreateWindowSeparators(
    _In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    _In_ const std::shared_ptr<UI::Caching>& pCaching,
    _In_ HWND parent,
    _In_opt_ HINSTANCE hInstance,
    _In_ const std::wstring& className,
    _In_opt_ const RECT& rect,
    _In_ const std::vector<UI::SeparatorConfig>& separators
)
{
    using namespace std::ranges;
    using namespace UI;

    if (!pDevice)
        throw std::runtime_error("DirectX 11 resources are missing");

    if (empty(className))
        throw std::runtime_error("Class name not specified");

    if (!parent)
        throw std::runtime_error("Parent window not specified");

    if (IsRectEmpty(&rect))
        throw std::runtime_error("Check the coordinates of the rectangle");
    // Returns TRUE (not zero!) if:
    // rect.right <= rect.left OR rect.bottom <= rect.top

    // Check for empty configuration.
    if (empty(separators))
        throw std::runtime_error("Empty separator configuration");

    // Checking for conflicting elements.
    if (any_of(separators, [](const auto& c) { return c.SplitX && c.SplitY; }))
        throw std::runtime_error("Ambiguous split configuration in elements");

    // Checking for separators using ranges.
    const bool has_x = any_of(separators, [](const auto& c) { return c.SplitX; });
    const bool has_y = any_of(separators, [](const auto& c) { return c.SplitY; });

    // Checking Axis Conflict.
    if (has_x && has_y)
        throw std::runtime_error("Cannot have separators on both X and Y axes");


    std::vector<HWND> substrate;

    const ElementConfig<WindowConfig> mainPlaneConfig{
        .hInstance = hInstance,
        .ClassName = className,
        .Rect = rect,
        .Parent = parent,
        .Flags = {
            .Type = ui_type::inbuilt,
            .Modes = ui_modes::customposition,
            .Draw = ui_draw::window::inbuilt,
            .Transform = /*ui_transform::restore_lx | ui_transform::restore_ty |*/ ui_transform::stretching_x | ui_transform::stretching_y
    },
    };
    HWND mainPlane = Element<UI::Window>::Create(pDevice, pCaching, mainPlaneConfig);

    RECT rc{}; Error(GetClientRect(mainPlane, &rc));

    INT indent{};
    for (auto&& [indx, sepConfig] : separators | std::views::enumerate)
    {
        const auto currentClass = [&](const std::wstring_view& suffix)
            {
                return std::format(L"{}_{}_{}", className, suffix, indx);
            };

        // Create the previous element.

        const ElementConfig<WindowConfig> previousConfig{
            .hInstance = hInstance,
            .ClassName = currentClass(L"substrate"),
            .Rect =
            {
                (indx == 0) ? 0 : (sepConfig.SplitX ? indent : 0),
                (indx == 0) ? 0 : (sepConfig.SplitX ? 0 : indent),
                sepConfig.SplitX ? sepConfig.Indent - indent : rc.right,
                sepConfig.SplitX ? rc.bottom : sepConfig.Indent - indent,
            },
            .Parent = mainPlane,
            .Flags = {
                .Type = ui_type::inbuilt,
                .Modes = ui_modes::customposition,
                .Draw = ui_draw::window::inbuilt,
                .Transform = ui_transform::restore_lx | ui_transform::restore_ty | ui_transform::stretching_x | ui_transform::stretching_y
        }
        };
        HWND previous = Element<UI::Window>::Create(pDevice, pCaching, previousConfig);
        substrate.push_back(previous);

        // Update the indent
        indent = sepConfig.Indent + UI::gSeparatorThickness;

        // Create a separator.
        const ElementConfig<SeparatorConfig> separatorConfig{
            .hInstance = hInstance,
            .ClassName = currentClass(L""),
            .Rect =
            {
                sepConfig.SplitX ? sepConfig.Indent : 0,
                sepConfig.SplitY ? sepConfig.Indent : 0,
                sepConfig.SplitX ? UI::gSeparatorThickness : rc.right,
                sepConfig.SplitX ? rc.bottom : UI::gSeparatorThickness
        },
            .Parent = mainPlane,
            .Flags = {
                .Type = ui_type::separator,
                .Modes = ui_modes::customposition,
                .Draw = ui_draw::separator::window,
                .Transform = sepConfig.Transform
        },
        .Extra = sepConfig
        };
        Element<UI::Separator>::Create(pDevice, pCaching, separatorConfig);

        // Processing the last element.
        if (static_cast<unsigned>(indx) == separators.size() - 1)
        {
            const ElementConfig<WindowConfig> lastConfig{
                .hInstance = hInstance,
                .ClassName = className + L"_substrate_" + std::to_wstring(indx + 1),
                .Rect =
                {
                    sepConfig.SplitX ? indent : 0,
                    sepConfig.SplitX ? 0 : indent,
                    sepConfig.SplitX ? rc.right - indent : rc.right,
                    sepConfig.SplitX ? rc.bottom : rc.bottom - indent,
            },
            .Parent = mainPlane,
            .Flags = {
                    .Type = ui_type::inbuilt,
                    .Modes = ui_modes::customposition,
                    .Draw = ui_draw::window::inbuilt 
            }
            };
            HWND last = Element<UI::Window>::Create(pDevice, pCaching, lastConfig);
            substrate.push_back(last);
        }
    }

    // So that the children's windows have the ability to change 
    // size according to the change in the size of the parent.
    SettingData(mainPlane);

    return substrate;
}

void LisaGui::CreateChannelBoxLayerEditor(
    _In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    _In_ const std::shared_ptr<UI::Caching>& pCaching,
    _In_ HWND parent,
    _In_opt_ HINSTANCE hInstance,
    _In_ const std::wstring& className,
    _In_ const RECT& rect
)
{
    using namespace UI;

    const ElementConfig<WindowConfig> channelBoxConfig{
        .hInstance = hInstance,
        .ClassName = className,
        .Rect = rect,
        .Parent = parent,
        .Flags = {
            .Type = ui_type::inbuilt,
            .Modes = ui_modes::customposition,
            .Draw = ui_draw::window::inbuilt | ui_draw::window::inbuilt_border_l | ui_draw::window::inbuilt_border_b,
            .Transform = ui_transform::restore_lx | ui_transform::stretching_y
    },
    };
    HWND channelBox = Element<UI::Window>::Create(pDevice, pCaching, channelBoxConfig);

    // Create object name.

    LONG indent{ 6 };
    const UI::ElementConfig<UI::FieldConfig> oNameConfigure{
        .hInstance = hInstance,
        .ClassName = className + L"_objectName",
        .Rect = {indent, indent, rect.right - indent * 2, gHeightField},
        .Parent = channelBox,
        .Flags = {
            .Type = UI::ui_type::inbuilt,
            .Modes = UI::ui_modes::customposition,
            .Draw = UI::ui_draw::field::simple,
            /*.Transform = ui_transform::restore_rx*/
        }
    };
    UI::Element<UI::Field>::Create(pDevice, pCaching, oNameConfigure);

    // Create field.
    
    std::vector<FieldConfig> fieldConfig
    {
        {.Value = 0.0, .Min = -DBL_MAX, .Max = DBL_MAX, .DecimalPlaces = 6},
        {.Value = 0.0, .Min = -DBL_MAX, .Max = DBL_MAX, .DecimalPlaces = 6},
        {.Value = 0.0, .Min = -DBL_MAX, .Max = DBL_MAX, .DecimalPlaces = 6},
        {.Value = 0.0, .Min = -DBL_MAX, .Max = DBL_MAX, .DecimalPlaces = 6},
        {.Value = 0.0, .Min = -DBL_MAX, .Max = DBL_MAX, .DecimalPlaces = 6},
        {.Value = 0.0, .Min = -DBL_MAX, .Max = DBL_MAX, .DecimalPlaces = 6},
        {.Value = 1.0, .Min = -DBL_MAX, .Max = DBL_MAX, .DecimalPlaces = 6},
        {.Value = 1.0, .Min = -DBL_MAX, .Max = DBL_MAX, .DecimalPlaces = 6},
        {.Value = 1.0, .Min = -DBL_MAX, .Max = DBL_MAX, .DecimalPlaces = 6},
    };

    LONG fieldWidth{ 120 };
    LONG r{ rect.right - (fieldWidth + indent) };
    LONG addTop{ indent * 2 + gHeightField };
    for (auto&& [indx, config] : fieldConfig | std::views::enumerate)
    {
        const UI::ElementConfig<UI::FieldConfig> configure{
                .hInstance = hInstance,
                .ClassName = className + L"_field_" + std::to_wstring(indx),
                .Rect = {r, addTop + gHeightField * static_cast<LONG>(indx), fieldWidth, gHeightField},
                .Parent = channelBox,
                .Flags = {
                .Type = UI::ui_type::inbuilt,
                .Modes = UI::ui_modes::customposition,
                .Draw = UI::ui_draw::field::simple,
                /*.Transform = ui_transform::restore_rx*/
        },
                .Extra = config
        };
        UI::Element<UI::Field>::Create(pDevice, pCaching, configure);
    }

    // Create text.
    std::vector<FontConfig> fontConfig
    {
        {.Input = L"Translate X", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
        {.Input = L"Y", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
        {.Input = L"Z", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
        {.Input = L"Rotate X", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
        {.Input = L"Y", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
        {.Input = L"Z", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
        {.Input = L"Scale X", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
        {.Input = L"Y", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
        {.Input = L"Z", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
    };

    LONG textWidth{ 80 };
    r = rect.right - (textWidth + fieldWidth + indent * 2);
    addTop = indent * 2 + gHeightField;
    for (auto&& [indx, config] : fontConfig | std::views::enumerate)
    {
        const UI::ElementConfig<UI::TextConfig> configure{
            .hInstance = hInstance,
            .ClassName = className + L"_text_" + std::to_wstring(indx),
            .Rect = {r, addTop + gHeightField * static_cast<LONG>(indx), textWidth, gHeightField},
            .Parent = channelBox,
            .Flags = {.Type = UI::ui_type::inbuilt, .Modes = UI::ui_modes::customposition, .Draw = UI::ui_draw::text::label },
            .Extra = {.ImgTxt = config}
        };
        UI::Element<UI::Text>::Create(pDevice, pCaching, configure);
    }  
}

void LisaGui::SettingData(HWND hwnd)
{
    SendMessageW(hwnd, WM_COMMAND, UI::SETTING_DATA_DURING_INITIALIZATION, NULL);
}

void LisaGui::RunMessageLoop()
{
    MSG msg;
    BOOL bRet;

    // Start the message loop.

    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            // handle the error and possibly exit
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    // Return the exit code to the system. 
    //return msg.wParam;
}