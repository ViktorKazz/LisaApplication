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

#ifndef GLOBAL_LISA_APP_H
#define GLOBAL_LISA_APP_H

#include <variant>
#include "HelperStructs.h"
#include "VertexStructs.h"

namespace LisaApp
{
    // A hash function used to hash a pair of any kind
    struct HashPair {
        template <class T1, class T2>
        size_t operator()(const std::pair<T1, T2>& p) const
        {
            // Hash the first element.
            size_t hash1 = std::hash<T1>{}(p.first);
            // Hash the second element.
            size_t hash2 = std::hash<T2>{}(p.second);
            // Combine the two hash values.
            //return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
            size_t seed = hash1;
            seed ^= hash2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };

    struct HashTriple {
        template <class T1, class T2, class T3>
        size_t operator()(const std::tuple<T1, T2, T3>& t) const {
            // Hash the first element.
            size_t hash1 = std::hash<T1>{}(std::get<0>(t));
            // Hash the second element.
            size_t hash2 = std::hash<T2>{}(std::get<1>(t));
            // Hash the third element.
            size_t hash3 = std::hash<T3>{}(std::get<2>(t));

            // Initialize the seed with the first hash.
            size_t seed = hash1;
            // Mix the second hash.
            seed ^= hash2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            // Adding the third hash
            seed ^= hash3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed;
        }
    };

    inline void hashCombine(std::size_t& seed, std::size_t v) {
        seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template <typename... Args>
    size_t tupleHash(const std::tuple<Args...>& t) {
        std::size_t seed = 0;
        std::apply([&](const auto&... args) {
            (hashCombine(seed, std::hash<decltype(args)>{}(args)), ...);
            }, t);
        return seed;
    }

    using VertexColor = std::vector<VertexStructs::VertexPositionColor>;
    using VertexCollection = std::vector<VertexStructs::VertexPositionNormalTextureTangentU>;
    using IndexCollection = std::vector<uint32_t>;
    using ComponentCollection = std::unordered_multimap<std::uint32_t, std::vector<std::pair<std::uint32_t, std::uint32_t>>>;
    using RefOnRender = std::unordered_map<std::pair<std::uint32_t, std::uint32_t>, std::vector<std::reference_wrapper<Item::OnRender>>, HashPair>;
    using RefInstVar = std::variant<RefOnRender, std::vector<Constants::InstanceData>>;
	
    namespace Global
	{
        enum selection : std::uint32_t
        {
            off = 0,
            mesh,
            face,
            edge,
            vertex,
            alreadyAllocated
        };

        enum PRIMITIVES_MEMBERS : UINT
        {
            SUBDIVISIONS,
            WIDTH,
            HEIGHT,
            DEPTH,
            SUBDIVISIONS_WIDTH,
            SUBDIVISIONS_HEIGHT,
            SUBDIVISIONS_DEPTH,
            RADIUS,
            SUBDIVISIONS_AXIS,
            SUBDIVISIONS_CAPS,
            SECTION_RADIUS,
            RH_COORDS
        };

        enum PRIMITIVES : UINT
        {
            SPHERE = 0,
            GEO_SPHERE,
            CUBE,
            CYLINDER,
            CONE,
            TORUS,
            PLANE,
            CIRCLE
        };

        enum MATERIAL : UINT
        {
            BASE = 0
        };

        enum pivot_components : std::uint32_t
        {
            center_frame = 1001,
            cone,
            cone_x,
            cone_y,
            cone_z,
            line,
            line_x,
            line_y,
            line_z,
            plane,
            plane_x,
            plane_y,
            plane_z,
            cube,
            cube_x,
            cube_y,
            cube_z,
            center_cube,
            aux_line,
            aux_line_x,
            aux_line_y,
            aux_line_z,
            outer_circle,
            inner_circle,
            circle,
            circle_x,
            circle_y,
            circle_z,
            sphere,
            aux_sphere_x,
            aux_sphere_y,
            aux_sphere_z,
            aux_sphere_xyz,
            aux_line_start,
            aux_line_end,
            aux_triangle,
            aux_plane
        };

        enum class PivotMode : std::uint32_t
        {
            Off = 0,
            Translate,
            Rotate,
            Scale
        };

        enum class PivotColorMode : std::uint32_t
        {
            Default = 0,
            PreSelect,
            Select
        };

        constexpr float gDefaultPivotRadius{ 7.5f };
	}
}

#endif GLOBAL_LISA_APP_H