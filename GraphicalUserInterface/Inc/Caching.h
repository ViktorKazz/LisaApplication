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

#ifndef CACHING_CLASS_H
#define CACHING_CLASS_H

#include "D3D11DeviceResources.h"

namespace UI
{
	constexpr LONG gNonClientAreaSizeMultTwo{ gNonClientAreaSize * 2 };
	constexpr LONG gNonClientAreaSizeDivTwo{ gNonClientAreaSize / 2 };
	constexpr LONG gNonClientAreaSizeDivThree{ gNonClientAreaSize / 3 };
	constexpr LONG gMMenuBarHeightMinBtnHeightDivTwo{ (gMainMenuBarHeight - gButtonHeight) / 2 };

	constexpr LONG gAdditionalBottombarSimpleLeft{ gNonClientAreaSizeDivThree + gNonClientAreaSize };
	constexpr LONG gAdditionalBottombarSimpleTop{ gNonClientAreaSize + (gMainMenuBarHeight - gMMenuBarHeightMinBtnHeightDivTwo) };
	constexpr LONG gAdditionalBottombarSimpleRight{ (gNonClientAreaSizeDivThree * 2) + gNonClientAreaSizeMultTwo };
	constexpr LONG gAdditionalMRMCLeft{ gNonClientAreaSizeDivTwo * 3 };
	constexpr LONG gAdditionalMRMCTop{ gNonClientAreaSize + gMMenuBarHeightMinBtnHeightDivTwo };

	constexpr RECT gCachingTopbarRect
	{
		gNonClientAreaSizeDivTwo + gNonClientAreaSize + gMainMenuBarHeight,
		gNonClientAreaSize + gMMenuBarHeightMinBtnHeightDivTwo,
		// Windows of this kind must have the POS_MODIFIABLE_X flag,
		// which means that they will expand automatically depending on the number of buttons, 
		// and therefore the value of the "r" variable is chosen randomly.
		0,
		gButtonHeight
	};

	class Caching
	{
	public:
		Caching() = default;
		~Caching() = default;

		// Accessors.

		std::wstring GetVirtualKey(WPARAM wParam, bool KeyState) 
		{ 
			auto it = m_virtualKey.contains({ wParam, KeyState });
			if (it) 
				return m_virtualKey[{wParam, KeyState}];
			else 
				return std::wstring();
		};

		void SetCachingRECT(std::wstring key, RECT meaning)
		{
			m_cachingRectMap.insert_or_assign(key, meaning);
		}

		RECT GetCachingRECT(std::wstring key)
		{
			return m_cachingRectMap.contains(key) ? m_cachingRectMap[key] : RECT();
		}

		bool ContainsCachingRECT(std::wstring key)
		{
			return m_cachingRectMap.contains(key) ? true : false;
		}

		auto GetInternalBackingBrush()           const noexcept { return m_pInternalBacking.Get(); };
		auto GetExternalBackingBrush()           const noexcept { return m_pExternalBacking.Get(); };
		auto GetIntermediateOutlineBrush()       const noexcept { return m_pIntermediateOutline.Get(); };
		auto GetTitleBrush()                     const noexcept { return m_pTitle.Get(); };
		auto GetShadowBrush()                    const noexcept { return m_pShadow.Get(); };
		auto GetPopUpBrush()                     const noexcept { return m_pPopUp.Get(); };
		auto GetPopUpFrameBrush()                const noexcept { return m_pPopUpFrame.Get(); };

		auto GetFieldBackgroundABrush()          const noexcept { return m_pFieldBackgroundA.Get(); };
		auto GetFieldBackgroundBBrush()          const noexcept { return m_pFieldBackgroundB.Get(); };
		auto GetFieldFrameNoActiveBrush()        const noexcept { return m_pFieldFrameNoActive.Get(); };
		auto GetFieldFrameActiveBrush()          const noexcept { return m_pFieldFrameActive.Get(); };
		auto GetFieldItemSelectedBrush()         const noexcept { return m_pItemSelected.Get(); };
		auto GetFieldTextDClickSelectionBrush()  const noexcept { return m_pTextDClickSelection.Get();};
		auto GetFieldTextSelectionBrush()        const noexcept { return m_pTextSelection.Get(); };
		auto GetFieldTextBrush()                 const noexcept { return m_pText.Get(); };
		auto GetFieldCaretBrush()                const noexcept { return m_pCaret.Get(); };

		auto GetButtonNoActive()                 const noexcept { return m_pButtonNoActive.Get(); };
		auto GetButtonPreActive()                const noexcept { return m_pButtonPreActive.Get(); };
		auto GetButtonActive()                   const noexcept { return m_pButtonActive.Get(); };		
		auto GetButtonMinimize()                 const noexcept { return m_pButtonMinimize.Get(); };
		auto GetButtonMinimizeOutline()          const noexcept { return m_pButtonMinimizeOutline.Get(); };
		auto GetButtonRestore()                  const noexcept { return m_pButtonRestore.Get(); };
		auto GetButtonRestoreOutline()           const noexcept { return m_pButtonRestoreOutline.Get(); };
		auto GetButtonClose()                    const noexcept { return m_pButtonClose.Get(); };
		auto GetButtonCloseOutline()             const noexcept { return m_pButtonCloseOutline.Get(); };		
		auto GetButtonNoActiveNF()               const noexcept { return m_pButtonNoActiveNF.Get(); };
		auto GetButtonPreActiveNF()              const noexcept { return m_pButtonPreActiveNF.Get(); };
		auto GetButtonActiveNF()                 const noexcept { return m_pButtonActiveNF.Get(); };
		auto GetButtonNoActivePopUp()            const noexcept { return m_pButtonNoActivePopUp.Get(); };
		auto GetButtonPreActivePopUp()           const noexcept { return m_pButtonPreActivePopUp.Get(); };
		auto GetButtonActivePopUp()              const noexcept { return m_pButtonActivePopUp.Get(); };
		auto GetButtonTxtNoActive()              const noexcept { return m_pButtonTxtNoActive.Get(); };
		auto GetButtonTxtNFNoActive()            const noexcept { return m_pButtonTxtNFNoActive.Get(); };
		auto GetButtonTxtPreActive()             const noexcept { return m_pButtonTxtPreActive.Get(); };
		auto GetButtonTxtActive()                const noexcept { return m_pButtonTxtActive.Get(); };
		auto GetButtonFrameOutlinerNoActive()    const noexcept { return m_pButtonFrameOutlinerNoActive.Get(); };
		auto GetButtonFrameOutlinerPreActive()   const noexcept { return m_pButtonFrameOutlinerPreActive.Get(); };
		auto GetButtonFrameOutlinerActive()      const noexcept { return m_pButtonFrameOutlinerActive.Get(); };
		auto GetButtonNFrameOutlinerNoActive()   const noexcept { return m_pButtonNFrameOutlinerNoActive.Get(); };
		auto GetButtonNFrameOutlinerPreActive()  const noexcept { return m_pButtonNFrameOutlinerPreActive.Get(); };
		auto GetButtonNFrameOutlinerActive()     const noexcept { return m_pButtonNFrameOutlinerActive.Get(); };
		auto GetButtonPFrameOutlinerNoActive()   const noexcept { return m_pButtonPFrameOutlinerNoActive.Get(); };
		auto GetButtonPFrameOutlinerPreActive()  const noexcept { return m_pButtonPFrameOutlinerPreActive.Get(); };
		auto GetButtonPFrameOutlinerActive()     const noexcept { return m_pButtonPFrameOutlinerActive.Get(); };
				
		auto GetFlipAreaEllipseBrush()           const noexcept { return m_pFlipAreaEllipse.Get(); };
		auto GetFlipAreaTriangleBrush()          const noexcept { return m_pFlipAreaTriangle.Get(); };
		auto GetFlipGradientStops()              const noexcept { return m_pFlipGradientStops.Get(); };

		auto GetBorder()                         const noexcept { return m_pBorder.Get(); };

		auto GetTitleTextFormat()                const noexcept { return m_pTitleTextFormat.Get(); };
		auto GetLabelTextFormat()                const noexcept { return m_pLabelTextFormat.Get(); };
		auto GetFieldTextFormat()                const noexcept { return m_pFieldTextFormat.Get(); };
		auto GetButtonTextFormat()               const noexcept { return m_pButtonTextFormat.Get(); };
		auto GetButtonPopUpTextFormat()          const noexcept { return m_pButtonPopUpTextFormat.Get(); };
		auto GetButtonPopUpExtraTextFormat()     const noexcept { return m_pButtonPopUpExtraTextFormat.Get(); };


		auto GetIconButtonClose()                const noexcept { return m_pIconButtonClose.Get(); };
		auto GetIconButtonRestore()              const noexcept { return m_pIconButtonRestore.Get(); };
		auto GetIconButtonMinimize()             const noexcept { return m_pIconButtonMinimize.Get(); };
		auto GetIconButtonGearNoSelected()       const noexcept { return m_pIconButtonGear[0].Get(); };
		auto GetIconButtonGearSelected()         const noexcept { return m_pIconButtonGear[1].Get(); };
		auto GetIconButtonGearPressed()          const noexcept { return m_pIconButtonGear[2].Get(); };
		auto GetIconButtonArrowNoSelected()      const noexcept { return m_pIconButtonArrow[0].Get(); };
		auto GetIconButtonArrowSelected()        const noexcept { return m_pIconButtonArrow[1].Get(); };
		auto GetIconButtonArrowPressed()         const noexcept { return m_pIconButtonArrow[2].Get(); };
		auto GetIconButtonFlipSelected()         const noexcept { return m_pIconButtonFlip[0].Get(); };
		auto GetIconButtonFlipPressed()          const noexcept { return m_pIconButtonFlip[1].Get(); };


		// A hash function used to hash a pair of any kind
		struct HashPair {
			template <class T1, class T2>
			size_t operator()(const std::pair<T1, T2>& p) const
			{
				// Hash the first element
				size_t hash1 = std::hash<T1>{}(p.first);
				// Hash the second element
				size_t hash2 = std::hash<T2>{}(p.second);
				// Combine the two hash values
				return hash1
					^ (hash2 + 0x9e3779b9 + (hash1 << 6)
						+ (hash1 >> 2));
			}
		};

		D2D1_POINT_2F RotationOfPointAAroundPointB2D(float aX, float aY, float bX, float bY, float angle)
		{
			float degrees = (gPI * angle) / 180;
			float x = std::cos(degrees) * (aX - bX) - std::sin(degrees) * (bY - aY) + bX;
			float y = std::sin(degrees) * (aX - bX) + std::cos(degrees) * (aY - bY) + bY;

			return { x, y };
		}

		void VirtualKeyCodes();
		void Brushes(const std::shared_ptr<UI::D11DeviceResources>& pDevice);
		void FlipGradientStopCollection(const std::shared_ptr<UI::D11DeviceResources>& pDevice);
		void CreateTextFormat(const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory);
		void CreateIconBitmap(const std::shared_ptr<UI::D11DeviceResources>& pDevice);

		HRESULT CreateIconClose(const std::shared_ptr<UI::D11DeviceResources>& pDevice);
		HRESULT CreateIconMaximizeRestore(const std::shared_ptr<UI::D11DeviceResources>& pDevice);
		HRESULT CreateIconMinimize(const std::shared_ptr<UI::D11DeviceResources>& pDevice);
		HRESULT CreateIconGear(const std::shared_ptr<UI::D11DeviceResources>& pDevice);
		HRESULT CreateIconArrow(const std::shared_ptr<UI::D11DeviceResources>& pDevice);
		HRESULT CreateIconFlip(const std::shared_ptr<UI::D11DeviceResources>& pDevice);

	private:
		std::unordered_map<std::wstring, RECT> m_cachingRectMap;

		D2D1_PIXEL_FORMAT m_pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);

		std::unordered_map<std::pair<WPARAM, bool>, std::wstring, HashPair> m_virtualKey{};
		
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pInternalBacking;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pExternalBacking;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pIntermediateOutline;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pTitle;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pShadow;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pPopUp;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pPopUpFrame;

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pFieldBackgroundA;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pFieldBackgroundB;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pFieldFrameNoActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pFieldFrameActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pItemSelected;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pTextDClickSelection;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pTextSelection;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pText;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pCaret;

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonNoActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonPreActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonMinimize;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonMinimizeOutline;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonRestore;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonRestoreOutline;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonClose;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonCloseOutline;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonNoActiveNF;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonPreActiveNF;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonActiveNF;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonNoActivePopUp;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonPreActivePopUp;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonActivePopUp;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonTxtNoActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonTxtNFNoActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonTxtPreActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonTxtActive;	
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonFrameOutlinerNoActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonFrameOutlinerPreActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonFrameOutlinerActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonNFrameOutlinerNoActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonNFrameOutlinerPreActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonNFrameOutlinerActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonPFrameOutlinerNoActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonPFrameOutlinerPreActive;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pButtonPFrameOutlinerActive;

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pFlipAreaEllipse;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pFlipAreaTriangle;
		Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> m_pFlipGradientStops;

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pBorder;

		Microsoft::WRL::ComPtr<IDWriteTextFormat> m_pTitleTextFormat;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> m_pLabelTextFormat;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> m_pFieldTextFormat;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> m_pButtonTextFormat;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> m_pButtonPopUpTextFormat;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> m_pButtonPopUpExtraTextFormat;

		Microsoft::WRL::ComPtr<ID2D1Bitmap> m_pIconButtonClose;
		Microsoft::WRL::ComPtr<ID2D1Bitmap> m_pIconButtonRestore;
		Microsoft::WRL::ComPtr<ID2D1Bitmap> m_pIconButtonMinimize;
		Microsoft::WRL::ComPtr<ID2D1Bitmap> m_pIconButtonGear[3];
		Microsoft::WRL::ComPtr<ID2D1Bitmap> m_pIconButtonArrow[3];
		Microsoft::WRL::ComPtr<ID2D1Bitmap> m_pIconButtonFlip[2];
	};
}

#endif // !CACHING_CLASS_H
