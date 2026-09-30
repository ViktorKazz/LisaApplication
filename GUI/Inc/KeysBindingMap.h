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

#ifndef KEYS_FOR_THE_BINDING_MAP
#define KEYS_FOR_THE_BINDING_MAP

#include <wtypes.h>

namespace LisaApp
{
	enum class wnd_keys : UINT
	{
		none = 0,
		polyPrimitives,
		lights,
		cameras,
		allByType,
		components,
		testMore1,
		testMore2,
		feedback,
		sphere,
		geoSphere,
		cube,
		cylinder,
		cone,
		torus,
		plane,
		about
	};
	
	enum class cmd_keys : UINT
	{
		none = 0,
		sphere,
		geoSphere,
		cube,
		cylinder,
		cone,
		torus,
		plane,

		sphereRadius,
		sphereSubdivAxis,
		sphereSubdivHeight,

		geoSphereRadius,
		geoSphereSubdiv,

		cubeWidth,
		cubeHeight,
		cubeDepth,
		cubeSubdivWidth,
		cubeSubdivHeight,
		cubeSubdivDepth,

		cylinderRadius,
		cylinderHeight,
		cylinderSubdivAxis,
		cylinderSubdivHeight,
		cylinderSubdivCaps,

		coneRadius,
		coneHeight,
		coneSubdivAxis,
		coneSubdivHeight,
		coneSubdivCaps,

		torusRadius,
		torusSectionRadius,
		torusSubdivAxis,
		torusSubdivHeight,

		planeWidth,
		planeDepth,
		planeSubdivWidth,
		planeSubdivDepth
	};
}

#endif // !KEYS_FOR_THE_BINDING_MAP