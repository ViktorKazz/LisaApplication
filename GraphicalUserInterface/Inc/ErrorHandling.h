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

#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include "windows.h"
#include <string>
#include <chrono>
#include <format>
#include <source_location>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace UI
{
    // Helper class for COM exceptions
    template<typename T>
    class ErrorHandling : public std::exception
    {
    public:

        ErrorHandling(
            T t, 
            const std::string_view message, 
            const std::source_location& location
        ) noexcept : 
            m_t(t), 
            m_message{ message }, 
            m_location{ location } 
        {
            ErrorInformation(t);
        }


        virtual const char* what() const noexcept override {
            return "Message missing";
        }

        // Display error information on the screen or save it to a file.
        void ErrorInformation(T t)
        {
            std::string err = std::format(
                "File: {}\n Line: {}\n Column: {}\n Function name: {}\n Output value: {}\n Message: {}\n",
                m_location.file_name(),
                m_location.line(),
                m_location.column(),
                m_location.function_name(),
                t,
                m_message.empty() ? what() : m_message
            );

#ifdef _DEBUG
            MessageBoxA(NULL, err.c_str(), "Error!", NULL);
#else
            MessageBoxA(NULL, "The error report was saved in the log file.", "Error!", NULL);
            WritingLogs(err);
#endif
        }

        bool IsExists(const std::filesystem::path& p, std::filesystem::file_status s = std::filesystem::file_status{});
        void WritingLogs(const std::string& error);

    private:
        T m_t;
        const std::string_view m_message{};
        const std::source_location m_location{};
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr, const std::string_view message = {}, 
        const std::source_location location = std::source_location::current()
    )
    {
        if (FAILED(hr))
            throw ErrorHandling(hr, message, location);
    }

    // Concept for checking type callability.
    template <typename T>
    concept Type = std::disjunction_v<
        std::is_same<T, int>,
        std::is_same<T, INT>,
        std::is_same<T, bool>,
        std::is_same<T, BOOL>
    >;

    // Error handling for type: int and bool
    template <typename Type>
    Type Error(Type t, const std::string_view message = {}, 
        const std::source_location location = std::source_location::current())
    {
        if constexpr (std::is_same_v<Type, int> || std::is_same_v<Type, INT>)
        {
            if (t < 0)
                throw ErrorHandling(t, message, location);
        }

        if constexpr (std::is_same_v<Type, bool> || std::is_same_v<Type, BOOL>)
        {
            if (!t)
                throw ErrorHandling(t, message, location);
        }

        return t;
    }

    // Error handling for type: HWND
    inline HWND HWNDError(HWND hwnd, const std::string_view message = {},
        const std::source_location location = std::source_location::current()
    )
    {
        if (!hwnd)
            throw ErrorHandling(NULL, message, location);
        return hwnd;
    }

    template<typename T>
    inline bool ErrorHandling<T>::IsExists(const std::filesystem::path& p, std::filesystem::file_status s) 
    {
        if (std::filesystem::status_known(s)) 
            return std::filesystem::exists(s);
        else 
            return std::filesystem::exists(p);
    }

    // Function for writing logs to a file.
    template<typename T>
    inline void ErrorHandling<T>::WritingLogs(const std::string& error)
    {
        std::string fileName = std::format("{:%F %T %Z}.log",
            std::chrono::zoned_time{ std::chrono::current_zone(),
                                    std::chrono::system_clock::now() });

        std::replace(fileName.begin(), fileName.end(), ':', '-');

        std::filesystem::path folderName{ "Logs" };
        std::filesystem::path path{ std::filesystem::absolute("..\\LisaApplication") };
        path /= folderName;

        if (!IsExists(path)) {
            if (!std::filesystem::create_directory(path)) {
                // Handling directory creation error.
                std::cerr << "Failed to create directory: " << path << std::endl;
                return;
            }
        }

        std::filesystem::path current{ std::filesystem::absolute(path) };
        current /= fileName;

        std::ofstream fout{ current, std::ios::out };

        if (!fout.is_open())
        {
            MessageBoxA(NULL, "Error opening file", "Error!", NULL);
        }

        fout << error;
    }
}

#ifdef __MINGW32__
namespace Microsoft
{
    namespace WRL
    {
        namespace Wrappers
        {
            class Event
            {
            public:
                Event() noexcept : m_handle{} {}
                explicit Event(HANDLE h) noexcept : m_handle{ h } {}
                ~Event() { if (m_handle) { ::CloseHandle(m_handle); m_handle = nullptr; } }

                void Attach(HANDLE h) noexcept
                {
                    if (h != m_handle)
                    {
                        if (m_handle) ::CloseHandle(m_handle);
                        m_handle = h;
                    }
                }

                bool IsValid() const { return m_handle != nullptr; }
                HANDLE Get() const { return m_handle; }

            private:
                HANDLE m_handle;
            };
        }
    }
}
#else
#include <wrl/event.h>
#endif

#endif // !ERROR_HANDLING_H