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

#include "..//Inc/TextFieldTools.h"

#include <variant>
#include <numeric>

// The function determines where there are more characters on the left or right.
UI::TextField::NumberSymbolsSides UI::TextField::FindNumberSymbolsSides() const
{
    NumberSymbolsSides n{};

    n = { m_endCaret, m_beginCaret };

    if (m_endCaret > m_beginCaret)
    {
        n = { m_beginCaret, m_endCaret };
    }

    return n;
}

// Function for formatting a string entered into a input field.
std::wstring UI::TextField::ProcessNumber(
    const std::wstring& input, 
    const DefaultValue& defaultValue, 
    size_t decimalPlaces
)
{
    if (input.empty()) return L"";

    size_t start = 0;
    bool is_negative = false;

    // Handling the minus sign.
    if (input[start] == L'-')
    {
        is_negative = true;
        start++;
    }

    // Division into integer and fractional parts.
    size_t dot_pos = input.find(L'.', start);
    std::wstring integer_part;
    std::wstring fractional_part;

    if (dot_pos != std::wstring::npos)
    {
        integer_part = input.substr(start, dot_pos - start);

        // If there is no sign before the point, then we put a zero in front of it.
        // -.123 .123 -> -0.123 0.123
        if (integer_part.empty())
        {
            integer_part.insert(0, L"0");
        }

        fractional_part = input.substr(dot_pos + 1);
    }
    else
    {
        integer_part = input.substr(start);
    }

    // Removing leading zeros from the integer part.
    size_t first_non_zero = integer_part.find_first_not_of(L'0');
    if (first_non_zero != std::wstring::npos)
    {
        integer_part = integer_part.substr(first_non_zero);
    }
    else
    {
        // All zeros or an empty string.
        integer_part = L"0";
    }

    // Assembling the result.
    std::wstring result;
    if (is_negative) result += L'-';

    result += integer_part;
    if (!fractional_part.empty())
    {
        // Add zeros if there are fewer characters than necessary.
        if (fractional_part.size() < decimalPlaces)
        {
            fractional_part.insert(fractional_part.size(), decimalPlaces - fractional_part.size(), '0');
        }
        // Or, we delete the unnecessary ones.
        else
        {
            fractional_part.erase(fractional_part.begin() + decimalPlaces, fractional_part.end());
        }

        result += L"." + fractional_part;
    }
    else
    {
        if (std::holds_alternative<std::double_t>(defaultValue))
        {
            // If there is no fractional part, 
            // we restore it by creating a separator - a period 
            // and fill it with the required number of zeros.
            fractional_part.insert(0, decimalPlaces, '0');
            result += L"." + fractional_part;
        }       
    }

    return result;
}

// We move the focus to the current field. 
// Then, if there is text, we select it and start the timer. 
// If there is no text, we simply start the timer.
void UI::TextField::LeftButtonClick(HWND hwnd, bool state)
{
    // Returning keyboard focus to the parent window.
    //SetFocus(GetAncestor(hwnd, GA_PARENT));

    // Sets the keyboard focus to the specified window. 
    // The window must be attached to the calling thread's message queue.
    // HWND - A handle to the window that will receive the keyboard input. 
    // If this parameter is NULL, keystrokes are ignored.   
    if (state == true)
        SetFocus(hwnd);

    m_setFocus = state;

    if (m_text.size())
    {
        m_isTextHighlighted = state;

        m_endCaret = m_text.size();
        m_beginCaret = 0;
    }

    if (state == true)
        SetTimer(hwnd, m_caretTimer, 600, (TIMERPROC)NULL);

    if (state)
    {
        // Write the current value of the input field to the buffer only on first activation.
        // This will prevent unnecessary data from being written 
        // to the buffer when you click the mouse on an activated field.
        if (m_buffer.empty())
            m_buffer = m_text;
    }
    else
    {
        // We do not draw the caret in inactive elements.
        m_drawingACaret = state;
    }
}

bool UI::TextField::WritingTextToClipboardW(HWND hwnd, wchar_t* inputText)
{
    HGLOBAL hglbCopy{};
    wchar_t* wcharCopy{};
    UINT uFormat = CF_UNICODETEXT;

    if (hwnd == NULL)
        return FALSE;

    // Open the clipboard, and empty it. 
    if (!OpenClipboard(hwnd))
        return FALSE;
    EmptyClipboard();

    // If text is selected, copy it using the CF_UNICODETEXT format.
    size_t length = wcslen(inputText);

    if (length)
    {
        // Allocate a global memory object for the text. 
        hglbCopy = GlobalAlloc(GMEM_MOVEABLE,
            (length + 1) * sizeof(wchar_t));
        if (hglbCopy == NULL)
        {
            CloseClipboard();
            return FALSE;
        }

        // Lock the handle and copy the text to the buffer. 
        wcharCopy = (wchar_t*)GlobalLock(hglbCopy);

        if (wcharCopy) {
            wmemcpy(wcharCopy, inputText, length);
            // Null character manually added.
            wcharCopy[length] = L'\0';
        }
        else {
            CloseClipboard();
            return FALSE;
        }

        GlobalUnlock(hglbCopy);
        // If wide character string with a low-ASCII character, 
        // in UTF-16 it's going to be encoded as the low-ASCII byte followed by a NULL. 
        // Use CF_UNICODETEXT instead of CF_TEXT.
        SetClipboardData(uFormat, hglbCopy);
    }
    // If no text is selected, ?the label as a whole is copied?. 
    else
    {
        // Place a registered clipboard format, the owner-display 
        // format, and the CF_TEXT format on the clipboard using 
        // delayed rendering. 
        SetClipboardData(uFormat, NULL);
        SetClipboardData(CF_OWNERDISPLAY, NULL);
        SetClipboardData(CF_UNICODETEXT, NULL);
    }

    CloseClipboard();

    return TRUE;
}

const wchar_t* UI::TextField::ReadingTextFromClipboardW(HWND hwnd)
{
    const wchar_t* wcharPaste{ L"" };
    HGLOBAL hglb;

    // Checking the incoming format.
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
        return FALSE;
    // Open the clipboard.
    if (!OpenClipboard(hwnd))
        return FALSE;
    // Extract text from clipboard.
    hglb = GetClipboardData(CF_UNICODETEXT);

    if (hglb != NULL)
    {
        // Block memory.
        wcharPaste = (wchar_t*)GlobalLock(hglb);
        if (wcharPaste != NULL)
        {
            // Unlock memory.
            GlobalUnlock(hglb);
        }
    }
    // Close the clipboard.
    CloseClipboard();

    return wcharPaste;
}

void UI::TextField::TimerForCaret(HWND hwnd)
{
    // In order for the caret to disappear and reappear, 
    // we must fulfill the following conditions.                
    if (!m_drawingACaret)
    {
        m_drawingACaret = true;
        InvalidateRect(hwnd, NULL, FALSE);
    }
    else if (m_drawingACaret)
    {
        m_drawingACaret = false;
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

void UI::TextField::DeactivateTextField(HWND hwnd)
{
    m_setFocus = false;
    // When changing focus to the parent window, 
    // turn off the timer and hide the caret.
    if (!m_setFocus)
    {
        // When you press Enter, we remove the text selection 
        // made when you pressed the Shift key.                   
        m_isTextHighlighted = false;

        m_endCaret = 0;
        m_beginCaret = m_endCaret;
        m_buffer.clear();

        KillTimer(hwnd, m_caretTimer);
        m_drawingACaret = false;
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

void UI::TextField::SelectDubleClickTextField(HWND hwnd)
{
    // When pressed once, the text field is highlighted in color without the ability to type.
    // 
    // Reset the selection from the previous fields.
    
    /*for (auto& il : m_isSelection)
        std::replace_if(il.begin(), il.end(), [](bool x) { return x; }, false);*/

    // Select the text field.
    m_isSelection = true;
    // For this type of text field, you must call WM_KILLFOCUS to deactivate the previously selected fields.
    SendMessageW(hwnd, WM_KILLFOCUS, 0, NULL);
    // Return the focus to the window.
    SetFocus(hwnd);
}

void UI::TextField::SetDefaultValue(const DefaultValue& defaultValue, UINT decimalPlaces)
{
    if (std::holds_alternative<std::wstring>(defaultValue))
        m_text = std::get<std::wstring>(defaultValue);

    if (std::holds_alternative<INT>(defaultValue))
        m_text = std::to_wstring(std::get<INT>(defaultValue));

    if (std::holds_alternative<UINT>(defaultValue))
        m_text = std::to_wstring(std::get<UINT>(defaultValue));

    if (std::holds_alternative<std::double_t>(defaultValue))
    {
        m_text = m_tools->DoubleToWstring(std::get<std::double_t>(defaultValue));

        size_t position = m_text.find(L".");

        if (position != std::wstring::npos)
        {
            // Determine the number of digits after the decimal point.
            // Add one since the position is counted from zero.
            m_text.erase(m_text.begin() + (position + 1) + decimalPlaces, m_text.end());
        }
    }
}

// Function for placing text from an input field when first selected with the mouse into the buffer.
std::wstring UI::TextField::GetBufferText() const
{
    return m_buffer;
};

std::wstring UI::TextField::CheckingEnteredCharacters(const std::wstring& str, const DefaultValue& defaultValue)
{
    std::wstring t{};

    auto Checking = [](const auto& vec, std::wstring instr)
        {
            std::wstring w{};
            
            std::for_each(vec.begin(), vec.end(), [&](const std::wstring& v)
                {
                    if (instr == v) { w = instr; }
                }
            );

            return w;
        };

    auto CheckingMinus = [&](const std::wstring& instr)
        {
            std::wstring w{ instr };

            size_t position = m_text.find(L"-");

            NumberSymbolsSides n{ FindNumberSymbolsSides() };

            // If the symbol is present and there is no selection of multiple symbols.
            if (position != std::wstring::npos && n.Left == n.Right)
            {
                w = L"";
            }
            // If the symbol is present and there is a selection several symbols. 
            // But the selection of symbols does not start from the beginning of the line.
            else if (position != std::wstring::npos && n.Left != 0)
            {
                w = L"";
            }
            // If the "-" symbol is missing and the selection of characters 
            // does not start from the beginning of the line.
            else if (position == std::wstring::npos && n.Left != 0)
            {
                w = L"";
            }

            return w;
        };
        
    // Create a character input limit for different types of input fields.

    std::vector<std::wstring> store{ L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"-", L"." };

    // If the input field is of the floating type.
    if (std::holds_alternative<std::double_t>(defaultValue))
    {
        std::vector<std::wstring> storef{ store.cbegin(), store.cend() };
        
        t = Checking(storef, str);

        // If the typed character is "." we should check if there is a similar character in the string.
        // And if such a symbol already exists, then you should not print the same one additionally.
        if (t == L".")
        {
            size_t position = m_text.find(L".");

            NumberSymbolsSides n{ FindNumberSymbolsSides() };

            // If the symbol is present and there is no selection of multiple symbols.
            if (position != std::wstring::npos && n.Left == n.Right)
            {
                t = L"";
            }
            // If the "." symbol is present and several symbols are selected.
            else if (position != std::wstring::npos && n.Left != n.Right)
            {
                // Find out how many characters were allocated.
                std::vector<size_t> v(n.Right - n.Left);
                // Fill the container sequentially starting from the smallest value.
                std::iota(v.begin(), v.end(), n.Left);
                
                // Is there a "." symbol in the interval ?
                auto result = std::find(v.begin(), v.end(), position);

                // If the "." symbol is not found.
                if (result == v.end())
                {
                    t = L"";
                }               
            }            
        }

        // The same with the "-" sign. It must be in one quantity.
        if (t == L"-")
        {
            t = CheckingMinus(t);
        }

    }

    // If the input field is a signed integer type.
    else if (std::holds_alternative<INT>(defaultValue))
    {
        std::vector<std::wstring> storei{ store.cbegin(), store.cend() - 1 };
        
        t = Checking(storei, str);

        // The same with the "-" sign. It must be in one quantity.
        if (t == L"-")
        {
            t = CheckingMinus(t);                     
        }

    }

    // If the input field is a un-signed integer type.
    else if (std::holds_alternative<UINT>(defaultValue))
    {
        std::vector<std::wstring> storeui{ store.cbegin(), store.cend() - 2 };

        t = Checking(storeui, str);
    }

    // If the input field is a wide string type.
    if (std::holds_alternative<std::wstring>(defaultValue))
    {
        return str;
    }

    return t;
}

bool UI::TextField::Typesetting(HWND hwnd, std::wstring input, const DefaultValue& defaultValue)
{
    if (!m_setFocus)
        return 0;

    if (!HIBYTE(GetKeyState(VK_CONTROL)))
    {
        std::wstring t{ input };

        t = CheckingEnteredCharacters(t, defaultValue);
        
        if (t.size())
        {
            NumberSymbolsSides n{ FindNumberSymbolsSides() };

            m_text.erase(n.Left, (n.Right - n.Left));
            m_text.insert(m_text.begin() + n.Left, t[0]);

            m_endCaret = n.Left + 1;
            m_beginCaret = m_endCaret;

            m_isTextHighlighted = false;
        };
    }

    if (HIBYTE(GetKeyState(VK_BACK)) & 0x80)
    {
        if (m_text.size())
        {
            NumberSymbolsSides n{ FindNumberSymbolsSides() };

            if (m_beginCaret == m_endCaret && m_endCaret != 0)
            {
                n.Left--;
                m_text.erase(n.Left, 1);
            }
            else
            {
                m_text.erase(n.Left, (n.Right - n.Left));
            }
            m_endCaret = n.Left;
            m_beginCaret = m_endCaret;

            m_isTextHighlighted = false;
        }
    }

    if (!HIBYTE(GetKeyState(VK_SHIFT)) && HIBYTE(GetKeyState(VK_LEFT)))
    {
        if (m_isTextHighlighted)
        {
            m_isTextHighlighted = false;

            if (m_beginCaret == m_endCaret)
            {
                m_endCaret = m_endCaret ? (m_endCaret -= 1) : 0;
            }
        }
        else
        {
            m_endCaret = m_endCaret ? (m_endCaret -= 1) : 0;
        }

        m_beginCaret = m_endCaret;
    }

    if (!HIBYTE(GetKeyState(VK_SHIFT)) && HIBYTE(GetKeyState(VK_RIGHT)))
    {
        if (m_isTextHighlighted)
        {
            m_isTextHighlighted = false;

            if (m_beginCaret == m_endCaret)
            {
                m_endCaret++;
                if (m_endCaret > m_text.size())
                {
                    m_endCaret = m_text.size();
                }
            }
        }
        else
        {
            m_endCaret++;
            if (m_endCaret > m_text.size())
            {
                m_endCaret = m_text.size();
            }
        }

        m_beginCaret = m_endCaret;
    }

    // Select text with the Shift key pressed and the left button pressed.
    if (HIBYTE(GetKeyState(VK_SHIFT)) && HIBYTE(GetKeyState(VK_LEFT)))
    {
        m_isTextHighlighted = true;

        m_endCaret = m_endCaret ? (m_endCaret -= 1) : 0;
    }

    //Select the text with the Shift key pressed and the right button.
    if (HIBYTE(GetKeyState(VK_SHIFT)) && HIBYTE(GetKeyState(VK_RIGHT)))
    {
        m_isTextHighlighted = true;

        m_endCaret++;
        if (m_endCaret > m_text.size())
        {
            m_endCaret = m_text.size();
        }
    }

    if (HIBYTE(GetKeyState(VK_RETURN)) & 0x80)
    {
        // Remove focus from the window to completely deactivate the text field.
        SendMessageW(hwnd, WM_KILLFOCUS, 0, NULL);

        m_setFocus = false;
        // When changing focus to the parent window, 
        // turn off the timer and hide the caret.
        if (!m_setFocus)
        {
            m_drawingACaret = false;

            // When you press Enter, we remove the text selection 
            // made when you pressed the Shift key.
            m_isTextHighlighted = false;
            m_endCaret = 0;
            m_beginCaret = m_endCaret;

            KillTimer(hwnd, m_caretTimer);
        }
    }

    if (HIBYTE(GetKeyState(VK_CONTROL)) && HIBYTE(GetKeyState(0x43)))// 0x43 C key
    {
        if (m_text.size() && m_endCaret != m_beginCaret)
        {
            NumberSymbolsSides n{ FindNumberSymbolsSides() };

            std::wstring sub = m_text.substr(n.Left, (n.Right - n.Left));

            wchar_t textBuffer[1024]{};
            wcscpy_s(textBuffer, sub.c_str());

            WritingTextToClipboardW(hwnd, textBuffer);
        }
    }

    if (HIBYTE(GetKeyState(VK_CONTROL)) && HIBYTE(GetKeyState(0x58)))// 0x58 X key
    {
        if (m_text.size() && m_endCaret != m_beginCaret)
        {
            NumberSymbolsSides n{ FindNumberSymbolsSides() };

            std::wstring sub = m_text.substr(n.Left, (n.Right - n.Left));

            wchar_t textBuffer[1024]{};
            wcscpy_s(textBuffer, sub.c_str());

            WritingTextToClipboardW(hwnd, textBuffer);

            m_text.erase(n.Left, (n.Right - n.Left));

            m_endCaret = n.Left;
            m_beginCaret = m_endCaret;

            m_isTextHighlighted = false;
        }
    }

    if (HIBYTE(GetKeyState(VK_CONTROL)) && HIBYTE(GetKeyState(0x56)))// 0x56 V key
    {
        NumberSymbolsSides n{ FindNumberSymbolsSides() };

        m_text.erase(n.Left, (n.Right - n.Left));

        std::wstring sub{ ReadingTextFromClipboardW(hwnd) };

        for (size_t s = 0; s < sub.size(); s++)
        {
            m_text.insert(m_text.begin() + n.Left, sub[sub.size() - (1 + s)]);
        }

        m_endCaret = n.Left + sub.size();
        m_beginCaret = m_endCaret;

        m_isTextHighlighted = false;
    }

    InvalidateRect(hwnd, NULL, FALSE);

    return true;
}

UI::OutputField UI::TextField::OnDrawText()
{
    OutputField f;
    
    f =
    {
        .IsSelection = m_isSelection,
        .SetFocus = m_setFocus,
        .DrawingACaret = m_drawingACaret,
        .IsTextHighlighted = m_isTextHighlighted,
        .BeginCaret = m_beginCaret,
        .EndCaret = m_endCaret,
        .InputText = m_text
    };

    return f;
}
