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

#ifndef PICKING_CLASS_H
#define PICKING_CLASS_H

#include <unordered_map>
#include <memory>
#include <ranges>
#include <DirectXMath.h>
#include "PolygonPrimitives.h"
#include "Globals.h"
#include <HelperMath.h>

namespace LisaApp
{
    class Picking final
    {
    public:
        Picking() = default;
        ~Picking() = default;

        // Accessors.

        template<typename Self>
        std::uint32_t& GetMode(this Self&& self)                             noexcept { return self.m_mode; }
        // first = m_objectID;
        // second = m_instancesID;
        template<typename Self>
        std::pair<std::uint32_t, std::uint32_t>& GetLastID(this Self&& self) noexcept { return self.m_objectAndInstancesID; }
        bool GetHit()                                                  const noexcept { return m_hit; }

        void SetMode(std::uint32_t mode)                                     noexcept { m_mode = mode; }
        void SetLastID(std::pair<std::uint32_t, std::uint32_t> lastID)       noexcept { m_objectAndInstancesID = lastID; }

        float DistanceToWorld(const DirectX::XMFLOAT4X4& world, float dist) const;
        void PickingMessage(const std::wstring& objectName, size_t instanceNumber);
        void SelectMesh(const SceneObjects& sceneObjects, RefOnRender& refWrapOnRender, bool hasShift, bool objectInsteadOfComponent);
        void DeselectMesh(RefOnRender& refWrapOnRender, bool hasShift);
        void ClearRefTriangleContainer(RefOnRender& refWrapOnRender) const;
        void ClearRefEdgeContainer(RefOnRender& refWrapOnRender) const;
        void ClearRefVertexContainer(RefOnRender& refWrapOnRender) const;

        std::wstring CreateKey(std::int32_t pickedTriangle, std::uint32_t index0, std::uint32_t index1) const;

        void SelectTriangleComponent(RefOnRender& refWrapOnRender, bool hasShift) const;
        void SelectEdgeComponent(RefOnRender& refWrapOnRender, bool hasShift) const;
        void SelectVertexComponent(RefOnRender& refWrapOnRender, bool hasShift) const;

        void CalculatingIntersectionWithTriangle(
            const DirectX::XMFLOAT4X4& world,
            const DirectX::XMVECTOR& v0,
            const DirectX::XMVECTOR& v1,
            const DirectX::XMVECTOR& v2,
            std::uint32_t objectID,
            std::uint32_t instancesID,
            float& Dist,
            bool& hit
        );

        void CalculatingIntersectionWithTriangle(
            const DirectX::XMFLOAT4X4& world,
            const DirectX::XMVECTOR& v0,
            const DirectX::XMVECTOR& v1,
            const DirectX::XMVECTOR& v2,
            std::uint32_t triangleNumber,
            std::uint32_t objectID,
            std::uint32_t instancesID,
            float& Dist,
            bool& hit
        );

        void CalculatingIntersectionWithLine(
            const DirectX::XMFLOAT4X4& world,
            const DirectX::XMVECTOR& v0,
            const DirectX::XMVECTOR& v1,
            std::uint32_t objectID,
            std::uint32_t instancesID,
            float threshold,
            float& Dist,
            bool& hit
        );

        void CalculatingIntersectionWithLine(
            const DirectX::XMFLOAT4X4& world,
            const DirectX::XMVECTOR& v0,
            const DirectX::XMVECTOR& v1,
            std::uint32_t i0,
            std::uint32_t i1,
            std::uint32_t triangleNumber,
            std::uint32_t edgeNumber,
            std::uint32_t objectID,
            std::uint32_t instancesID,
            float threshold,
            float& Dist,
            bool& hit
        );

        void CalculatingIntersectionWithPoint(
            const DirectX::XMFLOAT4X4& world,
            const DirectX::XMVECTOR& v0,
            std::uint32_t objectID,
            std::uint32_t instancesID,
            float threshold,
            float& Dist,
            bool& hit
        );

        void CalculatingIntersectionWithPoint(
            const DirectX::XMFLOAT4X4& world,
            const DirectX::XMVECTOR& v0,
            std::uint32_t i0,
            std::uint32_t triangleNumber,
            std::uint32_t vertexNumber,
            std::uint32_t objectID,
            std::uint32_t instancesID,
            float threshold,
            float& Dist,
            bool& hit
        );

        template<typename T>
        void CalculatingIntersections(const Item::OnRender& onRender, std::uint32_t instancesID, float& Dist)
        {
            using namespace LisaApp::HelperMath;
            using namespace LisaApp::Global;
            using namespace DirectX;

            const auto& vertices = (T*)onRender.Geo->VertexBufferCPU->GetBufferPointer();
            const auto& indices = (std::uint32_t*)onRender.Geo->IndexBufferCPU->GetBufferPointer();

            std::uint32_t componentCount{};
            std::uint32_t arrSize{};

            auto const calculations = [&]()
                {
                    componentCount = onRender.IndexCount / arrSize;

                    for (std::uint32_t i = 0; i < componentCount; ++i)
                    {
                        // Indices for this triangle.

                        std::vector<std::uint32_t> ind;
                        ind.reserve(arrSize);

                        for (std::uint32_t j = 0; j < arrSize; j++)
                            ind.push_back(indices[i * arrSize + j]);

                        // Vertices for this triangle.

                        std::vector<XMVECTOR> v;
                        v.reserve(arrSize);

                        for (std::uint32_t j = 0; j < arrSize; j++)
                            v.push_back(XMLoadFloat3(&vertices[ind[j]].position));

                        XMFLOAT4X4 w = onRender.Instances[instancesID].World;

                        // We have to iterate over all the triangles in order to find the nearest intersection.
                        float t{ 0.0f };

                        if (m_mode == selection::mesh)
                        {
                            if (onRender.PrimitiveType == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
                                CalculatingIntersectionWithTriangle(w, v[0], v[1], v[2], onRender.ID, instancesID, t, m_hit);
                            else if (onRender.PrimitiveType == D3D_PRIMITIVE_TOPOLOGY_LINELIST)
                                CalculatingIntersectionWithLine(w, v[0], v[1], onRender.ID, instancesID, 0.001f, t, m_hit);
                            else if (onRender.PrimitiveType == D3D_PRIMITIVE_TOPOLOGY_POINTLIST)
                                CalculatingIntersectionWithPoint(w, v[0], onRender.ID, instancesID, 0.01f, t, m_hit);
                        }
                        else if (m_mode == selection::face)
                        {
                            CalculatingIntersectionWithTriangle(w, v[0], v[1], v[2], i, onRender.ID, instancesID, t, m_hit);
                        }
                        else if (m_mode == selection::edge)
                        {
                            if (onRender.PrimitiveType == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
                                for (size_t e{ 0 }; e < arrSize; e++)
                                {
                                    const auto x = (e == 2) ? 0 : (e + 1);
                                    CalculatingIntersectionWithLine(w, v[e], v[x], ind[e], ind[x], i, static_cast<std::uint32_t>(e), onRender.ID, instancesID, 0.001f, t, m_hit);
                                }

                            else if (onRender.PrimitiveType == D3D_PRIMITIVE_TOPOLOGY_LINELIST)
                                CalculatingIntersectionWithLine(w, v[0], v[1], ind[0], ind[1], i, 0, onRender.ID, instancesID, 0.001f, t, m_hit);
                        }
                        else if (m_mode == selection::vertex)
                        {
                            for (size_t e{ 0 }; e < arrSize; e++)
                                CalculatingIntersectionWithPoint(w, v[e], ind[e], i, static_cast<std::uint32_t>(e), onRender.ID, instancesID, 0.005f, t, m_hit);
                        }
                    }
                };


            if (onRender.PrimitiveType == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
            {
                if (onRender.Bounds.Intersects(m_rayOrigin, m_rayDir, Dist))
                {
                    Dist = DistanceToWorld(onRender.Instances[instancesID].World, Dist);

                    if (Dist < m_tmin)
                    {
                        arrSize = 3;
                        calculations();
                    }
                }
            }
            else if (onRender.PrimitiveType == D3D_PRIMITIVE_TOPOLOGY_LINELIST)
            {
                arrSize = 2;
                calculations();
            }
            else if (onRender.PrimitiveType == D3D_PRIMITIVE_TOPOLOGY_POINTLIST)
            {
                arrSize = 1;
                calculations();
            }
        }

        template<typename T>
        void ProcessingObjects(const RefInstVar& container, const Item::OnRender& onRender,
            const DirectX::XMMATRIX& invView, float viewX, float viewY)
        {
            auto const calculations = [&](const DirectX::XMFLOAT4X4& world, const Item::OnRender& orender, std::uint32_t instancesID)
                {
                    DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&world);
                    DirectX::XMVECTOR WW{ XMMatrixDeterminant(W) };
                    DirectX::XMMATRIX invWorld = DirectX::XMMatrixInverse(&WW, W);

                    // Tranform ray to view space of Mesh.
                    DirectX::XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

                    // Ray definition in view space.
                    m_rayOrigin = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
                    m_rayDir = DirectX::XMVectorSet(viewX, viewY, 1.0f, 0.0f);

                    m_rayOrigin = XMVector3TransformCoord(m_rayOrigin, toLocal);
                    m_rayDir = XMVector3TransformNormal(m_rayDir, toLocal);

                    // Make the ray direction unit length for the intersection tests.
                    m_rayDir = DirectX::XMVector3Normalize(m_rayDir);

                    // To store the distance to the bounding box
                    float bbmin{ 0.0f };

                    CalculatingIntersections<T>(orender, instancesID, bbmin);
                };


            if (std::holds_alternative<std::vector<Constants::InstanceData>>(container))
            {
                auto& type = std::get<std::vector<Constants::InstanceData>>(container);

                for (auto&& [iter, instance] : type | std::views::enumerate)
                {
                    calculations(instance.World, onRender, static_cast<uint32_t>(iter));
                }
            }
            else if (std::holds_alternative<RefOnRender>(container))
            {
                auto& type = std::get<RefOnRender>(container);

                for (const auto& o : type)
                {
                    auto& [oID, instID] = o.first;
                    auto& orender = o.second.front().get();

                    calculations(orender.Instances[instID].World, orender, instID);
                }
            }
        }

        template<typename T>
        void PrePick(const DirectX::XMMATRIX& getViev, const DirectX::XMFLOAT4X4& Proj4x4f,
            std::int32_t sx, std::int32_t sy, std::int32_t width, std::int32_t height, RefOnRender& refWrapOnRender)
        {
            using namespace LisaApp::HelperMath;
            using namespace LisaApp::Global;

            m_selectedIndex[0] = 0u; m_selectedIndex[1] = 0u; m_selectedIndex[2] = 0u;

            m_hit = false;

            DirectX::XMFLOAT4X4 P = Proj4x4f;

            // Compute picking ray in view space.
            float vx = (+2.0f * sx / width - 1.0f) / P(0, 0);
            float vy = (-2.0f * sy / height + 1.0f) / P(1, 1);

            DirectX::XMMATRIX V = getViev;
            DirectX::XMVECTOR VV{ XMMatrixDeterminant(V) };
            DirectX::XMMATRIX invView = XMMatrixInverse(&VV, V);

            // Find the nearest ray/triangle intersection.
            m_tmin = FLT_MAX;


            ProcessingObjects<T>(refWrapOnRender, {}, invView, vx, vy);

            if (!IsLockForUpdate)
            {
                if (refWrapOnRender.contains({ m_objectID, m_instancesID }))
                {
                    auto& primitives = refWrapOnRender.at({ m_objectID, m_instancesID }).back().get();

                    // To remove the color of the component of the previous object
                    // after the cursor has moved to another object, execute the following code.

                    if (!m_refInstances.contains({ m_objectID, m_instancesID }))
                    {
                        if (!m_refInstances.empty())
                        {
                            const auto& it = m_refInstances.begin();
                            it->second.get().DrawTriangle = 0;
                            m_refInstances.clear();
                        }
                        m_refInstances.insert({ { m_objectID, m_instancesID }, primitives.Instances[m_instancesID] });
                    }

                    primitives.Instances[m_instancesID].DrawTriangle = m_hit ? 1 : 0;
                    primitives.Instances[m_instancesID].TriangleNumber = m_pickedTriangle;

                    if (m_mode == selection::edge)
                        primitives.Instances[m_instancesID].ComponentTriangle = m_pickedEdge;
                    
                    else if (m_mode == selection::vertex)
                        primitives.Instances[m_instancesID].ComponentTriangle = m_pickedVertex;                    
                }
            }

            // The idea behind the following "if" branches is that the 
            // if condition (refWrapOnRender.contains({ m_objectID, m_instancesID })) 
            // is executed only once after the cursor leaves the primitive.

            if (!m_hit)
                IsLockForUpdate = true;
            else
                IsLockForUpdate = false;
        }

        template<typename T>
        bool AdditionalSelectionCheck(
            RefOnRender& refWrapOnRender,
            const SceneObjects& sceneObjects, 
            const DirectX::XMMATRIX& invView,
            float vx,
            float vy
            )
        {
            using namespace LisaApp::Global;

            uint32_t buffer{ m_mode };
            m_mode = selection::mesh;

            // Check the link container first.

            for (const auto& ref : refWrapOnRender)
            {
                auto& [objectID, instancesID] = ref.first;
                auto& primitives = ref.second.front().get();

                ProcessingObjects<T>(primitives.Instances, primitives, invView, vx, vy);
            }

            // If the component was not selected, but the ray intersected the object itself.
            // In this case, there is no need to do anything.

            if (m_hit)
            {
                m_mode = buffer;
                
                // We must return m_hit with its original value to 
                // avoid false positives in the component selection functions.
                m_hit = false;
                return false;
            }
            else
            {
                for (const auto& so : sceneObjects)
                {
                    const auto& [ID, primitives] = so;

                    ProcessingObjects<T>(primitives->OnRender[0].Instances, primitives->OnRender[0], invView, vx, vy);
                }
            }

            // If not, then the mouse click occurred on an empty space.
            if (!m_hit)
            {
                m_mode = buffer;
                return false;
            }

            // Another object was selected, meaning the reference container must be cleared. 
            // Containers storing data about the selected components will not be cleared.
            if (m_hit)
            {
                return true;
            }

            return false;
        }

        template<typename T>
        bool Pick(
            RefOnRender& refWrapOnRender,
            const SceneObjects& sceneObjects,
            const DirectX::XMMATRIX& getViev,
            const DirectX::XMFLOAT4X4& Proj4x4f,
            std::int32_t sx,
            std::int32_t sy,
            std::int32_t width,
            std::int32_t height,
            bool hasShift
        )
        {
            using namespace LisaApp::HelperMath;
            using namespace LisaApp::Global;
            using namespace DirectX;

            bool objectInsteadOfComponent{ false };

            // Find the nearest ray/triangle intersection.
            m_tmin = FLT_MAX;

            DirectX::XMFLOAT4X4 P = Proj4x4f;

            float scaleFactor{ 2.0f };

            // Compute picking ray in view space.
            float vx = (scaleFactor * sx / width - 1.0f) / P(0, 0);
            float vy = (-scaleFactor * sy / height + 1.0f) / P(1, 1);

            DirectX::XMMATRIX V = getViev;
            DirectX::XMVECTOR VV{ XMMatrixDeterminant(V) };
            DirectX::XMMATRIX invView = XMMatrixInverse(&VV, V);

            if (m_mode == selection::mesh)
            {
                m_objectID = 0;
                m_instancesID = 0;
                m_hit = false;

                for (const auto& so : sceneObjects)
                {
                    const auto& [ID, primitives] = so;

                    ProcessingObjects<T>(primitives->OnRender[0].Instances, primitives->OnRender[0], invView, vx, vy);
                }
            }
            // Additionally, we will check whether the mouse click was on an empty space 
            // or on an object that is not in the link container.

            if (!m_hit && (m_mode == selection::face || m_mode == selection::edge || m_mode == selection::vertex))
            {
                objectInsteadOfComponent = AdditionalSelectionCheck<T>(refWrapOnRender, sceneObjects, invView, vx, vy);
            }


            if (m_mode == selection::mesh)
            {
                // If the ray intersects one or more bounding boxes, we find the one that is closer to the camera.
                // Then we highlight the found object with color.

                if (m_objectID || m_instancesID)
                {
                    SelectMesh(sceneObjects, refWrapOnRender, hasShift, objectInsteadOfComponent);
                }
                else if (!m_objectID && !m_instancesID)
                {
                    DeselectMesh(refWrapOnRender, hasShift);
                }
            }
            else if (m_mode == selection::face)
            {
                SelectTriangleComponent(refWrapOnRender, hasShift);
            }
            else if (m_mode == selection::edge)
            {
                SelectEdgeComponent(refWrapOnRender, hasShift);                
            }
            else if (m_mode == selection::vertex)
            {
                SelectVertexComponent(refWrapOnRender, hasShift);
            }

            return objectInsteadOfComponent;
        }

    private:
        DirectX::XMVECTOR m_rayOrigin = DirectX::XMVectorZero();
        DirectX::XMVECTOR m_rayDir = DirectX::XMVectorZero();

        // Component selection mode.
        // Select off = 0, mesh = 1, face = 2, edge = 3, vertex = 4, already allocated = 5.
        std::uint32_t m_mode{ 1 };

        std::uint32_t m_objectID{};
        std::uint32_t m_instancesID{};
        std::pair<std::uint32_t, std::uint32_t> m_objectAndInstancesID{};

        std::uint32_t m_selectedIndex[3]{};

        std::uint32_t m_pickedTriangle{};
        std::uint32_t m_pickedEdge{};
        std::uint32_t m_pickedVertex{};

        // If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
        // so do the ray/triangle tests.
        //
        // If we did not hit the bounding box, then it is impossible that we hit 
        // the Mesh, so do not waste effort doing ray/triangle tests.
        float m_tmin{ 0.0f };

        // Required for additional verification.
        bool m_hit{ false };

        std::unordered_map<
            std::pair<std::uint32_t, std::uint32_t>, 
            std::reference_wrapper<Constants::InstanceData>,
            HashPair
        > m_refInstances;

        bool IsLockForUpdate{};
    };
}

#endif // !PICKING_CLASS_H
