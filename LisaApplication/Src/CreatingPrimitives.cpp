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

#include "CreatingPrimitives.h"
#include "Globals.h"
#include "XoshiroPRNGs.h"
#include <chrono>

// Creating scene objects. Or creating objects for a 3D viewport.
void LisaApp::CreatingPrimitives::Create(
    _In_ ID3D12Device3* device,
    _In_ ID3D12GraphicsCommandList* commandList,
    UINT primitives,
    PrimitivesData pd,
    INT material
)
{
    using namespace LisaApp::Global;

    // Create a 3D primitive.
    std::unique_ptr<PolygonPrimitives> prim{ nullptr };

    std::wstring name{};
    std::wstring primitiveTopology{};

    switch (primitives)
    {
    case SPHERE:
        prim = PolygonPrimitives::CreateSphere(device, commandList, pd.Radius, pd.SubdivisionsAxis, pd.SubdivisionsHeight);
        break;
    case GEO_SPHERE:
        prim = PolygonPrimitives::CreateGeoSphere(device, commandList, pd.Radius, pd.Subdivisions, pd.RhCoords);
        break;
    case CUBE:
        prim = PolygonPrimitives::CreateBox(device, commandList, pd.Width, pd.Height, pd.Depth, pd.SubdivisionsWidth, pd.SubdivisionsHeight, pd.SubdivisionsDepth);
        break;
    case CYLINDER:
        prim = PolygonPrimitives::CreateCylinder(device, commandList, pd.Radius, pd.Height, pd.SubdivisionsAxis, pd.SubdivisionsHeight, pd.SubdivisionsCaps);
        break;
    case CONE:
        prim = PolygonPrimitives::CreateCone(device, commandList, pd.Radius, pd.Height, pd.SubdivisionsAxis, pd.SubdivisionsHeight, pd.SubdivisionsCaps);
        break;
    case TORUS:
        prim = PolygonPrimitives::CreateTorus(device, commandList, pd.Radius, pd.SectionRadius, pd.SubdivisionsAxis, pd.SubdivisionsHeight, pd.RhCoords);
        break;
    case PLANE:
        prim = PolygonPrimitives::CreatePlane(device, commandList, pd.Width, pd.Depth, pd.SubdivisionsWidth, pd.SubdivisionsDepth);
        break;
    default:
        assert(false && L"There is no primitive with this number.");
        break;
    }


    // Connect the material only to the first layer and the first instance.

    prim->ConnectMaterial(0, 0, material);
 
    // Generate an ID for a primitive.

    std::time_t stime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const std::uint64_t seed = stime + m_sceneObjects.size();
    XoshiroCpp::Xoshiro128PlusPlus rng(seed);
    std::uint32_t id{ rng() };

    id = std::labs(id);

    // Assign a number to the object.

    ObjectNumbering objectNumbering{ .ID = id };

    if (m_objectNumberingMap.contains(prim->OnRender[0].Name))
    {
        objectNumbering.Number = m_objectNumberingMap[prim->OnRender[0].Name].Number;
        objectNumbering.Quantity = m_objectNumberingMap[prim->OnRender[0].Name].Quantity + 1;

        if (m_sceneObjects.contains(m_objectNumberingMap[prim->OnRender[0].Name].ID))
        {
            objectNumbering.Number += 1;         
        }  
    }
    else
    {
        objectNumbering.Number = 1;
        objectNumbering.Quantity += 1;
    }


    // First, we write information about the object's numbering into the container using the base name as the key.
    m_objectNumberingMap.insert_or_assign(prim->OnRender[0].Name, objectNumbering);

    // Then, we add a number to the base name and name the object.
    prim->OnRender[0].Name = prim->OnRender[0].Name + std::to_wstring(objectNumbering.Number);
    prim->OnRender[0].ID = id;

    // We place all new primitives into the container using ID.
    m_sceneObjects.insert({ id, std::move(prim) });


    for (const auto& ref : m_refWrapOnRender)
    {
        // Remove the selection from the object by turning off the layer responsible for the selection.
        
        auto& [objectID, instancesID] = ref.first;

        if (ref.second.size() == 1)
            ref.second.front().get().Instances[instancesID].Mode = 0;
        else
        {
            const auto& mesh = ref.second.begin() + 1;
            mesh->get().Instances[instancesID].Mode = 0;

            if (ref.second.front().get().Instances.size() == 1)
                mesh->get().TurnOff();
            else
                mesh->get().Instances[instancesID].Hide = 1;
        }
    }

    if (!m_refWrapOnRender.empty())
        m_refWrapOnRender.clear();

    // Needed for determine the last selected object.

    m_objectAndInstancesID.first = id;
    m_objectAndInstancesID.second = 0;


    m_refWrapOnRender.insert({ { id, 0 }, {m_sceneObjects[id]->begin(), m_sceneObjects[id]->end()} });

    // To select a new primitive, make the mesh object visible.

    const auto& ref = m_refWrapOnRender.at({ id, 0 });

    if (ref.size() == 1)
        ref.front().get().Instances[0].Mode = 1;
    else
    {
        const auto& mesh = ref.begin() + 1;
        mesh->get().TurnOn(ref.front());
    }


    // We display a message about the creation of a new object.
    std::wstring select = L"Result: " + m_sceneObjects[id]->OnRender[0].Name + L";";

    OutputDebugString(select.c_str());
    OutputDebugString(L"\n");
}

void LisaApp::CreatingPrimitives::CreatePso(
    _In_ ID3D12Device3* device,
    _In_ ID3D12RootSignature* rootSignature,
    DXGI_FORMAT ssaoNormalMap,
    DXGI_FORMAT depthBuffer,
    std::uint32_t sampleCount,
    std::uint32_t sampleQuality
)
{
    // Unzipping shaders.
    //m_newShadersPath = HelperUtilities::Unzipping("Resources\\Shaders\\rs.ls");
    m_shader.SetPathToShaders(LisaApp::HelperPath("..\\LisaApplication\\Resources\\Shaders")  /*m_newShadersPath*/);

    m_pso = { rootSignature, VertexStructs::VertexPositionNormalTextureTangentU::InputLayout };

    LisaApp::PSOConfig::EffectInitial einit;

    einit.pso.SampleDesc = { sampleCount, sampleQuality };
    einit.pso.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    einit.pso.RasterizerState.FrontCounterClockwise = TRUE;
    einit.pso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    m_pso.CreatePipelineState(einit.pso, device, m_shader.Default(), Opaque);

    // PSO for drawing normals.
    einit.pso.RTVFormats[0] = ssaoNormalMap;
    einit.pso.DSVFormat = depthBuffer;
    einit.pso.SampleDesc = { 1, 0 };
    m_pso.CreatePipelineState(einit.pso, device, m_shader.DrawNormals(), Normals);

    LisaApp::PSOConfig::EffectShadow shadow;

    // PSO for shadow map pass.
    const D3D_SHADER_MACRO alphaTestDefines[] = { "ALPHA_TEST", "1", NULL, NULL };
    m_pso.CreatePipelineState(shadow.pso, device, m_shader.Shadows(), Shadow);

    LisaApp::PSOConfig::EffectHighlight highlight;

    highlight.pso.SampleDesc = { sampleCount, sampleQuality };
    highlight.pso.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    m_pso.CreatePipelineState(highlight.pso, device, m_shader.Wireframe(), Wireframe);
    m_pso.CreatePipelineState(highlight.pso, device, m_shader.PickingEdge(), Edge);

    highlight.pso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    m_pso.CreatePipelineState(highlight.pso, device, m_shader.PickingTriangle(), Triangle);
    m_pso.CreatePipelineState(highlight.pso, device, m_shader.PickingVertex(), Vertex);
}

void LisaApp::CreatingPrimitives::CreateInstances(UINT matIndex)
{
    if (!m_refWrapOnRender.empty())
    {
        std::vector<std::pair<std::uint32_t, std::uint32_t>> IDStore;
        IDStore.reserve(m_refWrapOnRender.size());

        for (auto& ref : m_refWrapOnRender)
        {
            auto& [objectID, instancesID] = ref.first;

            if (ref.second.size() == 1)
                ref.second.front().get().Instances[instancesID].Mode = 0;
            else
            {
                const auto& mesh = ref.second.begin() + 1;
                mesh->get().Instances[instancesID].Hide = 1;
            }

            auto& onRender = ref.second.front().get();

            IDStore.emplace_back(onRender.ID, static_cast<std::uint32_t>(onRender.Instances.size()));
        }
        m_refWrapOnRender.clear();


        for (auto& i : IDStore)
        {
            auto& [objectID, instancesID] = i;

            // Create an instance.

            m_sceneObjects[objectID]->AddInstances(instancesID);
        
            // Connect the material.

            m_sceneObjects[objectID]->ConnectMaterial(0, instancesID, matIndex);

            // Place the instance in the same place and having the same shape as the copied object,

            for (auto it = m_sceneObjects[objectID]->begin(); it != m_sceneObjects[objectID]->end(); it++)
            {
                it->Instances[instancesID].World = m_sceneObjects[objectID]->begin()->Instances[instancesID - 1uz].World;
                it->Attributes[instancesID] = m_sceneObjects[objectID]->begin()->Attributes[instancesID - 1uz];
            }

            // Remember the ID of the last selected object.
            
            m_objectAndInstancesID.first = objectID;
            m_objectAndInstancesID.second = instancesID;

            //

            m_refWrapOnRender.insert({ { objectID, instancesID }, {m_sceneObjects[objectID]->begin(), m_sceneObjects[objectID]->end()} });

            // We display in the console that a new instance has been created.            

            std::wstring select = {
                L"Result " +
                m_sceneObjects[objectID]->OnRender[0].Name +
                L".instance[" +
                std::to_wstring(instancesID) +
                L"]" +
                L";"
            };

            OutputDebugString(select.c_str());
            OutputDebugString(L"\n");
        }
    }
}

void LisaApp::CreatingPrimitives::Delete()
{
    if (!m_refWrapOnRender.empty())
    {
        std::vector<std::pair<std::uint32_t, std::uint32_t>> IDStore;
        IDStore.reserve(m_refWrapOnRender.size());

        for (auto& i : m_refWrapOnRender)
        {
            auto& [objectID, instancesID] = i.first;

            IDStore.emplace_back(objectID, instancesID);
        }
        m_refWrapOnRender.clear();

        std::wstring name{};

        for (auto& i : IDStore)
        {
            auto& [objectID, instancesID] = i;

            if (objectID && instancesID == 0)
            {
                // Get the base name of the object from its name.
                name = m_sceneObjects[objectID]->OnRender[0].Name;
                std::wstring n = name;

                // Find the position of the last character that is not a number.
                size_t pos = name.find_last_not_of(L"0123456789");

                if (pos != std::wstring::npos) {
                    // Extract a substring from the beginning of the string up to and including the found position.
                    name = name.substr(0, pos + 1);
                }
                else {
                    // If all characters are numbers, the string will be empty.
                    //name.clear();
                }

                // Decrement the iterator for objects with this base name.
                if (m_objectNumberingMap[name].Quantity)
                    m_objectNumberingMap[name].Quantity -= 1;

                // If the object being deleted with that base name was the last one.
                if (m_objectNumberingMap[name].Quantity == 0)
                    m_objectNumberingMap.erase(name);


                // When the object itself is selected, and not its instance.
                m_sceneObjects.erase(objectID);

                n = L"delete " + n + L";";

                OutputDebugString(n.c_str());
                OutputDebugString(L"\n");
            }
            if (instancesID)
            {
                // When an instance is selected.

                // Check if the main object (instance) exists.
                // Because it may already be deleted if it was selected first and then the instance.
                
                if (m_sceneObjects.contains(objectID))
                {
                    // Delete the instance

                    for (size_t j = 0; j < m_sceneObjects[objectID]->GetNumberOfLayers(); j++)
                    {
                        m_sceneObjects[objectID]->OnRender[j].Instances.erase(
                            std::next(m_sceneObjects[objectID]->OnRender[j].Instances.begin(), instancesID));
                    }

                    // and attributes.

                    for (size_t j = 0; j < m_sceneObjects[objectID]->GetNumberOfLayers(); j++)
                    {
                        m_sceneObjects[objectID]->OnRender[j].Attributes.erase(
                            std::next(m_sceneObjects[objectID]->OnRender[j].Attributes.begin(), instancesID));
                    }

                    name = m_sceneObjects[objectID]->OnRender[0].Name;
                }
                
                std::wstring d = { L"delete " + name + L".instance[" + std::to_wstring(instancesID) + L"]" + L";" };

                OutputDebugString(d.c_str());
                OutputDebugString(L"\n");
            }
        }
    }    
}

void LisaApp::CreatingPrimitives::EditingAttributes(
    const IPivot::Attributes& attributes, 
    const IPivot::Axis& activeAxis,
    const LisaApp::Global::PivotMode& pivotMode
)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    for (auto& i : m_refWrapOnRender)
    {
        auto& [objectID, instancesID] = i.first;
        auto& primitives = i.second.front().get();

        // Changing the attributes and creating a matrix is ​​enough for the first layer.

        // Translation.

        if (pivotMode == PivotMode::Translate)
        {
            if (activeAxis.AxisX)
                primitives.Attributes[instancesID].TranslateX += attributes.OutputTranslateX;
            if (activeAxis.AxisY)
                primitives.Attributes[instancesID].TranslateY += attributes.OutputTranslateY;
            if (activeAxis.AxisZ)
                primitives.Attributes[instancesID].TranslateZ += attributes.OutputTranslateZ;
        }

        // Rotation.

        if (pivotMode == PivotMode::Rotate)
        {
            XMMATRIX m = XMMatrixRotationQuaternion(XMQuaternionNormalize(primitives.Attributes[instancesID].Quaternion));
            XMVECTOR axis{};

            if (activeAxis.AxisX)
                axis = XMVector3Normalize(XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), m));
            else if (activeAxis.AxisY)
                axis = XMVector3Normalize(XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), m));
            else if (activeAxis.AxisZ)
                axis = XMVector3Normalize(XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), m));
            else if (activeAxis.OuterCircle)
                axis = attributes.AxisNorm;
            else if (activeAxis.Sphere)
                axis = attributes.AxisNorm;

            primitives.Attributes[instancesID].Quaternion =
                XMQuaternionMultiply(primitives.Attributes[instancesID].Quaternion, XMQuaternionRotationNormal(axis, attributes.Radian));
        }

        // Scaling.

        if (pivotMode == PivotMode::Scale)
        {
            if (activeAxis.AxisX)
                primitives.Attributes[instancesID].ScaleX += attributes.OutputScaleX;
            if (activeAxis.AxisY)
                primitives.Attributes[instancesID].ScaleY += attributes.OutputScaleY;
            if (activeAxis.AxisZ)
                primitives.Attributes[instancesID].ScaleZ += attributes.OutputScaleZ;
        }

        //primitives.get().Attributes[instancesID].RotateX = attributes.RotateX;
        //primitives.get().Attributes[instancesID].RotateY = attributes.RotateY;
        //primitives.get().Attributes[instancesID].RotateZ = attributes.RotateZ;

        XMMATRIX matrix = XMMatrixAffineTransformation(
            XMVectorSet(
                primitives.Attributes[instancesID].ScaleX,
                primitives.Attributes[instancesID].ScaleY,
                primitives.Attributes[instancesID].ScaleZ,
                0.0f
            ),
            XMVectorZero(),
            primitives.Attributes[instancesID].Quaternion,
            XMVectorSet(
                primitives.Attributes[instancesID].TranslateX,
                primitives.Attributes[instancesID].TranslateY,
                primitives.Attributes[instancesID].TranslateZ,
                0.0f
            )
        );

        /*primitives.get().Bounds.Extents.x = primitives.get().BoundsOriginal.Extents.x * attributes.ScaleX;
        primitives.get().Bounds.Extents.y = primitives.get().BoundsOriginal.Extents.y * attributes.ScaleY;
        primitives.get().Bounds.Extents.z = primitives.get().BoundsOriginal.Extents.z * attributes.ScaleZ;*/

        // Apply the matrix to all layers.

        for (auto& p : i.second)
            XMStoreFloat4x4(&p.get().Instances[instancesID].World, matrix);

    }
}

//For the sake of speed, this function for preserving adjacent indices had to be abandoned.
// 

//auto const ProcessAdjacentVertices = [&](
 //    const auto& holdData,
 //    auto& geo,
 //    std::unordered_map<std::uint32_t, std::uint32_t>& storeIndex,
 //    size_t componentIndex
 //    ) 
 //    {
 //        for (const auto& h : holdData) 
 //        {
 //            auto range = geo->ComponentsInfo.equal_range(h.second.Triangle);
 //            std::uint32_t baseIndex = h.second.Triangle * 3;
 //
 //            std::vector<std::uint32_t> targetIndices;
 //            if (componentIndex == std::numeric_limits<size_t>::max()) 
 //            {
 //                // Для FACE — все три вершины
 //                targetIndices = { baseIndex, baseIndex + 1, baseIndex + 2 };
 //            }
 //            else 
 //            {
 //                // Для EDGE и VERTEX — одна или две вершины
 //                switch (h.second.Component)
 //                {
 //                case 0: targetIndices.push_back(baseIndex); break;
 //                case 1: targetIndices.push_back(baseIndex + 1); break;
 //                case 2: targetIndices.push_back(baseIndex + 2); break;
 //                }
 //                if (componentIndex == 1) 
 //                { // Для EDGE добавляем вторую вершину
 //                    targetIndices.push_back((baseIndex + (h.second.Component + 1) % 3));
 //                }
 //            }
 //
 //            for (auto& i = range.first; i != range.second; ++i) 
 //            {
 //                std::uint32_t& index = i->second.begin()->first;
 //                if (std::find(targetIndices.begin(), targetIndices.end(), index) != targetIndices.end())
 //                {
 //                    for (auto it = i->second.begin(); it != i->second.end(); ++it) 
 //                    {
 //                        storeIndex.insert_or_assign(it->first, 0);
 //                    }
 //                }
 //            }
 //        }
 //    };
 //       if (selectMode == SELECTION::FACE) {
 //           ProcessAdjacentVertices(ref.second.back().get().HoldTriangle, geo, storeIndex, std::numeric_limits<size_t>::max());
 //       }
 //       else if (selectMode == SELECTION::EDGE) {
 //           ProcessAdjacentVertices(ref.second.back().get().HoldEdge, geo, storeIndex, 1);
 //       }
 //       else if (selectMode == SELECTION::VERTEX) {
 //           ProcessAdjacentVertices(ref.second.back().get().HoldVertex, geo, storeIndex, 0);
 //       }

void LisaApp::CreatingPrimitives::EditingComponents(
    const IPivot::Attributes& attributes,
    const IPivot::Axis& activeAxis,
    const LisaApp::Global::PivotMode& pivotMode,
    std::uint32_t selectMode
)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;

    // Using an unordered map is justified. 
    // Sets of adjacent vertices may contain the same indices for 
    // those triangles or edges that are selected next to each other.
    //
    //  v1, v2  v5,       The sets of adjacent vertices for
    //     *  e1 *        v1 and v2 will contain the same indices.
    //     |\    |  
    //     | \   |        This is also true for v3 and v4.
    // e0  |  \  | e2
    //     |   \ |
    //     |    \|
    //     *     *
    //    v0    v3, v4 

    std::uint32_t i0{}, i1{}, i2{};

    if (m_updateContiguousIndexsContainer)
    {
        m_contiguousIndexesStorage.clear();
        m_rawIndexStorage.clear();
    }

    m_contiguousIndexesStorage.resize(m_refWrapOnRender.size());
    m_rawIndexStorage.resize(m_refWrapOnRender.size());

    for (auto&& [iter, ref] : m_refWrapOnRender | std::views::enumerate)
    {
        Item::OnRender& obj = ref.second.front().get();
        XMFLOAT4X4& world = obj.Instances[0].World;
        std::uint32_t& ID{ obj.ID };

  
        auto& geo = m_sceneObjects[ID]->OnRender[0].Geo;
        auto vertices = reinterpret_cast<VertexType*>(geo->VertexBufferCPU->GetBufferPointer());
        
        // For each vertex there is a store of its adjacent vertices.
        // This is a vector of pairs, where the first pair is the 
        // index and triangle(in exactly that order) of the parent triangle,   
        // and all other pairs contain adjacent indices and triangles.

        if (m_updateContiguousIndexsContainer)
        {
            if (selectMode == selection::face)
            {
                for (const auto& h : ref.second.back().get().HoldTriangle)
                {
                    auto range = geo->ComponentsInfo.equal_range(h.second.Triangle);

                    i0 = h.second.Triangle * 3;
                    i1 = i0 + 1;
                    i2 = i0 + 2;

                    m_rawIndexStorage[iter].emplace_back(i0);
                    m_rawIndexStorage[iter].emplace_back(i1);
                    m_rawIndexStorage[iter].emplace_back(i2);

                    for (auto& i = range.first; i != range.second; ++i)
                    {
                        std::uint32_t& index = i->second.begin()->first;

                        if (index == i0)
                        {
                            for (auto it = i->second.begin(); it != i->second.end(); ++it)
                            {
                                m_contiguousIndexesStorage[iter].insert_or_assign(it->first, it->second);
                            }
                        }
                        else if (index == i1)
                        {
                            for (auto it = i->second.begin(); it != i->second.end(); ++it)
                            {
                                m_contiguousIndexesStorage[iter].insert_or_assign(it->first, it->second);
                            }
                        }
                        else if (index == i2)
                        {
                            for (auto it = i->second.begin(); it != i->second.end(); ++it)
                            {
                                m_contiguousIndexesStorage[iter].insert_or_assign(it->first, it->second);
                            }
                        }
                    }
                }
            }
            else if (selectMode == selection::edge)
            {
                for (const auto& h : ref.second.back().get().HoldEdge)
                {
                    auto range = geo->ComponentsInfo.equal_range(h.second.Triangle);

                    i0 = h.second.Triangle * 3;
                    i1 = i0 + 1;
                    i2 = i0 + 2;

                    std::uint32_t ind[2]{};

                    if (h.second.Component == 0)
                    {
                        ind[0] = i0;
                        ind[1] = i1;
                    }
                    else if (h.second.Component == 1)
                    {
                        ind[0] = i1;
                        ind[1] = i2;
                    }
                    else if (h.second.Component == 2)
                    {
                        ind[0] = i2;
                        ind[1] = i0;
                    }

                    m_rawIndexStorage[iter].emplace_back(ind[0]);
                    m_rawIndexStorage[iter].emplace_back(ind[1]);

                    for (auto& i = range.first; i != range.second; ++i)
                    {
                        std::uint32_t& index = i->second.begin()->first;

                        if (index == ind[0])
                        {
                            for (auto it = i->second.begin(); it != i->second.end(); ++it)
                            {
                                m_contiguousIndexesStorage[iter].insert_or_assign(it->first, it->second);
                            }
                        }
                        else if (index == ind[1])
                        {
                            for (auto it = i->second.begin(); it != i->second.end(); ++it)
                            {
                                m_contiguousIndexesStorage[iter].insert_or_assign(it->first, it->second);
                            }
                        }
                    }
                }
            }
            else if (selectMode == selection::vertex)
            {
                for (const auto& h : ref.second.back().get().HoldVertex)
                {
                    auto range = geo->ComponentsInfo.equal_range(h.second.Triangle);

                    i0 = h.second.Triangle * 3;
                    i1 = i0 + 1;
                    i2 = i0 + 2;

                    std::uint32_t ind{};

                    if (h.second.Component == 0)
                        ind = i0;
                    else if (h.second.Component == 1)
                        ind = i1;
                    else if (h.second.Component == 2)
                        ind = i2;

                    m_rawIndexStorage[iter].emplace_back(ind);

                    for (auto& i = range.first; i != range.second; ++i)
                    {
                        std::uint32_t& index = i->second.begin()->first;

                        if (index == ind)
                        {
                            for (auto it = i->second.begin(); it != i->second.end(); ++it)
                            {
                                m_contiguousIndexesStorage[iter].insert_or_assign(it->first, it->second);
                            }
                        }
                    }
                }
            }
        }
        

        XMVECTOR determinant = XMMatrixDeterminant(XMLoadFloat4x4(&world));
        XMMATRIX mInverse = XMMatrixInverse(&determinant, XMLoadFloat4x4(&world));
        
        XMVECTOR center{ attributes.TranslateX,  attributes.TranslateY, attributes.TranslateZ };
        XMVECTOR centerToLocal = XMVector3Transform(center, mInverse);

        XMMATRIX affineTransform{};

        if (pivotMode == PivotMode::Rotate)
        {
            // Subtract the object's quatarion so that the rotation starts with axes x0, y0, z0.

            // For several objects, it was decided to take the quatarion from the first object.
            // Also, the rotation pivot is set according to the quatarion of the first object.

            XMVECTOR qInverse = XMQuaternionInverse(m_refWrapOnRender.begin()->second.front().get().Attributes[0].Quaternion);
            XMVECTOR qMultiply = XMQuaternionMultiply(attributes.Quaternion, qInverse);

            XMMATRIX m = XMMatrixRotationQuaternion(XMQuaternionNormalize(qMultiply));
            XMVECTOR axis{};

            if (activeAxis.AxisX)
                axis = XMVector3Normalize(XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), m));
            else if (activeAxis.AxisY)
                axis = XMVector3Normalize(XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), m));
            else if (activeAxis.AxisZ)
                axis = XMVector3Normalize(XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), m));
            else if (activeAxis.OuterCircle)
                axis = attributes.AxisNorm;
            else if (activeAxis.Sphere)
                axis = attributes.AxisNorm;

            XMVECTOR quaternion = XMQuaternionIdentity();
            quaternion = XMQuaternionMultiply(quaternion, XMQuaternionRotationNormal(axis, attributes.Radian));

            affineTransform = XMMatrixAffineTransformation({ 1.0f, 1.0f, 1.0f }, centerToLocal, quaternion, {});
        }

        for (const auto& umap : m_contiguousIndexesStorage[iter])
        {
            const std::uint32_t& index = umap.first;
            const std::uint32_t& triangle = umap.second;

            if (pivotMode == PivotMode::Translate)
            {
                XMVECTOR toWorld = XMVector3Transform(XMLoadFloat3(&vertices[index].position), XMLoadFloat4x4(&world));

                if (activeAxis.AxisX)
                    toWorld.m128_f32[0] += attributes.OutputTranslateX;
                if (activeAxis.AxisY)
                    toWorld.m128_f32[1] += attributes.OutputTranslateY;
                if (activeAxis.AxisZ)
                    toWorld.m128_f32[2] += attributes.OutputTranslateZ;

                XMVECTOR toLocal = XMVector3Transform(toWorld, mInverse);
                XMStoreFloat3(&vertices[index].position, toLocal);
            }

            if (pivotMode == PivotMode::Rotate)
            {
                XMVECTOR transform = XMVector3Transform(XMLoadFloat3(&vertices[index].position), affineTransform);
                XMStoreFloat3(&vertices[index].position, transform);
            }

            if (pivotMode == PivotMode::Scale)
            {
                // Move the vertex to the center of coordinates, scale it and return it back.

                XMVECTOR scaling{ 1.0f, 1.0f, 1.0f };

                if (activeAxis.AxisX)
                    scaling.m128_f32[0] += attributes.OutputScaleX;
                if (activeAxis.AxisY)
                    scaling.m128_f32[1] += attributes.OutputScaleY;
                if (activeAxis.AxisZ)
                    scaling.m128_f32[2] += attributes.OutputScaleZ;

                XMVECTOR p = XMLoadFloat3(&vertices[index].position);
                XMVECTOR toOrigin = XMVectorSubtract(p, centerToLocal);

                XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scaling);

                XMVECTOR scale = XMVector3Transform(toOrigin, scaleMatrix);
                XMVECTOR toBack = XMVectorAdd(scale, centerToLocal);

                XMStoreFloat3(&vertices[index].position, toBack);
            }

            // Update the normals.

            i0 = triangle * 3;
            i1 = i0 + 1;
            i2 = i0 + 2;

            XMVECTOR v0 = XMVectorSubtract(XMLoadFloat3(&vertices[i1].position), XMLoadFloat3(&vertices[i0].position));
            XMVECTOR v1 = XMVectorSubtract(XMLoadFloat3(&vertices[i2].position), XMLoadFloat3(&vertices[i0].position));

            XMVECTOR cross = XMVector3Cross(v1, v0);
            XMStoreFloat3(&vertices[i0].normal, XMVector3Normalize(cross));

            v0 = XMVectorSubtract(XMLoadFloat3(&vertices[i0].position), XMLoadFloat3(&vertices[i1].position));
            v1 = XMVectorSubtract(XMLoadFloat3(&vertices[i2].position), XMLoadFloat3(&vertices[i1].position));

            cross = XMVector3Cross(v0, v1);
            XMStoreFloat3(&vertices[i1].normal, XMVector3Normalize(cross));

            v0 = XMVectorSubtract(XMLoadFloat3(&vertices[i0].position), XMLoadFloat3(&vertices[i2].position));
            v1 = XMVectorSubtract(XMLoadFloat3(&vertices[i1].position), XMLoadFloat3(&vertices[i2].position));

            cross = XMVector3Cross(v1, v0);
            XMStoreFloat3(&vertices[i2].normal, XMVector3Normalize(cross));            
        }

        if (!m_contiguousIndexesStorage[iter].empty())
        {
            DirectX::BoundingBox newBounds = NewBoundingBox(vertices, iter);

            std::uint32_t componentCount = m_sceneObjects[ID]->OnRender[0].IndexCount;
            const UINT vbByteSize = componentCount * sizeof(VertexType);

            m_sceneObjects[ID]->MappedData(vertices, vbByteSize);

            if (m_sceneObjects[ID]->OnRender)
            {
                m_sceneObjects[ID]->OnRender[0].Geo->VertexBufferGPU = m_sceneObjects[ID]->GetUploadBuffer();

                DirectX::BoundingBox oldBounds = m_sceneObjects[ID]->OnRender[0].Bounds;
                m_sceneObjects[ID]->OnRender[0].Bounds.CreateMerged(m_sceneObjects[ID]->OnRender[0].Bounds, oldBounds, newBounds);
            }
        }


        if (static_cast<size_t>(iter) == m_refWrapOnRender.size() - 1)
            m_updateContiguousIndexsContainer = false;
    }
}

bool LisaApp::CreatingPrimitives::PreparingEditComponents(
    std::uint32_t mode, 
    const std::pair<std::uint32_t, std::uint32_t>& lastID
)
{
    using namespace LisaApp::Global;

    if (m_picking->GetMode() != mode)
        m_picking->SetMode(mode);
    else
        return false;

    // 
    if (mode == selection::face)
        m_PSOMode = Triangle;
    else if (mode == selection::edge)
        m_PSOMode = Edge;
    else if (mode == selection::vertex)
        m_PSOMode = Vertex;

    for (auto& ref : m_refWrapOnRender)
    {
        auto& [objectID, instancesID] = ref.first;

        const auto& mesh = ref.second.begin() + 1;

        if (mode == selection::mesh)
        {
            if (objectID == lastID.first && instancesID == lastID.second)
                mesh->get().Instances[instancesID].Mode = m_picking->GetMode();
            else
                mesh->get().Instances[instancesID].Mode = selection::alreadyAllocated;
        }
        else
        {
            mesh->get().Instances[instancesID].Mode = m_picking->GetMode();
        }
        

        const auto& components = ref.second.back();
        components.get().Instances[instancesID].Mode = m_picking->GetMode();
        components.get().Instances[instancesID].DrawTriangle = false;

        if (mode == selection::mesh)
            components.get().TurnOff();
        else
            components.get().TurnOn(ref.second.front());
        

        if (mode == selection::face)
            components.get().Instances[m_instancesID].HoldDataSize = static_cast<UINT>(components.get().HoldTriangle.size());
        else if (mode == selection::edge)
            components.get().Instances[m_instancesID].HoldDataSize = static_cast<UINT>(components.get().HoldEdge.size());        
        else if (mode == selection::vertex)
            components.get().Instances[m_instancesID].HoldDataSize = static_cast<UINT>(components.get().HoldVertex.size());
    }

    return true;
}

void LisaApp::CreatingPrimitives::Draw(
    _In_ ID3D12GraphicsCommandList* commandList
)
{
    for (auto& so : m_sceneObjects)
    {
        so.second->Draw(commandList, 
            {
                m_pso.GetPipeline(Opaque),
                m_pso.GetPipeline(Wireframe),
                m_pso.GetPipeline(m_PSOMode), 
            });
    }
}

void LisaApp::CreatingPrimitives::DrawNormals(
    _In_ ID3D12GraphicsCommandList* commandList
)
{
    for (auto& so : m_sceneObjects)
        so.second->Draw(commandList, { m_pso.GetPipeline(Normals) });
}

void LisaApp::CreatingPrimitives::DrawShadow(
    _In_ ID3D12GraphicsCommandList* commandList
)
{
    for (auto& so : m_sceneObjects)
        so.second->Draw(commandList, { m_pso.GetPipeline(Shadow) });
}

// Updating the constant buffer.
void LisaApp::CreatingPrimitives::UpdateCB(
    const DirectX::XMMATRIX& getViev,
    const DirectX::BoundingFrustum& camFrustum,
    bool frustumCullingEnabled
)
{
    for (const auto& so : m_sceneObjects)
    {
        so.second->UpdateBuffer(getViev, camFrustum, frustumCullingEnabled);
    }
}

void LisaApp::CreatingPrimitives::PrePick(
    const DirectX::XMMATRIX& getViev,
    const DirectX::XMFLOAT4X4& Proj4x4f,
    std::int32_t sx,
    std::int32_t sy,
    INT width,
    INT height
)
{
    using vertexType = VertexStructs::VertexPositionNormalTextureTangentU;
    m_picking->PrePick<vertexType>(getViev, Proj4x4f, sx, sy, width, height, m_refWrapOnRender);
}

// Selecting an object or its component.
bool LisaApp::CreatingPrimitives::Pick(
    const DirectX::XMMATRIX& getViev, 
    const DirectX::XMFLOAT4X4& Proj4x4f,
    std::int32_t sx,
    std::int32_t sy,
    INT width, 
    INT height,
    bool hasShift
)
{
    using vertexType = VertexStructs::VertexPositionNormalTextureTangentU;

    // An unordered map is used to store references, and therefore direct access to the last element is not possible.
    // Therefore, it was decided to record the IDs of objects both when they were created and selected.

    // And in this case, classes pass IDs to each other.
    m_picking->SetLastID(m_objectAndInstancesID);

    bool p = m_picking->Pick<vertexType>(m_refWrapOnRender, m_sceneObjects, getViev, Proj4x4f, sx, sy, width, height, hasShift);

    m_objectAndInstancesID = m_picking->GetLastID();

    return p;
}
