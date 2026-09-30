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

#ifndef COLOR_UI_H
#define COLOR_UI_H

#include <d2d1.h>
#include <wrl.h> 

namespace UI
{
    namespace Palette
    {
		constexpr UINT Amour{ 0xFDEFF5 };
		constexpr UINT Amour2{ 0xFEE3EE };
		constexpr UINT LavenderBlush1{ 0xFEF6F9 };
		constexpr UINT LavenderBlush2{ 0xFDF4F6 };
		constexpr UINT HippiePink{ 0xA94F63 };
		constexpr UINT Carissma1{ 0xDC8095 };
		constexpr UINT Carissma2{ 0xED8598 };
		constexpr UINT RiceFlower{ 0xF3FCD8 };
		constexpr UINT Panache{ 0xEEFFE3 };
		constexpr UINT FreeSpeechBlue{ 0x4264C2 };
		constexpr UINT Perano{ 0xB3C0E5 };
		constexpr UINT CatalinaBlue{ 0x233567 };
		constexpr UINT NightRider{ 0x353535 };
		constexpr UINT Black{ 0x000000 };
		constexpr UINT Endeavour{ 0x315B96 };
		constexpr UINT Magnolia{ 0xF6F1FF };
		constexpr UINT WhiteNectar{ 0xF8F5D4 };
		constexpr UINT Lochmara{ 0x1F75A4 };
		constexpr UINT Selago{ 0xFDFCFD };
		constexpr UINT UnknownName{ 0xBF80D6 };
		constexpr UINT BeautyBush{ 0xF1BABA };
		constexpr UINT SlightlyDesaturatedBlue{ 0x6C91BB };
		constexpr UINT BrandyPunch{ 0xBD7F7F };
		constexpr UINT CarouselPink{ 0xFADEE3 };
		constexpr UINT Bridesmaid{ 0xFBE6E1 };
		constexpr UINT CabSav{ 0x4A262E };
		constexpr UINT Blobfish{ 0xFFC1CC };
		constexpr UINT Roman{ 0xDF6666 };
		constexpr UINT CopperRust{ 0x94544C };
		constexpr UINT OrangeDawn{ 0xFC5753 };
		constexpr UINT DeepYellowPink{ 0xE54247 };
		constexpr UINT ShinyOrange{ 0xFDBB40 };
		constexpr UINT YellowGold{ 0xE1A332 };
		constexpr UINT LimeGreen{ 0x32C747 };
		constexpr UINT EmeraldGreen{ 0x28A935 };
		constexpr UINT Pearl{ 0xdbbdb5 };
	};

	namespace Colors
	{
		constexpr UINT InternalBacking{ Palette::SlightlyDesaturatedBlue };
		constexpr UINT ExternalBacking{ Palette::Bridesmaid };
		constexpr UINT IntermediateOutline{ Palette::BrandyPunch };
		constexpr UINT Title{ Palette::Amour };
		constexpr UINT Shadow{ Palette::NightRider };
		constexpr UINT PopUp{ Palette::LavenderBlush2 };
		constexpr UINT PopUpFrame{ Palette::Blobfish };

		constexpr UINT FieldBackgroundA{ Palette::Selago };
		constexpr UINT FieldBackgroundB{ Palette::WhiteNectar };
		constexpr UINT FieldFrameNoActive{ Palette::Carissma1 };
		constexpr UINT FieldFrameActive{ Palette::FreeSpeechBlue };
		constexpr UINT ItemSelected{ Palette::UnknownName };
		constexpr UINT TextDClickSelection{ Palette::Perano };
		constexpr UINT TextSelection{ Palette::Perano };
		constexpr UINT Text{ Palette::CabSav };
		constexpr UINT Caret{ Palette::Black };

		constexpr UINT StatusBarIcon{ Palette::Carissma1 };
		constexpr UINT StatusBarText{ Palette::Carissma1 };

		constexpr UINT ButtonNoActive{ Palette::CarouselPink };
		constexpr UINT ButtonPreActive{ Palette::LavenderBlush2 };
		constexpr UINT ButtonActive{ Palette::Carissma2 };
		constexpr UINT ButtonMinimize{ Palette::OrangeDawn };
		constexpr UINT ButtonMinimizeOutline{ Palette::DeepYellowPink };
		constexpr UINT ButtonRestore{ Palette::ShinyOrange };
		constexpr UINT ButtonRestoreOutline{ Palette::YellowGold };
		constexpr UINT ButtonClose{ Palette::LimeGreen };
		constexpr UINT ButtonCloseOutline{ Palette::EmeraldGreen };		
		constexpr UINT ButtonNoActiveNF{ Palette::SlightlyDesaturatedBlue };
		constexpr UINT ButtonPreActiveNF{ Palette::Bridesmaid };
		constexpr UINT ButtonActiveNF{ Palette::Bridesmaid };
		constexpr UINT ButtonNoActivePopUp{ Palette::LavenderBlush2 };
		constexpr UINT ButtonPreActivePopUp{ Palette::CarouselPink };
		constexpr UINT ButtonActivePopUp{ Palette::Carissma2 };
		constexpr UINT ButtonTxtNoActive{ Palette::CabSav };
		constexpr UINT ButtonTxtNFNoActive{ Palette::Amour };
		constexpr UINT ButtonTxtPreActive{ Palette::CopperRust };
		constexpr UINT ButtonTxtActive{ Palette::Amour };
		constexpr UINT ButtonFrameOutlinerNoActive{ Palette::BrandyPunch };
		constexpr UINT ButtonFrameOutlinerPreActive{ Palette::BrandyPunch };
		constexpr UINT ButtonFrameOutlinerActive{ Palette::Carissma2 };
		constexpr UINT ButtonNFrameOutlinerNoActive{ Palette::SlightlyDesaturatedBlue };
		constexpr UINT ButtonNFrameOutlinerPreActive{ Palette::Bridesmaid };
		constexpr UINT ButtonNFrameOutlinerActive{ Palette::Bridesmaid };
		constexpr UINT ButtonPFrameOutlinerNoActive{ Palette::LavenderBlush2 };
		constexpr UINT ButtonPFrameOutlinerPreActive{ Palette::CarouselPink };
		constexpr UINT ButtonPFrameOutlinerActive{ Palette::Carissma2 };
		
		constexpr UINT FlipAreaEllipse{ Palette::LavenderBlush2 };
		constexpr UINT FlipAreaTriangle{ Palette::BeautyBush };

		constexpr UINT Primitives2DBase{ Palette::Carissma1 };

		constexpr UINT Border{ Palette::Pearl };
	};
}

#endif // !COLOR_UI_H