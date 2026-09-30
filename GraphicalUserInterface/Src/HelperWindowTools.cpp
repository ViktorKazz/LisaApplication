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

#include "..//Inc/HelperWindowTools.h"
#include "..//Inc/Primitives2D.h"

#include <iomanip>
#include <sstream>

// This function compares the size of the client area with the size of the monitor screen.
bool HelperWTools::ComparisonWindowSizes(HWND hwnd)
{
	bool comparison{};

	RECT workAreaSize{};
	// SPI_GETWORKAREA
	// Retrieves the size of the working area on the primary display monitor. 
	// The working area is the portion of the screen that is not hidden by 
	// the system taskbar or the application desktop toolbar.
	// For more information https://learn.microsoft.com/ru-ru/windows/win32/api/winuser/nf-winuser-systemparametersinfow
	SystemParametersInfoW(SPI_GETWORKAREA, 0, &workAreaSize, 0);
	RECT clientRc;
	GetClientRect(hwnd, &clientRc);

	if (workAreaSize.right == clientRc.right)
	{
		if (workAreaSize.bottom == clientRc.bottom)
		{
			comparison = true;
		}
	}

	return comparison;
}

void HelperWTools::MinMaxWindow(LPARAM lParam, UINT minX, UINT minY)
{
	// We set the minimum and maximum window dimensions.
	// We set the minimum values arbitrarily.
	// The maximum values are the size of the entire working area of the screen excluding the taskbar strip.
	// The function (SystemParametersInfoW) is needed to define a work area without a taskbar bar. 
	RECT workAreaSize{};
	SystemParametersInfoW(SPI_GETWORKAREA, 0, &workAreaSize, 0);

	MINMAXINFO* mmi = (MINMAXINFO*)lParam;
	mmi->ptMinTrackSize.x = minX;
	mmi->ptMinTrackSize.y = minY;
	mmi->ptMaxSize.x = workAreaSize.right;
	mmi->ptMaxSize.y = workAreaSize.bottom;
}

void HelperWTools::ButtonDoubleClick(const HWND& hwnd, const LPARAM& lParam, INT minY, INT maxY, bool resize)
{
	POINT pt{};
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	// The ScreenToClient function converts the screen coordinates
	// of a specified point on the screen to client - area coordinates.
	ScreenToClient(hwnd, &pt);

	RECT workAreaSize{};
	// SPI_GETWORKAREA
	// Retrieves the size of the working area on the primary display monitor. 
	// The working area is the portion of the screen that is not hidden by 
	// the system taskbar or the application desktop toolbar.
	// For more information https://learn.microsoft.com/ru-ru/windows/win32/api/winuser/nf-winuser-systemparametersinfow
	SystemParametersInfoW(SPI_GETWORKAREA, 0, &workAreaSize, 0);
	RECT clientRc;
	GetClientRect(hwnd, &clientRc);

	if (resize)
	{
		// We determine the place where there will be a double click with the mouse.
		if (pt.y > minY && pt.y < minY + maxY)
		{
			// If the window is already maximized, it returns to its previous size and vice versa.
			if (clientRc.right == workAreaSize.right && clientRc.bottom == workAreaSize.bottom)
			{
				ShowWindow(hwnd, SW_RESTORE);
			}
			else
			{
				ShowWindow(hwnd, SW_MAXIMIZE);
			}
		}
	}	
}

LRESULT HelperWTools::NCHitTest(
	const HWND& hwnd, 
	const LPARAM& lParam, 
	INT nonClientAreaSize, 
	INT menuBarHeight, 
	bool resize
)
{
	LRESULT lResult{};

	RECT clientRc;
	GetClientRect(hwnd, &clientRc);

	POINT pt{};
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	// The ScreenToClient function converts the screen coordinates
	// of a specified point on the screen to client - area coordinates.
	ScreenToClient(hwnd, &pt);

	if (resize)
	{
		// We define non-client areas -> top-left, top and top-right.
		if (pt.y < nonClientAreaSize)
		{
			if (pt.x < nonClientAreaSize)
			{
				return HTTOPLEFT;
			}
			else if (pt.x > (clientRc.right - nonClientAreaSize))
			{
				return HTTOPRIGHT;
			}
			return HTTOP;
		}
		// We define non-client areas -> bottom-left, bottom and bottom-right.
		if (pt.y > (clientRc.bottom - nonClientAreaSize))
		{
			if (pt.x < nonClientAreaSize)
			{
				return HTBOTTOMLEFT;
			}
			else if (pt.x > (clientRc.right - nonClientAreaSize))
			{
				return HTBOTTOMRIGHT;
			}
			return HTBOTTOM;
		}
		if (pt.x < nonClientAreaSize)
		{
			return HTLEFT;
		}
		if (pt.x > (clientRc.right - nonClientAreaSize))
		{
			return HTRIGHT;
		}
	}
	// We define non-client areas -> menu bar.
	if (pt.y > nonClientAreaSize && pt.y < nonClientAreaSize + menuBarHeight)
	{
		if (pt.x > nonClientAreaSize && pt.x < (clientRc.right - nonClientAreaSize))
		{
			return HTCAPTION;
		}
	}
	if (pt.y > nonClientAreaSize && pt.y < (clientRc.bottom - nonClientAreaSize))
	{
		if (pt.x > nonClientAreaSize && pt.x < (clientRc.right - nonClientAreaSize))
		{
			return HTCLIENT;
		}
	}

	return lResult;
}

bool HelperWTools::SearchByOneKey(const wchar_t* source, const wchar_t* key)
{
	bool find{ false };

	std::vector<wchar_t*> temporaryStorage{};

	if (source)
	{
		// We split the line and write individual words into a container..
		wchar_t* buffer = NULL;
		wchar_t* string = new wchar_t[wcslen(source) + 1] {};

		for (size_t i = 0; i < wcslen(source) + 1; i++)
		{
			string[i] = source[i];
		}

		wchar_t* token = wcstok_s(string, L"_", &buffer);

		while (token)
		{
			temporaryStorage.push_back(token);
			token = wcstok_s(NULL, L"_", &buffer);
		}

		// We check for matches.
		for (const auto& ts : temporaryStorage)
		{
			if (wcscmp(ts, key) == 0)
			{
				find = true;
			}
		}

		delete[] string;
	}

	return find;
}

std::wstring HelperWTools::GetKey(UI::ui_type type)
{
	std::wstring key{};

	switch (type)
	{
	case UI::ui_type::simple:
		key = L"simple";
		break;
	case UI::ui_type::popUp:
		key = L"popUp";
		break;
	case UI::ui_type::inbuilt:
		key = L"inBuilt";
		break;
	case UI::ui_type::message:
		key = L"message";
		break;
	case UI::ui_type::separator:
		key = L"separator";
		break;
	default:
		throw std::runtime_error(std::format("{} {}", "Invalid UI type", static_cast<std::uint16_t>(type)));
	}

	return key;
}

// The function returns the fully qualified name of the class.
std::wstring HelperWTools::CreateClass(UI::ui_type type, const std::wstring& name)
{
	std::wstring key{ GetKey(type) };	
	std::wstring wClass{ name + L"_" + key };

	//OutputDebugString(wClass.c_str());
	//OutputDebugString(L"\n");

	return wClass;
}

// Creates a Direct2D bitmap from the specified file name.
HRESULT HelperWTools::LoadBitmapFromFile(
	const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
	const Microsoft::WRL::ComPtr<IWICImagingFactory2>& pWICFactory,
	ID2D1Bitmap** ppBitmap,
	PCWSTR uri,
	UINT destinationWidth,
	UINT destinationHeight
)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder;
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pSource;
	Microsoft::WRL::ComPtr<IWICStream> pStream;
	Microsoft::WRL::ComPtr<IWICFormatConverter> pConverter;
	Microsoft::WRL::ComPtr<IWICBitmapScaler>pScaler;

	hr = pWICFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		pDecoder.ReleaseAndGetAddressOf()
	);
	
	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		UI::ThrowIfFailed(pDecoder.Get()->GetFrame(0, pSource.ReleaseAndGetAddressOf()));

		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		UI::ThrowIfFailed(pWICFactory->CreateFormatConverter(pConverter.ReleaseAndGetAddressOf()));

		// If a new width or height was specified, create an
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth, originalHeight;
			UI::ThrowIfFailed(pSource.Get()->GetSize(&originalWidth, &originalHeight));

			if (destinationWidth == 0)
			{
				FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
				destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
			}
			else if (destinationHeight == 0)
			{
				FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
				destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
			}

			UI::ThrowIfFailed(pWICFactory->CreateBitmapScaler(pScaler.ReleaseAndGetAddressOf()));

			UI::ThrowIfFailed(pScaler.Get()->Initialize(
				pSource.Get(),
				destinationWidth,
				destinationHeight,
				WICBitmapInterpolationModeCubic
			));

			UI::ThrowIfFailed(pConverter.Get()->Initialize(
				pScaler.Get(),
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
			));
		}
		else // Don't scale the image.
		{
			UI::ThrowIfFailed(pConverter.Get()->Initialize(
				pSource.Get(),
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
			));
		}
		// Create a Direct2D bitmap from the WIC bitmap.

		UI::ThrowIfFailed(pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter.Get(),
			NULL,
			ppBitmap
		));
	}
	else if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
	{		
		wchar_t error[256]{};
		wcsncpy_s(
			error, 
			L"hr = ERROR_FILE_NOT_FOUND : The system cannot find the file ", 
			wcslen(L"hr = ERROR_FILE_NOT_FOUND : The system cannot find the file ")
		);
		wcsncat_s(error, uri, wcslen(uri));
		
		OutputDebugString(error);
		OutputDebugString(L"\n");
	}

	return hr;
}

std::wstring HelperWTools::FloatToWstring(std::float_t value)
{
	std::wostringstream oss;
	// Ensures that the period is used as a decimal separator.
	// Isolates the influence of system regional settings.
	oss.imbue(std::locale("C"));

	// max_digits10: 
	// For float: 9 digits,
	// For double: 17 digits,
	// Guarantees unambiguous restoration of the original value.
	oss << std::fixed << std::setprecision(std::numeric_limits<std::float_t>::max_digits10) << value;
	return oss.str();
}

std::float_t HelperWTools::WstringToFloat(const std::wstring& str)
{
	std::wistringstream iss(str);

	// Ensures that the period is used as a decimal separator.
	// Isolates the influence of system regional settings.
	iss.imbue(std::locale("C"));

	std::float_t value;
	iss >> value;

	// Checking for complete parsing and absence of errors.
	if (iss.fail() || !iss.eof()) {
		throw std::invalid_argument("Invalid std::float_t conversion");
	}

	return value;
}

std::double_t HelperWTools::WstringToDouble(const std::wstring& str)
{
	std::wistringstream iss(str);

	// Ensures that the period is used as a decimal separator.
	// Isolates the influence of system regional settings.
	iss.imbue(std::locale("C"));

	std::double_t value;
	iss >> value;

	// Checking for complete parsing and absence of errors.
	if (iss.fail() || !iss.eof()) {
		throw std::invalid_argument("Invalid std::double_t conversion");
	}

	return value;
}

std::wstring HelperWTools::DoubleToWstring(std::double_t value)
{
	std::wostringstream oss;
	// Ensures that the period is used as a decimal separator.
	// Isolates the influence of system regional settings.
	oss.imbue(std::locale("C"));

	// max_digits10: 
	// For float: 9 digits,
	// For double: 17 digits,
	// Guarantees unambiguous restoration of the original value.
	oss << std::fixed << std::setprecision(std::numeric_limits<std::double_t>::max_digits10) << value;
	return oss.str();
}

void HelperWTools::DrawFlipIcon(
	const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pRenderTarget,
	ID2D1Image* input1, 
	ID2D1Image* input2, 
	D2D1_SIZE_F size, 
	D2D1_SIZE_F scale,
	bool pressed
)
{
	Microsoft::WRL::ComPtr<ID2D1Effect> translationEffect{ nullptr };
	pRenderTarget->CreateEffect(CLSID_D2D12DAffineTransform, &translationEffect);

	D2D1_MATRIX_3X2_F matrix{};

	if (pressed)
		translationEffect->SetInput(0, input1);
	else
		translationEffect->SetInput(0, input2);

	matrix = D2D1::Matrix3x2F::Scale(scale);
	matrix = matrix * D2D1::Matrix3x2F::Translation(size);
	translationEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);
	pRenderTarget->DrawImage(translationEffect.Get());
};
