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

#include "CustomTextures.h"

// Generate a basic white texture.
std::vector<UINT8> CustomTextures::Base()
{
	const UINT TextureWidth{ 256 };
	const UINT TextureHeight{ 256 };
	const UINT NumAlphaShades{ 256 };
	const UINT NumTextureColors{ 1 };
	const UINT TexturePixelSizeX = TextureWidth / NumAlphaShades;
	const UINT TexturePixelSizeY = TextureHeight / NumTextureColors;

	const UINT textureSize = (TextureWidth * sizeof(UINT)) * TextureHeight;

	std::vector<UINT8> data(textureSize);
	UINT8* pData = &data[0];

	for (UINT a = 0; a < NumAlphaShades; ++a)
	{
		UINT start_x = a * TexturePixelSizeX;
		UINT end_x = start_x + TexturePixelSizeX;

		UINT start_y = TexturePixelSizeY * 0;
		UINT end_y = start_y + TexturePixelSizeY;
		for (UINT y = start_y; y < end_y; ++y)
		{
			for (UINT x = start_x; x < end_x; ++x)
			{
				UINT offset = (static_cast<unsigned long long>(y) * TextureWidth + x) * sizeof(UINT);
				pData[offset + 0] = (uint8_t)(1.0f * 255.0f);
				pData[offset + 1] = (uint8_t)(1.0f * 255.0f);
				pData[offset + 2] = (uint8_t)(1.0f * 255.0f);
				pData[offset + 3] = (uint8_t)(1.0f * 255.0f);
			}
		}
	}

	return data;
}

// Generate a simple black and white checkerboard texture.
std::vector<UINT8> CustomTextures::Checkerboard()
{
	const UINT TextureWidth{ 256 };
	const UINT TextureHeight{ 256 };
	const UINT TexturePixelSize = 4;              // The number of bytes used to represent a pixel in the texture.
	const UINT rowPitch = TextureWidth * TexturePixelSize;
	const UINT cellPitch = rowPitch >> 3;         // The width of a cell in the checkboard texture.
	const UINT cellHeight = TextureWidth >> 3;    // The height of a cell in the checkerboard texture.
	const UINT textureSize = rowPitch * TextureHeight;

	std::vector<UINT8> data(textureSize);
	UINT8* pData = &data[0];

	for (UINT n = 0; n < textureSize; n += TexturePixelSize)
	{
		UINT x = n % rowPitch;
		UINT y = n / rowPitch;
		UINT i = x / cellPitch;
		UINT j = y / cellHeight;

		if (i % 2 == j % 2)
		{
			pData[n] = 0x00;        // R
			pData[n + 1] = 0x00;    // G
			pData[n + 2] = 0x00;    // B
			pData[n + 3] = 0xff;    // A
		}
		else
		{
			pData[n] = 0xff;        // R
			pData[n + 1] = 0xff;    // G
			pData[n + 2] = 0xff;    // B
			pData[n + 3] = 0xff;    // A
		}
	}

	return data;
}