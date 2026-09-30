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

#include "Picking.h"

// There are two objects. One is enlarged and located behind the other.
// The beam passes through both objects. 
// The one that is zoomed in on will be selected, even though it is behind.
// To avoid this, the distance to the object (or its component)
// must be translated back from local space to world space.
float LisaApp::Picking::DistanceToWorld(const DirectX::XMFLOAT4X4& world, float dist) const
{
    using namespace DirectX;

    XMVECTOR rayOrigin = XMVector3TransformCoord(m_rayOrigin, XMLoadFloat4x4(&world));

    XMVECTOR rayDir = XMVector3TransformNormal(m_rayDir, XMLoadFloat4x4(&world));

    XMVECTOR d = XMVectorSet(dist, dist, dist, 0.0f);

    XMVECTOR intersection = XMVectorMultiplyAdd(d, rayDir, rayOrigin);

    XMVECTOR l = XMVector3Length(XMVectorSubtract(intersection, rayOrigin));

    return XMVectorGetX(l);
}

// Status bar messages.
void LisaApp::Picking::PickingMessage(const std::wstring& objectName, size_t instanceNumber)
{
    std::wstring select{};
    
    // Display the object name in the console...
    if (instanceNumber == 0)
        select = { L"select " + objectName + L";" };
    // ...or a instances of it.
    else
        select = { L"select " + objectName + L".instance[" + std::to_wstring(instanceNumber) + L"]" + L";" };

    OutputDebugString(select.c_str());
    OutputDebugString(L"\n");
}

void LisaApp::Picking::CalculatingIntersectionWithTriangle(
    const DirectX::XMFLOAT4X4& world,
    const DirectX::XMVECTOR& v0,
    const DirectX::XMVECTOR& v1,
    const DirectX::XMVECTOR& v2,
    std::uint32_t objectID, 
    std::uint32_t instancesID, 
    float& Dist, 
    bool& hit
)
{
    using namespace LisaApp::HelperMath;

    if (Intersects(m_rayOrigin, m_rayDir, v0, v1, v2, Dist))
    {
        Dist = DistanceToWorld(world, Dist);

        if (Dist < m_tmin)
        {
            // This is the new nearest picked triangle.
            m_tmin = Dist;
            
            m_objectID = objectID;
            m_instancesID = instancesID;

            hit = true;
        }
    }
}

void LisaApp::Picking::CalculatingIntersectionWithTriangle(
    const DirectX::XMFLOAT4X4& world,
    const DirectX::XMVECTOR& v0,
    const DirectX::XMVECTOR& v1,
    const DirectX::XMVECTOR& v2,
    std::uint32_t triangleNumber,
    std::uint32_t objectID,
    std::uint32_t instancesID,
    float& Dist,
    bool& hit
)
{
    using namespace LisaApp::HelperMath;

    if (Intersects(m_rayOrigin, m_rayDir, v0, v1, v2, Dist))
    {
        Dist = DistanceToWorld(world, Dist);

        if (Dist < m_tmin)
        {
            // This is the new nearest picked triangle.
            m_tmin = Dist;

            m_objectID = objectID;
            m_instancesID = instancesID;

            m_pickedTriangle = triangleNumber;

            hit = true;
        }
    }
}

void LisaApp::Picking::CalculatingIntersectionWithLine(
    const DirectX::XMFLOAT4X4& world,
    const DirectX::XMVECTOR& v0,
    const DirectX::XMVECTOR& v1,
    std::uint32_t objectID,
    std::uint32_t instancesID,
    float threshold,
    float& Dist,
    bool& hit
)
{
    using namespace LisaApp::HelperMath;

    if (RayIntersectsSegment(m_rayOrigin, m_rayDir, v0, v1, threshold, Dist))
    {
        Dist = DistanceToWorld(world, Dist);

        if (Dist < m_tmin)
        {
            // This is the new nearest picked triangle.
            m_tmin = Dist;

            m_objectID = objectID;
            m_instancesID = instancesID;

            hit = true;
        }
    }
}

void LisaApp::Picking::CalculatingIntersectionWithLine(
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
)
{
    using namespace LisaApp::HelperMath;

    if (RayIntersectsSegment(m_rayOrigin, m_rayDir, v0, v1, threshold, Dist))
    {
        Dist = DistanceToWorld(world, Dist);

        if (Dist < m_tmin)
        {
            // This is the new nearest picked triangle.
            m_tmin = Dist;

            m_objectID = objectID;
            m_instancesID = instancesID;

            m_pickedTriangle = triangleNumber;
            m_pickedEdge = edgeNumber; // Edge in triangle: 0, 1 or 2.

            m_selectedIndex[0] = i0;
            m_selectedIndex[1] = i1;

            hit = true;
        }
    }
}

void LisaApp::Picking::CalculatingIntersectionWithPoint(
    const DirectX::XMFLOAT4X4& world,
    const DirectX::XMVECTOR& v0,
    std::uint32_t objectID,
    std::uint32_t instancesID,
    float threshold,
    float& Dist,
    bool& hit
)
{
    using namespace LisaApp::HelperMath;

    if (IsPointNearRay(v0, m_rayOrigin, m_rayDir, threshold, Dist))
    {
        Dist = DistanceToWorld(world, Dist);

        if (Dist < m_tmin)
        {
            // This is the new nearest picked triangle.
            m_tmin = Dist;

            m_objectID = objectID;
            m_instancesID = instancesID;

            hit = true;
        }
    }
}

void LisaApp::Picking::CalculatingIntersectionWithPoint(
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
)
{
    using namespace LisaApp::HelperMath;

    if (IsPointNearRay(v0, m_rayOrigin, m_rayDir, threshold, Dist))
    {
        Dist = DistanceToWorld(world, Dist);

        if (Dist < m_tmin)
        {
            // This is the new nearest picked triangle.
            m_tmin = Dist;

            m_objectID = objectID;
            m_instancesID = instancesID;

            m_pickedTriangle = triangleNumber;
            m_pickedVertex = vertexNumber;

            m_selectedIndex[0] = i0;

            hit = true;
        }
    }
}

void LisaApp::Picking::SelectMesh(
    const SceneObjects& sceneObjects, 
    RefOnRender& refWrapOnRender, 
    bool hasShift, 
    bool objectInsteadOfComponent
)
{
    if (!hasShift || objectInsteadOfComponent)
    {
        for (auto& ref : refWrapOnRender)
        {
            auto& [objectID, instancesID] = ref.first;

            if (ref.second.size() == 1) // For objects that contain one layer.
                ref.second.front().get().Instances[instancesID].Mode = 0;
            else
            {
                // For objects that contain three layers. 
                // 3D primitives and other 3D objects.

                // Layer 2 is responsible for the model's wireframe.
                const auto& mesh = ref.second.begin() + 1;
                mesh->get().Instances[instancesID].Mode = 0;

                // The last third layer is responsible for the components.
                // If an object had components selected, 
                // then when selecting another object,
                // the displayed components of the previous objects must be hidden.
                const auto& components = ref.second.back();
                components.get().Instances[instancesID].HoldDataSize = 0;

                if (ref.second.front().get().Instances.size() == 1)
                    mesh->get().TurnOff();
                else
                    mesh->get().Instances[instancesID].Hide = 1;
            }
        }
        refWrapOnRender.clear();
    }


    if (refWrapOnRender.contains({ m_objectID, m_instancesID }))
    {
        const auto& ref = refWrapOnRender.at({ m_objectID, m_instancesID });
        
        // If several objects are selected and a click was made on an object 
        // that does not have a pivot or that is not the last selected one.
        // In this case, it needs to be removed from the link container.

        // Layer 2 is responsible for the model's wireframe.
        const auto& mesh = ref.begin() + 1;
        mesh->get().Instances[m_instancesID].Mode = 0;

        if (ref.front().get().Instances.size() == 1)
            mesh->get().TurnOff();
        else
            mesh->get().Instances[m_instancesID].Hide = 1;
        
        refWrapOnRender.erase({ m_objectID, m_instancesID });

        // If the selected object is the one on which the pivot is located.
        if (m_objectAndInstancesID.first == m_objectID && m_objectAndInstancesID.second == m_instancesID)
        {
            // Select the "last" object in the container.
            // 
            // The selection method isn't the best, but I couldn't find another one at this stage. 
            // The problem is that in an unordered map, 
            // it's impossible to access the last object, like in a vector.
            for (const auto& i : refWrapOnRender)
            {
                const auto& [objectID, instancesID] = i.first;

                m_objectAndInstancesID.first = objectID;
                m_objectAndInstancesID.second = instancesID;
            }

            const auto& [objectID, instancesID] = m_objectAndInstancesID;

            if (refWrapOnRender.contains({ objectID, instancesID }))
            {
                const auto& reff = refWrapOnRender.at({ objectID, instancesID });

                // Layer 2 is responsible for the model's wireframe.
                const auto& meshh = reff.begin() + 1;
                meshh->get().Instances[instancesID].Mode = 0;
            }
        }
    }
    else
    {
        // Change the frame color of the previous object, if such an object exists.

        const auto& [objectID, instancesID] = m_objectAndInstancesID;

        if (refWrapOnRender.contains({ objectID, instancesID }))
        {
            const auto& ref = refWrapOnRender.at({ objectID, instancesID });

            // Layer 2 is responsible for the model's wireframe.
            const auto& mesh = ref.begin() + 1;
            mesh->get().Instances[instancesID].Mode = Global::selection::alreadyAllocated;
        }

        m_objectAndInstancesID.first = m_objectID;
        m_objectAndInstancesID.second = m_instancesID;
        
        // Add the selected object to the container.

        refWrapOnRender.insert({ { m_objectID, m_instancesID }, {sceneObjects.at(m_objectID)->begin(), sceneObjects.at(m_objectID)->end()} });
        
        const auto& ref = refWrapOnRender.at({ m_objectID, m_instancesID });

        if (ref.size() == 1) // For objects that contain one layer.
            ref.front().get().Instances[instancesID].Mode = 1;
        else
        {
            // For objects that contain three layers. 
            // 3D primitives and other 3D objects.

            // Layer 2 is responsible for the model's wireframe.
            const auto& mesh = ref.begin() + 1;

            if (ref.front().get().Instances.size() == 1)
            {
                mesh->get().TurnOn(ref.front());
                mesh->get().Instances[instancesID].Hide = 0;
            }
            else
            {
                mesh->get().Instances[instancesID].Hide = 0;
            }
        }

        //PickingMessage(refWrapOnRender.at({ m_objectID, m_instancesID }).front().get().Name, m_instancesID);
    }
}

void LisaApp::Picking::DeselectMesh(RefOnRender& refWrapOnRender, bool hasShift)
{
    if (!hasShift)
    {
        // If you clicked on an empty space. 
        // But earlier one of the objects was selected.

        for (auto& ref : refWrapOnRender)
        {
            auto& [objectID, instancesID] = ref.first;

            if (ref.second.size() == 1) // For objects that contain one layer.
                ref.second.front().get().Instances[instancesID].Mode = 0;
            else
            {
                // For objects that contain three layers. 
                // 3D primitives and other 3D objects.

                // Layer 2 is responsible for the model's wireframe.
                const auto& mesh = ref.second.begin() + 1;
                mesh->get().Instances[instancesID].Mode = 0;

                if (ref.second.front().get().Instances.size() == 1)
                    mesh->get().TurnOff();
                else
                    mesh->get().Instances[instancesID].Hide = 1;
            }
        }
        refWrapOnRender.clear();
    }
}

void LisaApp::Picking::ClearRefTriangleContainer(RefOnRender& refWrapOnRender) const
{
    for (const auto& ref : refWrapOnRender)
    {
        auto& onRender = ref.second.back().get();

        onRender.Instances[0].HoldDataSize = 0;
        if (!onRender.HoldTriangle.empty())
            onRender.HoldTriangle.clear();
    }
}

void LisaApp::Picking::ClearRefEdgeContainer(RefOnRender& refWrapOnRender) const
{
    for (const auto& ref : refWrapOnRender)
    {
        auto& onRender = ref.second.back().get();

        onRender.Instances[0].HoldDataSize = 0;
        if (!onRender.HoldEdge.empty())
            onRender.HoldEdge.clear();
    }
}

void LisaApp::Picking::ClearRefVertexContainer(RefOnRender& refWrapOnRender) const
{
    for (const auto& ref : refWrapOnRender)
    {
        auto& onRender = ref.second.back().get();

        onRender.Instances[0].HoldDataSize = 0;
        if (!onRender.HoldVertex.empty())
            onRender.HoldVertex.clear();
    }
}

std::wstring LisaApp::Picking::CreateKey(std::int32_t pickedTriangle, std::uint32_t index0, std::uint32_t index1) const
{
    wchar_t indexA[16]{};
    wchar_t indexB[16]{};
    wchar_t triangle[16]{};
    wchar_t key[48]{};

    swprintf_s(indexA, L"%d", (std::min)(index0, index1));
    swprintf_s(indexB, L"%d", (std::max)(index0, index1));
    swprintf_s(triangle, L"%d", pickedTriangle);

    wcsncat_s(key, indexA, wcslen(indexA));
    wcsncat_s(key, L"_", wcslen(L"_"));
    wcsncat_s(key, indexB, wcslen(indexB));
    wcsncat_s(key, L"_", wcslen(L"_"));
    wcsncat_s(key, triangle, wcslen(triangle));

    return key;
}

void LisaApp::Picking::SelectTriangleComponent(RefOnRender& refWrapOnRender, bool hasShift) const
{
    if (hasShift)
    {
        // You can select components on any instance, but they will only be displayed on the main one.

        if (m_hit)
        {
            Constants::HoldData data;

            // Layer 3 is responsible for the components. It is the last one.
            auto& onRender = refWrapOnRender.at({ m_objectID, 0 }).back().get();

            std::uint32_t sz = static_cast<std::uint32_t>(onRender.HoldTriangle.size());

            if (onRender.HoldTriangle.contains(m_pickedTriangle))
            {
                onRender.Instances[0].HoldDataSize = sz - 1;
                onRender.HoldTriangle.erase(m_pickedTriangle);
            }
            else
            {
                data.Triangle = m_pickedTriangle;

                onRender.Instances[0].HoldDataSize = sz + 1;
                onRender.HoldTriangle.insert_or_assign(m_pickedTriangle, data);
            }
        }
    }
    else
    {
        if (m_hit)
        {
            // Single selection is only possible on the master instance. 
            // Selecting components on other instances will not produce any results.

            Constants::HoldData data;

            for (const auto& ref : refWrapOnRender)
            {
                const auto& [objectID, instancesID] = ref.first;
                auto& onRender = ref.second.back().get();

                // We clean it because we are only selecting one component.
                if (!onRender.HoldTriangle.empty())
                    onRender.HoldTriangle.clear();

                if (m_objectID == objectID && m_instancesID == 0)
                {
                    data.Triangle = m_pickedTriangle;

                    onRender.Instances[0].HoldDataSize = 1;
                    onRender.HoldTriangle.insert_or_assign(m_pickedTriangle, data);
                }
                else
                {
                    onRender.Instances[0].HoldDataSize = 0;
                }
            }
        }
        else
        {
            ClearRefTriangleContainer(refWrapOnRender);
        }
    }
}

void LisaApp::Picking::SelectEdgeComponent(RefOnRender& refWrapOnRender, bool hasShift) const
{
    if (hasShift)
    {
        // You can select components on any instance, but they will only be displayed on the main one.

        if (m_hit)
        {
            Constants::HoldData data;
            std::wstring key{};
            bool isFind{};

            // Layer 3 is responsible for the components. It is the last one.
            auto& onRender = refWrapOnRender.at({ m_objectID, 0 }).back().get();

            std::uint32_t sz = static_cast<std::uint32_t>(onRender.HoldEdge.size());

            auto range = onRender.Geo->ComponentsInfo.equal_range(m_pickedTriangle);

            std::vector<std::pair<std::uint32_t, std::uint32_t>> vectorA{};
            std::vector<std::pair<std::uint32_t, std::uint32_t>> vectorB{};

            // Obtain containers with adjacent vertices for the selected vertices.

            for (auto& i = range.first; i != range.second; ++i)
            {
                std::uint32_t& index = i->second.begin()->first;

                if (index == m_selectedIndex[0])
                    vectorA = i->second;
                
                if (index == m_selectedIndex[1])
                    vectorB = i->second;
            }
            
            for (auto itA = vectorA.begin(); itA != vectorA.end(); ++itA)
            {
                std::uint32_t& index0 = itA->first;
                std::uint32_t& triangle = itA->second;

                for (auto itB = vectorB.begin(); itB != vectorB.end(); ++itB)
                {
                    std::uint32_t& index1 = itB->first;

                    key = CreateKey(triangle, index0, index1);

                    if (onRender.HoldEdge.contains(key))
                    {
                        isFind = true;

                        itA = vectorA.end() - 1;
                        itB = vectorB.end() - 1;
                    }                   
                }
            }

            if (isFind)
            {
                onRender.Instances[0].HoldDataSize = sz - 1;
                onRender.HoldEdge.erase(key);
            }
            else
            {
                data.Triangle = m_pickedTriangle;
                data.Component = m_pickedEdge;

                key = CreateKey(m_pickedTriangle, m_selectedIndex[0], m_selectedIndex[1]);

                onRender.Instances[0].HoldDataSize = sz + 1;
                onRender.HoldEdge.insert_or_assign(key, data);
            }           
        }
    }
    else
    {
        if (m_hit)
        {
            // Single selection is only possible on the master instance. 
            // Selecting components on other instances will not produce any results.

            Constants::HoldData data;

            for (auto&& [iter, ref] : refWrapOnRender | std::views::enumerate)
            {
                const auto& [objectID, instancesID] = ref.first;
                auto& onRender = ref.second.back().get();

                // We clean it because we are only selecting one component.
                if (!onRender.HoldEdge.empty())
                    onRender.HoldEdge.clear();

                if (m_objectID == objectID && m_instancesID == 0)
                {
                    data.Triangle = m_pickedTriangle;
                    data.Component = m_pickedEdge;
                    
                    std::wstring key = CreateKey(m_pickedTriangle, m_selectedIndex[0], m_selectedIndex[1]);

                    onRender.Instances[0].HoldDataSize = 1;
                    onRender.HoldEdge.insert_or_assign(key, data);
                }
                else
                {
                    onRender.Instances[0].HoldDataSize = 0;
                }
            }
        }
        else
        {
            ClearRefEdgeContainer(refWrapOnRender);
        }
    }
}

void LisaApp::Picking::SelectVertexComponent(RefOnRender& refWrapOnRender, bool hasShift) const
{
    if (hasShift)
    {
        // You can select components on any instance, but they will only be displayed on the main one.

        if (m_hit)
        {
            Constants::HoldData data;
            std::uint32_t key{};
            bool isFind{};

            // Layer 3 is responsible for the components. It is the last one.
            auto& onRender = refWrapOnRender.at({ m_objectID, 0 }).back().get();

            std::uint32_t sz = static_cast<std::uint32_t>(onRender.HoldVertex.size());

            auto range = onRender.Geo->ComponentsInfo.equal_range(m_pickedTriangle);

            for (auto& i = range.first; i != range.second; ++i)
            {
                std::uint32_t& index = i->second.begin()->first;

                if (index == m_selectedIndex[0])
                {
                    for (auto it = i->second.begin(); it != i->second.end(); ++it)
                    {
                        std::uint32_t& ind = it->first;
                        key = ind;

                        if (onRender.HoldVertex.contains(key))
                        {
                            isFind = true;

                            it = i->second.end() - 1;
                        }
                    }
                }
            }

            if (isFind)
            {
                onRender.Instances[0].HoldDataSize = sz - 1;
                onRender.HoldVertex.erase(key);
            }
            else
            {
                data.Triangle = m_pickedTriangle;
                data.Component = m_pickedVertex;

                onRender.Instances[0].HoldDataSize = sz + 1;
                onRender.HoldVertex.insert_or_assign(m_selectedIndex[0], data);
            }
        }
    }
    else
    {
        if (m_hit)
        {
            // Single selection is only possible on the master instance. 
            // Selecting components on other instances will not produce any results.

            Constants::HoldData data;

            for (auto&& [iter, ref] : refWrapOnRender | std::views::enumerate)
            {
                const auto& [objectID, instancesID] = ref.first;
                auto& onRender = ref.second.back().get();

                // We clean it because we are only selecting one component.
                if (!onRender.HoldVertex.empty())
                    onRender.HoldVertex.clear();

                if (m_objectID == objectID && m_instancesID == 0)
                {
                    data.Triangle = m_pickedTriangle;
                    data.Component = m_pickedVertex;

                    onRender.Instances[0].HoldDataSize = 1;
                    onRender.HoldVertex.insert_or_assign(m_selectedIndex[0], data);
                }
                else
                {
                    onRender.Instances[0].HoldDataSize = 0;
                }
            }
        }
        else
        {
            ClearRefVertexContainer(refWrapOnRender);
        }
    }
}