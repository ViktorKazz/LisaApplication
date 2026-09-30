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

#ifndef SCENE_OBJECTS_CLASS_H
#define SCENE_OBJECTS_CLASS_H

#include "pch.h"
#include "Picking.h"
#include "PipelineState.h"
#include "UploadShaders.h"

namespace LisaApp
{
    struct PrimitivesData
    {
        float Radius{};

        float Width{};
        float Height{};
        float Depth{};

        UINT Subdivisions{};

        UINT SubdivisionsWidth{};
        UINT SubdivisionsHeight{};
        UINT SubdivisionsDepth{};
        UINT SubdivisionsAxis{};
        UINT SubdivisionsCaps{};

        float SectionRadius{};

        bool RhCoords{ true };
    };
    
    struct ObjectNumbering { std::uint32_t ID; std::uint32_t Number; std::uint32_t Quantity; };

    class CreatingPrimitives
    {
    public:
        CreatingPrimitives() = default;
        ~CreatingPrimitives() = default;

        // Accessors.

        Picking* GetPicking()                                          const noexcept { return m_picking.get(); }
        bool IsSelect()                                                const noexcept { return !m_refWrapOnRender.empty(); }
        // first = m_objectID;
        // second = m_instancesID;
        template<typename Self>
        std::pair<std::uint32_t, std::uint32_t>& GetLastID(this Self&& self) noexcept { return self.m_objectAndInstancesID; }       
        template<typename Self>
        RefOnRender& GetRefWrapOnRender(this Self&& self)                    noexcept { return self.m_refWrapOnRender; }

        // Update the container containing adjacent indexes. 
        // When calling the function m_updateContiguousIndexsContainer = true.
        void UpdateContiguousIndexsContainer() { m_updateContiguousIndexsContainer = true; }

        void Create(
            _In_ ID3D12Device3* device,
            _In_ ID3D12GraphicsCommandList* commandList,
            UINT primitives,
            PrimitivesData pd,
            INT materials
        );

        void CreatePso(
            _In_ ID3D12Device3* device,
            _In_ ID3D12RootSignature* rootSignature,
            DXGI_FORMAT ssaoNormalMap,
            DXGI_FORMAT depthBuffer,
            std::uint32_t sampleCount,
            std::uint32_t sampleQuality
        );

        void CreateInstances(UINT matIndex);

        void Delete();

        void EditingAttributes(
            const IPivot::Attributes& attributes, 
            const IPivot::Axis& activeAxis, 
            const LisaApp::Global::PivotMode& pivotMode
        );

        void EditingComponents(
            const IPivot::Attributes& attributes,
            const IPivot::Axis& activeAxis,
            const LisaApp::Global::PivotMode& pivotMode,
            std::uint32_t selectMode
        );

        bool PreparingEditComponents(std::uint32_t mode, const std::pair<std::uint32_t, std::uint32_t>& lastID = {});

        void Draw(_In_ ID3D12GraphicsCommandList* commandList);
        void DrawNormals(_In_ ID3D12GraphicsCommandList* commandList);
        void DrawShadow(_In_ ID3D12GraphicsCommandList* commandList);

        void UpdateCB(
            const DirectX::XMMATRIX& getViev,
            const DirectX::BoundingFrustum& camFrustum,
            bool frustumCullingEnabled
        );

        void PrePick(
            const DirectX::XMMATRIX& getViev,
            const DirectX::XMFLOAT4X4& Proj4x4f,
            std::int32_t sx,
            std::int32_t sy,
            INT width,
            INT height
        );

        bool Pick(
            const DirectX::XMMATRIX& getViev,
            const DirectX::XMFLOAT4X4& Proj4x4f,
            std::int32_t sx,
            std::int32_t sy,
            INT width,
            INT height,
            bool hasShift
        );

    private:
        template<typename T>
        DirectX::BoundingBox NewBoundingBox(T* vertices, size_t iter)
        {
            using namespace DirectX;

            float minX{}, maxX{};
            float minY{}, maxY{};
            float minZ{}, maxZ{};

            for (size_t i = 0; i < m_rawIndexStorage[iter].size(); i++)
            {
                const std::uint32_t& ti = m_rawIndexStorage[iter][i];

                if (i == 0)
                {
                    minX = vertices[ti].position.x;
                    maxX = vertices[ti].position.x;

                    minY = vertices[ti].position.y;
                    maxY = vertices[ti].position.y;

                    minZ = vertices[ti].position.z;
                    maxZ = vertices[ti].position.z;
                }
                else
                {
                    minX = std::fmin(vertices[ti].position.x, minX);
                    maxX = std::fmax(vertices[ti].position.x, maxX);

                    minY = std::fmin(vertices[ti].position.y, minY);
                    maxY = std::fmax(vertices[ti].position.y, maxY);

                    minZ = std::fmin(vertices[ti].position.z, minZ);
                    maxZ = std::fmax(vertices[ti].position.z, maxZ);
                }
            }

            DirectX::XMVECTOR c = XMVectorScale(XMVectorAdd({ minX, minY, minZ }, { maxX, maxY, maxZ }), 0.5f);
            DirectX::XMVECTOR e = XMVectorScale(XMVectorSubtract({ maxX, maxY, maxZ }, { minX, minY, minZ }), 0.5f);

            DirectX::BoundingBox bounds;
            DirectX::XMStoreFloat3(&bounds.Center, c);
            DirectX::XMStoreFloat3(&bounds.Extents, e);

            return bounds;
        }

        std::uint32_t m_PSOMode{};
        std::uint32_t m_objectID{};
        std::uint32_t m_instancesID{};
        std::pair<std::uint32_t, std::uint32_t> m_objectAndInstancesID{};

        SceneObjects m_sceneObjects{};
        RefOnRender m_refWrapOnRender;

        std::unordered_map<std::wstring, ObjectNumbering> m_objectNumberingMap{};

        std::unique_ptr<Picking> m_picking = std::make_unique<Picking>();

        bool m_updateContiguousIndexsContainer{};
        std::vector<std::unordered_map<std::uint32_t, std::uint32_t>> m_contiguousIndexesStorage{};

        std::vector<std::vector<std::uint32_t>> m_rawIndexStorage{};

        UploadShaders m_shader;
        
        enum PSOMode : std::uint32_t
        {
            Opaque = 0,
            Wireframe,
            Edge,
            Triangle,
            Vertex,
            Shadow,
            Normals
        };

        PipelineState m_pso;
    };
}

#endif // !SCENE_OBJECTS_CLASS_H
