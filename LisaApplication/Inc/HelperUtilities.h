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

#ifndef HELPER_UTILITIES_H
#define HELPER_UTILITIES_H

#include "pch.h"
#include "Archive.h"
#include <comdef.h>

namespace LisaApp
{
	class AppException
	{
	public:
		AppException() = default;
		AppException(const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
			
			FunctionName(functionName),
			Filename(filename),
			LineNumber(lineNumber)
		{
		}

		std::wstring ToString(const std::wstring& msg)const
		{
			return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
		}

		
		std::wstring FunctionName;
		std::wstring Filename;
		int LineNumber = -1;
	};
	
	inline std::wstring HelperPath(const std::filesystem::path& path)
	{
		// Define the path to the application's internal directory.
		std::filesystem::path currentPath = std::filesystem::current_path();

		// Find the full path to it.
		std::filesystem::path fullPath{ currentPath / path };

		// Check if the file exists.
		bool exist = std::filesystem::exists(fullPath);

		if (!exist)
		{
			AppException e{};
			MessageBox(nullptr, e.ToString(fullPath).c_str(), L"Failed", MB_OK);
			
			return {};
		}

		// If the file does not exist, an error message will appear.
		//assert(exist && L"Such file does not exist.");

		return fullPath.c_str();
	}
}

class HelperUtilities
{
public:

	static std::filesystem::path Unzipping(const std::filesystem::path& archive)
	{ 
		std::filesystem::path currentPath = std::filesystem::current_path();
		
		std::filesystem::file_status s = std::filesystem::file_status{};
		std::filesystem::path path{ std::filesystem::temp_directory_path() };
		std::filesystem::path archiveName{ archive.stem() };

		// If such a directory already exists, then delete it.
        // This can be useful if the directory was created but not deleted, 
        // for example due to a program crash.
		{
			if (std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(path / archiveName))
				std::filesystem::remove_all(path / archiveName);
		}

		LisaApp::Archive a;
		a.PrintArchiveInfo(archive.string());
		std::vector<std::string> archiveInfo = a.GetArchiveInfo();
		std::filesystem::path readPt{ currentPath / archive };
		a.Read(readPt.string(), a.GetFlags().CreatePath, path, a.GetFlags().AllFiles, {});

		path.append(archiveName.string());


		bool existsArchiveName{};

		if (std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(path))
			existsArchiveName = true;    //exists
		else
			existsArchiveName = false;   //does not exist

		std::string err{};
		err.append("The path to the archive ");
		err.append(archive.filename().string());
		err.append(" was not found.");

		if (!existsArchiveName)
		{
			MessageBoxA(nullptr, err.c_str(), "Failed", MB_OK);
			throw std::runtime_error(err.c_str());
		}

		size_t sz{};
		std::filesystem::path temp;
		
		while (sz < archiveInfo.size())
		{
			temp = path;
			std::string fileName{ archiveInfo[sz] };
			temp.append(fileName);

			if (std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(temp))
			{				
				sz++;
			}
			else
			{
				err.append("Shader ");
				err.append(fileName);
				err.append(" not found.");

				MessageBoxA(nullptr, err.c_str(), "Failed", MB_OK);
				throw std::runtime_error(err.c_str());
			}			
		}

		return path;
	}

	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		// Constant buffers must be a multiple of the minimum hardware
		// allocation size (usually 256 bytes).  So round up to nearest
		// multiple of 256.  We do this by adding 255 and then masking off
		// the lower 2 bytes which store all bits < 256.
		// Example: Suppose byteSize = 300.
		// (300 + 255) & ~255
		// 555 & ~255
		// 0x022B & ~0x00ff
		// 0x022B & 0xff00
		// 0x0200
		// 512
		return (byteSize + 255) & ~255;
	}

	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);
};

#endif // !HELPER_UTILITIES_H