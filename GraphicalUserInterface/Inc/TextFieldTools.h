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

#ifndef TEXT_FIELD_TOOLS_H
#define TEXT_FIELD_TOOLS_H

#include <utility>
#include <unordered_map>
#include <string>
#include <windows.h>
#include <algorithm>

#include "GlobalValue.h"

namespace UI
{
    struct OutputField
    {
        bool IsSelection{};
        bool SetFocus{};
        bool DrawingACaret{};
        bool IsTextHighlighted{};

        size_t BeginCaret{};
        size_t EndCaret{};

        std::wstring InputText{};
    };

    class TextField
    {
    public:
        TextField() = default;
        virtual ~TextField() = default;

        // Accessors.

        void SetText(const std::wstring& text)
        {
            m_text = text;
        };

        struct NumberSymbolsSides
        {
            size_t Left{};
            size_t Right{};
        };

        template<typename T, typename A, typename B>
        std::wstring Checking(
            const T& defaultValue, 
            const A& min,
            const B& max,
            const UINT& decimalPlaces,
            const std::wstring& text, 
            const std::wstring& buffer
        ) const
        {
            std::wstring txt;

            // Update the text entered in the field.
            // For example, if the input field has a floating type, 
            // then instead of the entered .0123 it will be converted to 0.0123.

            // Also here the value entered in the input field is checked for a range.
            // The string value remains unchanged for now.

            if (std::holds_alternative<std::wstring>(defaultValue))
                txt = text;

            if (std::holds_alternative<INT>(defaultValue))
            {
                INT temp{ std::stoi(text) };

                if (temp >= min && temp <= max)
                {
                    txt = std::to_wstring(temp);
                }
                else if (min == 0 && max == 0)
                {
                    txt = std::to_wstring(temp);
                }
                else
                {
                    txt = buffer;
                }
            }

            if (std::holds_alternative<UINT>(defaultValue))
            {
                // The unsigned long type has a size of 4 bytes, just like the unsigned integer.
                // Therefore, it was decided to use the std::stoul function.
                UINT temp{ std::stoul(text) };

                if (temp >= min && temp <= max)
                {
                    txt = std::to_wstring(temp);
                }
                else if (min == 0 && max == 0)
                {
                    txt = std::to_wstring(temp);
                }
                else
                {
                    txt = buffer;
                }

            }

            if (std::holds_alternative<float>(defaultValue))
            {
                float temp{ std::stof(text) };

                if (temp >= min && temp <= max)
                {
                    txt = std::to_wstring(temp);

                    // Determine the number of digits after the decimal point.
                    txt.erase(txt.end() - decimalPlaces, txt.end());
                }
                else if (min == 0 && max == 0)
                {
                    txt = std::to_wstring(temp);
                }
                else
                {
                    txt = buffer;
                }
            }

            return txt;
        };

        NumberSymbolsSides FindNumberSymbolsSides() const;

        // Function for formatting a string entered into a input field.
        std::wstring ProcessNumber(const std::wstring& input, const DefaultValue& defaultValue, size_t decimalPlaces);
        void LeftButtonClick(HWND hwnd, bool state);

        bool WritingTextToClipboardW(HWND hwnd, wchar_t* inputText);
        const wchar_t* ReadingTextFromClipboardW(HWND hwnd);        

        void TimerForCaret(HWND hwnd);
        void DeactivateTextField(HWND hwnd);

        void SelectDubleClickTextField(HWND hwnd);
        
        void SetDefaultValue(const DefaultValue& defaultValue, UINT decimalPlaces);
        std::wstring GetBufferText() const;

        std::wstring CheckingEnteredCharacters(const std::wstring& str, const DefaultValue& defaultValue);

        bool Typesetting(HWND hwnd, std::wstring input, const DefaultValue& defaultValue);

        OutputField OnDrawText();

    private:
        HelperWTools* m_tools{};
        // Timer for blinking the caret in a text field.
        UINT_PTR m_caretTimer{};

        bool m_isSelection{};
        bool m_setFocus{};
        bool m_drawingACaret{};
        bool m_isTextHighlighted{};
        size_t m_beginCaret{};
        size_t m_endCaret{};
        std::wstring m_text{};
        std::wstring m_buffer{};

    };
}

#endif // !TEXT_FIELD_TOOLS_H