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

#ifndef GLOBAL_VALUE_H
#define GLOBAL_VALUE_H

#include "pch.h"
#include "ColorUI.h"

namespace UI
{
    // std::wstring input
    // std::wstring fontName
    // float fontSize
    // UINT fontRGB
    // DWRITE_FONT_WEIGHT fontWeight
    // DWRITE_FONT_STYLE fontStyle
    // DWRITE_FONT_STRETCH fontStretch
    // DWRITE_TEXT_ALIGNMENT textAlignment
    // DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment
    struct FontConfig
    {
        const wchar_t* Input = {};
        const wchar_t* FontName = L"Verdana";
        float FontSize = 12;
        UINT FontRGB = Colors::Text;
        DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_MEDIUM;
        DWRITE_FONT_STYLE FontStyle = DWRITE_FONT_STYLE_NORMAL;
        DWRITE_FONT_STRETCH FontStretch = DWRITE_FONT_STRETCH_NORMAL;
        DWRITE_TEXT_ALIGNMENT TextAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
        DWRITE_PARAGRAPH_ALIGNMENT ParagraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    };

    // Define a type that will accept either a single string or an array of strings as input.
    // This will be useful for specifying the path to images, 
    // and for writing text we will add a separate structure the Text.
    using varimtx = std::variant<std::wstring, std::vector<std::wstring>, FontConfig, std::vector<FontConfig>>;
    
    // A helper function that will help get the number of elements in variant types, as well as their values.
    template <typename T>
    T ReadVar(const UI::varimtx& imtx, size_t index = 0) {

        // Get the type of the argument
        //using U = std::decay_t<decltype(type)>;
        T type{};

        if constexpr (std::is_same_v<T, size_t>)
        {
            if (std::holds_alternative<std::wstring>(imtx))
                type = 1;
            else if (std::holds_alternative<std::vector<std::wstring>>(imtx))
                type = std::get<std::vector<std::wstring>>(imtx).size();
            else if (std::holds_alternative<UI::FontConfig>(imtx))
                type = 1;
            else if (std::holds_alternative<std::vector<UI::FontConfig>>(imtx))
                type = std::get<std::vector<UI::FontConfig>>(imtx).size();
        }
        else if constexpr (std::is_same_v<T, std::wstring>)
        {
            if (std::holds_alternative<std::wstring>(imtx))
                type = std::get<std::wstring>(imtx);
            else if (std::holds_alternative<std::vector<std::wstring>>(imtx))
                type = std::get<std::vector<std::wstring>>(imtx)[index];
        }
        else if constexpr (std::is_same_v<T, UI::FontConfig>)
        {
            if (std::holds_alternative<UI::FontConfig>(imtx))
                type = std::get<UI::FontConfig>(imtx);
            else if (std::holds_alternative<std::vector<UI::FontConfig>>(imtx))
                type = std::get<std::vector<UI::FontConfig>>(imtx)[index];
        }

        return type;
    }

    // To specify the default value of an input field.
    using DefaultValue = std::variant<std::wstring, INT, UINT, std::double_t>;
    using MinValue = std::variant<INT, UINT, std::double_t>;
    using MaxValue = std::variant<INT, UINT, std::double_t>;

    struct GetFieldData
    {
        UINT BelongingToAGroup{};
        UINT Purpose{};
        std::wstring Data{};
    };

    using ui_draw_variant = std::variant<
        ui_draw::window, 
        ui_draw::button, 
        ui_draw::separator, 
        ui_draw::image, 
        ui_draw::text, 
        ui_draw::field
    >;
    struct FlagsConfig{
        ui_type Type;
        ui_modes Modes;
        ui_draw_variant Draw;
        ui_transform Transform;
        ui_command Command;
    };
 

    struct MessageConfig
    {
        HWND MsgHwnd{ nullptr };
        UINT MsgCommand{};
        UINT MsgSubCommand{};
    };

    enum COMMAND : UINT
    {
        SETTING_DATA_DURING_INITIALIZATION = 0,
        GETTING_DATA_DURING_INITIALIZATION,
        RESIZING_CHILD_WINDOW_FROM_SIMPLE,
        RESIZING_CHILD_WINDOW_FROM_INBUILT,
        RESIZING_ELEMENTS,
        GET_MODIFIABLE_X,
        GET_MODIFIABLE_Y,
        WINDOW_RESTORE_MAXIMIZE,
        WINDOW_MINIMIZE,
        UPDATE_TITLE,
        GET_DATA_FROM_INPUT_FIELD,
        SET_DEFAULT_VALUE_IN_INPUT_FIELD,
        CHILDRENS_WINDOW_SURVEY,
        REVERSE_COMMAND_TO_OPEN_A_POP_UP_WINDOW,
        WINDOW_IS_CHECKED_FOR_FRIEND_OR_FOE,
        RESPONSE_CONFIRMATION_OF_FRIENDLINESS,
        MESSAGE_TO_ROOT_NOFRAME_BUTTON,
        REMOVING_A_WINDOW_FROM_A_CONTAINER,
        CREATE_PRIMITIVES,
        OBTAIN_HWND_OF_NEIGHBORING_SEPARATORS
    };

    constexpr INT gNonClientAreaSize{ 12 };
    constexpr INT gMainMenuBarHeight{ 34 };
    constexpr INT gMainIconPlace{ 34 };
    constexpr INT gButtonWidth{ 24 };
    constexpr INT gButtonHeight{ 24 };
    constexpr INT gPopUpButtonHeight{ 24 };
    constexpr INT gSeparatorPopUpHeight{ 3 };
    constexpr INT gIconPlace{ 26 };     // For the pop-up button.
    constexpr INT gEmptyPlace{ 32 };    // For the pop-up button.
    constexpr INT gImagePlace{ 26 };    // For the pop-up button.
    constexpr INT gFlipPlace{ 40 };     // The indentation on the left where the sublayer changes when clicked with the mouse.
    //constexpr INT gForwardPlace{ 40 };  // The indentation on the right where the sublayer changes when clicked with the mouse.
    constexpr INT gHeightField{ 22 };
    constexpr INT gSeparatorThickness{ 6 };
    constexpr INT gBaseDPI{ 96 };
    constexpr float gIndentLeftEdgeWindow{ 100.0f };
    constexpr float gPI{ 3.14159265358979323846f };
}

#endif // !GLOBAL_VALUE_H