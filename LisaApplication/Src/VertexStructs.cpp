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

#include "VertexStructs.h"

//--------------------------------------------------------------------------------------
// Vertex struct holding position and Size information.
const D3D12_INPUT_ELEMENT_DESC VertexStructs::VertexPositionColor::InputElements[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

static_assert(sizeof(VertexStructs::VertexPositionColor) == 28, "Vertex struct/layout mismatch");

const D3D12_INPUT_LAYOUT_DESC VertexStructs::VertexPositionColor::InputLayout =
{
    VertexStructs::VertexPositionColor::InputElements,
    VertexStructs::VertexPositionColor::InputElementCount
};

// Vertex struct holding position, normal vector, and texture mapping information.
const D3D12_INPUT_ELEMENT_DESC VertexStructs::VertexPositionNormalTexture::InputElements[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

static_assert(sizeof(VertexStructs::VertexPositionNormalTexture) == 32, "Vertex struct/layout mismatch");

const D3D12_INPUT_LAYOUT_DESC VertexStructs::VertexPositionNormalTexture::InputLayout =
{
    VertexStructs::VertexPositionNormalTexture::InputElements,
    VertexStructs::VertexPositionNormalTexture::InputElementCount
};

// Vertex struct holding position, normal vector, texture mapping and TangentU information.
const D3D12_INPUT_ELEMENT_DESC VertexStructs::VertexPositionNormalTextureTangentU::InputElements[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

static_assert(sizeof(VertexStructs::VertexPositionNormalTextureTangentU) == 44, "Vertex struct/layout mismatch");

const D3D12_INPUT_LAYOUT_DESC VertexStructs::VertexPositionNormalTextureTangentU::InputLayout =
{
    VertexStructs::VertexPositionNormalTextureTangentU::InputElements,
    VertexStructs::VertexPositionNormalTextureTangentU::InputElementCount
};

// Vertex struct holding position, normal vector, texture mapping, tangentU and barycentric information.
//const D3D12_INPUT_ELEMENT_DESC VertexStructs::VertexPositionNormalTextureTangentUBar::InputElements[] =
//{
//    { "POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//    { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//    { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//    { "TANGENT",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//    { "BARYCENTRIC", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//};
//
//static_assert(sizeof(VertexStructs::VertexPositionNormalTextureTangentUBar) == 56, "Vertex struct/layout mismatch");
//
//const D3D12_INPUT_LAYOUT_DESC VertexStructs::VertexPositionNormalTextureTangentUBar::InputLayout =
//{
//    VertexStructs::VertexPositionNormalTextureTangentUBar::InputElements,
//    VertexStructs::VertexPositionNormalTextureTangentUBar::InputElementCount
//};