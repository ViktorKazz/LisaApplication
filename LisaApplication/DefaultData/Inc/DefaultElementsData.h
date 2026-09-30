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

#ifndef DEFAULT_ELEMENTS_DATA_H
#define DEFAULT_ELEMENTS_DATA_H

#include <wtypes.h>

namespace LisaApp
{
	namespace Default
	{
        struct SphereData
        {
            float Radius{ 1.0f };
            UINT SubdivisionsAxis{ 20 };
            UINT SubdivisionsHeight{ 20 };
        };

        struct GeoSphereData
        {
            float Radius{ 1.0f };
            UINT Subdivisions{ 3 }; 
            bool RhCoords{ true };
        };

        struct CubeData
        {
            float Width{ 1.0f };
            float Height{ 1.0f };
            float Depth{ 1.0f };
            UINT SubdivisionsWidth{ 1 };
            UINT SubdivisionsHeight{ 1 }; 
            UINT SubdivisionsDepth{ 1 };
        };

        struct CylinderData
        {
            float Radius{ 1.0f }; 
            float Height{ 2.0f }; 
            UINT SubdivisionsAxis{ 20 }; 
            UINT SubdivisionsHeight{ 1 }; 
            UINT SubdivisionsCaps{ 1 };
        };

        struct ConeData
        {
            float Radius{ 1.0f };
            float Height{ 2.0f }; 
            UINT SubdivisionsAxis{ 20 }; 
            UINT SubdivisionsHeight{ 1 }; 
            UINT SubdivisionsCaps{ 1 };
        };

        struct TorusData
        {
            float Radius{ 1.0f };
            float SectionRadius{ 1.0f }; 
            UINT SubdivisionsAxis{ 20 }; 
            UINT SubdivisionsHeight{ 20 }; 
            bool RhCoords{ true };
        };

        struct PlaneData
        {
            float Width{ 1.0f };
            float Depth{ 1.0f }; 
            UINT SubdivisionsWidth{ 10 }; 
            UINT SubdivisionsDepth{ 10 };
        };
	}
	
}

#endif // !DEFAULT_ELEMENTS_DATA_H
