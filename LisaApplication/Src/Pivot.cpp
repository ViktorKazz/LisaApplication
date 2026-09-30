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

#include "Pivot.h"

void LisaApp::Pivot::Create(
    _In_ ID3D12Device3* device,
    _In_ ID3D12GraphicsCommandList* commandList,
    _In_ ID3D12RootSignature* rootSignature,
    std::uint32_t sampleCount,
    std::uint32_t sampleQuality
)
{
    using namespace DirectX;

    CreatePso(device, rootSignature, sampleCount, sampleQuality);

    m_pivotTr.Create(device, commandList);
    m_pivotRt.Create(device, commandList);
    m_pivotSc.Create(device, commandList);
}

void LisaApp::Pivot::CreatePso(
    _In_ ID3D12Device3* device, 
    _In_ ID3D12RootSignature* rootSignature,
    std::uint32_t sampleCount, 
    std::uint32_t sampleQuality
)
{
    // Unzipping shaders.
    //m_newShadersPath = HelperUtilities::Unzipping("Resources\\Shaders\\rs.ls");
    m_shader.SetPathToShaders(LisaApp::HelperPath("..\\LisaApplication\\Resources\\Shaders")  /*m_newShadersPath*/);

    // To make sure objects appear on top of everything else, we'll disable depth testing.
    // DepthEnable = FALSE

    m_pso = { rootSignature, VertexStructs::VertexPositionColor::InputLayout };

    LisaApp::PSOConfig::EffectInitial einit;

    einit.pso.SampleDesc = { sampleCount, sampleQuality };
    einit.pso.DepthStencilState.DepthEnable = FALSE;

    einit.pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    const D3D_SHADER_MACRO coloringPerInstances[] = { "COLORING_PER_INSTANCES", "1", NULL, NULL };
    m_pso.CreatePipelineState(einit.pso, device, m_shader.SimpleColoring(), Line);

    const D3D_SHADER_MACRO changeColorRelativeToTheCamera[] = { "CHANGE_COLOR", "1", NULL, NULL };
    m_pso.CreatePipelineState(einit.pso, device, m_shader.SimpleColoring(nullptr, changeColorRelativeToTheCamera), LineCircle);

    einit.pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    m_pso.CreatePipelineState(einit.pso, device, m_shader.PivotCenterFrame(), CenterFrame);

    einit.pso.BlendState.RenderTarget[0] = LisaApp::PSOConfig::BlendDesc();
    m_pso.CreatePipelineState(einit.pso, device, m_shader.SimpleColoring(), Mesh);

    const D3D_SHADER_MACRO sphereColorMode[] = { "PIVOT_SPHERE", "1", NULL, NULL };
    m_pso.CreatePipelineState(einit.pso, device, m_shader.SimpleColoring(nullptr, sphereColorMode), Sphere);

    m_pso.CreatePipelineState(einit.pso, device, m_shader.ColorRotationAngle(), ColorRotationAngle);
}

void LisaApp::Pivot::Draw(_In_ ID3D12GraphicsCommandList* commandList)
{
    using namespace LisaApp::Global;

    if (m_onOff)
    {
        switch (m_mode)
        {
        case PivotMode::Translate:
        {
            m_pivotTr.Draw(
                commandList,
                m_pso.GetPipeline(CenterFrame),
                m_pso.GetPipeline(Mesh),
                m_pso.GetPipeline(Line)
            );

        }
        break;
        case PivotMode::Rotate:
        {
            m_pivotRt.Draw(
                commandList, 
                m_pso.GetPipeline(Line), 
                m_pso.GetPipeline(Sphere), 
                m_pso.GetPipeline(LineCircle), 
                m_pso.GetPipeline(ColorRotationAngle)
            );
        }
        break;
        case PivotMode::Scale:
        {
            m_pivotSc.Draw(
                commandList,
                m_pso.GetPipeline(Mesh),
                m_pso.GetPipeline(Line)
            );
        }
        break;
        default:
            break;
        }
    }
}

void LisaApp::Pivot::UpdateCB(
    const DirectX::XMMATRIX& getViev,
    const DirectX::BoundingFrustum& camFrustum,
    bool frustumCullingEnabled
)
{
    using namespace LisaApp::Global;

    if (m_onOff)
    {
        switch (m_mode)
        {
        case PivotMode::Translate:
            m_pivotTr.UpdateCB(getViev, camFrustum, frustumCullingEnabled);
        break;
        case PivotMode::Rotate:
            m_pivotRt.UpdateCB(getViev, camFrustum, frustumCullingEnabled);
        break;
        case PivotMode::Scale:
            m_pivotSc.UpdateCB(getViev, camFrustum, frustumCullingEnabled);
        break;
        default:
            break;
        }
    }
}

void LisaApp::Pivot::ClearRefStorage()
{
    for (auto& i : m_refWrapOnRender)
    {
        auto& [objectID, instancesID] = i.first;
        auto& onRender = i.second.front().get();

        onRender.Instances[instancesID].Mode = 0;
    }
    m_refWrapOnRender.clear();
}

bool LisaApp::Pivot::Pick(
    const DirectX::XMMATRIX& getViev,
    const DirectX::XMFLOAT4X4& proj4x4f,
    std::int32_t sx,
    std::int32_t sy,
    std::int32_t screenWidth,
    std::int32_t screenHeight
)
{
    using namespace LisaApp::Global;

    using vertexType = VertexStructs::VertexPositionColor;


    if (m_onOff)
    {
        switch (m_mode)
        {
        case PivotMode::Translate:
        {
            m_picking->Pick<vertexType>(m_refWrapOnRender, m_pivotTr.GetStorageTranslate(), getViev, proj4x4f, sx, sy, screenWidth, screenHeight, false);
            
            // If we select the pivot cone, we will also select the line and vice versa.
            m_pivotTr.SelectingAdjacentPivotObjects(m_refWrapOnRender);
            
            return m_picking->GetHit();
        }
        break;
        case PivotMode::Rotate:
            m_picking->Pick<vertexType>(m_refWrapOnRender, m_pivotRt.GetStorageRotate(), getViev, proj4x4f, sx, sy, screenWidth, screenHeight, false);
            return m_picking->GetHit();
        break;
        case PivotMode::Scale:
            m_picking->Pick<vertexType>(m_refWrapOnRender, m_pivotSc.GetStorageScale(), getViev, proj4x4f, sx, sy, screenWidth, screenHeight, false);

            // If we select the pivot cone, we will also select the line and vice versa.
            m_pivotSc.SelectingAdjacentPivotObjects(m_refWrapOnRender);

            return m_picking->GetHit();
        break;
        default:
            break;
        }
    }

    return false;
}

// Function for scaling the pivot.
void LisaApp::Pivot::Scale(
    const DirectX::XMMATRIX& getViev,
    const DirectX::XMFLOAT4X4& proj4x4f,
    const DirectX::XMVECTOR& cameraPosition,
    std::int32_t screenWidth,
    std::int32_t screenHeight
)
{
    using namespace LisaApp::HelperMath;
    using namespace LisaApp::Global;
    using namespace DirectX;

   // m_cameraPosition = cameraPosition;

    // Updating the matrix.
    // This is the plane matrix with which the rays intersect to calculate the scale of the pivot radius.

    m_scaleMatrix.r[3].m128_f32[0] = m_attributes.TranslateX;
    m_scaleMatrix.r[3].m128_f32[1] = m_attributes.TranslateY;
    m_scaleMatrix.r[3].m128_f32[2] = m_attributes.TranslateZ;

    // Translate the pivot base into screen coordinates.

    std::pair<float, float> outputA = ConvertWorldSpaceToViewSpace(
        getViev,
        proj4x4f,
        screenWidth,
        screenHeight,
        m_attributes.TranslateX, 
        m_attributes.TranslateY, 
        m_attributes.TranslateZ
    );
 
    if (!m_getDefaultPivotRadiusInPixel)
    {
        // Using the default pivot radius, we will translate the top point into screen coordinates.

        std::pair<float, float> outputB = ConvertWorldSpaceToViewSpace(
            getViev,
            proj4x4f, 
            screenWidth,
            screenHeight,
            m_attributes.TranslateX,
            gDefaultPivotRadius,
            m_attributes.TranslateZ
        );

        XMVECTOR s = XMVectorSubtract(
            XMVECTOR{ outputB.first, outputB.second, 0.0f, 0.0f }, 
            XMVECTOR{ outputA.first, outputA.second, 0.0f, 0.0f }
        );
        
        // Сalculate the distance in pixels between the points.
        // Due to the fact that monitors have different pixel densities, 
        // it was decided to calculate the default value.
        XMVECTOR l = XMVector2Length(s);

        m_getDefaultPivotRadiusInPixel = XMVectorGetX(l);
    }

    // Define the plane.

    // Get look and normal.
    XMVECTOR nrm = XMVectorSet(
        -getViev.r[0].m128_f32[2],
        -getViev.r[1].m128_f32[2],
        -getViev.r[2].m128_f32[2],
        0.0f
    );
    XMVECTOR n = XMVector3Normalize(nrm);
    XMVECTOR planeXYZ = XMPlaneFromPointNormal(
        { m_attributes.TranslateX, m_attributes.TranslateY, m_attributes.TranslateZ, 0.0f }, n);


    DirectX::XMVECTOR rayOrigin{};
    DirectX::XMVECTOR rayDir{};

    std::int32_t sx = static_cast<std::int32_t>(outputA.first);
    std::int32_t sy = static_cast<std::int32_t>(outputA.second);
    
    CalculatingRays(getViev, proj4x4f, m_scaleMatrix, rayOrigin, rayDir, sx, sy, screenWidth, screenHeight);
    XMVECTOR lowerIntersection = LisaApp::HelperMath::IntersectRayPlane(rayOrigin, rayDir, planeXYZ);

    sx = static_cast<std::int32_t>(outputA.first);
    sy = static_cast<std::int32_t>(outputA.second + m_getDefaultPivotRadiusInPixel);

    CalculatingRays(getViev, proj4x4f, m_scaleMatrix, rayOrigin, rayDir, sx, sy, screenWidth, screenHeight);
    XMVECTOR upperIntersection = LisaApp::HelperMath::IntersectRayPlane(rayOrigin, rayDir, planeXYZ);


    XMVECTOR l = XMVector3Length(XMVectorSubtract(lowerIntersection, upperIntersection));

    // Initially, the scale will not be equal to 1.
    // This is due to the fact that the plane with which the rays intersect has a tilt, as does the camera itself.
    float scale = l.m128_f32[0] / gDefaultPivotRadius;
      
    m_attributes.ScaleX = scale;
    m_attributes.ScaleY = scale;
    m_attributes.ScaleZ = scale;

    // Scaling the pivot when the camera moves away and closer.

    m_pivotTr.Scale(m_attributes, cameraPosition);
    
    //

    m_pivotRt.Scale(m_attributes, cameraPosition);

    //

    m_pivotSc.Scale(m_attributes, cameraPosition);

    SetPivotDirty(false);
}

/*
// You can take any point on the plane, for example the point (10, 0, 10).
            DirectX::XMVECTOR pointOnPlane = DirectX::XMVectorSet(10.0f, 0.0f, 10.0f, 0.0f);

            // For this plane, the coordinate Y = 0.
            // Thus, the equation of a plane is: y = 0, or in general: 0x + 1y + 0z + 0 = 0.
            // Here: A = 0, B = 1, C = 0, D = 0.
            // Therefore, for the plane y = 0, the normal will be (0, 1, 0),
            // since the plane is perpendicular to the Y-axis.
            DirectX::XMVECTOR normal = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

            // XMPlaneFromPointNormal returns an XMVECTOR where the xyz component is the normal and w is D.
            return DirectX::XMPlaneFromPointNormal(pointOnPlane, normal);
*/

// Function for hiding pivot components depending on the camera angle.
void LisaApp::Pivot::HidingPivotComponents(const DirectX::XMVECTOR& camerasLook)
{
    m_pivotTr.HidingPivotComponents(camerasLook);
    m_pivotSc.HidingPivotComponents(camerasLook);
}

void LisaApp::Pivot::PlaceThePivotInTheDesiredPositionComponent(
    const RefOnRender& refOnRender,
    const DirectX::XMVECTOR& camerasLook,
    std::uint32_t selectMode
)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    SetPivotDirty(true);

    std::uint32_t i0{}, i1{}, i2{};
    std::vector<XMVECTOR> allPos;
    
    auto addPositions = [&](const auto& umap, const auto& w, const auto& vstore)
        {
            if (selectMode == selection::face)
            {
                for (const auto& h : umap)
                {
                    i0 = h.second.Triangle * 3;
                    i1 = i0 + 1;
                    i2 = i0 + 2;

                    allPos.emplace_back(XMVector3Transform(XMLoadFloat3(&vstore[i0].position), XMLoadFloat4x4(&w)));
                    allPos.emplace_back(XMVector3Transform(XMLoadFloat3(&vstore[i1].position), XMLoadFloat4x4(&w)));
                    allPos.emplace_back(XMVector3Transform(XMLoadFloat3(&vstore[i2].position), XMLoadFloat4x4(&w)));
                }
            }
            else if (selectMode == selection::edge)
            {
                for (const auto& h : umap)
                {
                    i0 = h.second.Triangle * 3;
                    i1 = i0 + 1;
                    i2 = i0 + 2;

                    if (h.second.Component == 0)
                    {
                        allPos.emplace_back(XMVector3Transform(XMLoadFloat3(&vstore[i0].position), XMLoadFloat4x4(&w)));
                        allPos.emplace_back(XMVector3Transform(XMLoadFloat3(&vstore[i1].position), XMLoadFloat4x4(&w)));
                    }
                    else if (h.second.Component == 1)
                    {
                        allPos.emplace_back(XMVector3Transform(XMLoadFloat3(&vstore[i1].position), XMLoadFloat4x4(&w)));
                        allPos.emplace_back(XMVector3Transform(XMLoadFloat3(&vstore[i2].position), XMLoadFloat4x4(&w)));
                    }
                    else if (h.second.Component == 2)
                    {
                        allPos.emplace_back(XMVector3Transform(XMLoadFloat3(&vstore[i2].position), XMLoadFloat4x4(&w)));
                        allPos.emplace_back(XMVector3Transform(XMLoadFloat3(&vstore[i0].position), XMLoadFloat4x4(&w)));
                    }
                }
            }
            else if (selectMode == selection::vertex)
            {
                for (const auto& h : umap)
                {
                    i0 = h.second.Triangle * 3;
                    i1 = i0 + 1;
                    i2 = i0 + 2;

                    if (h.second.Component == 0)
                    {
                        allPos.emplace_back(XMVector3Transform(XMLoadFloat3(&vstore[i0].position), XMLoadFloat4x4(&w)));
                    }
                    else if (h.second.Component == 1)
                    {
                        allPos.emplace_back(XMVector3Transform(XMLoadFloat3(&vstore[i1].position), XMLoadFloat4x4(&w)));
                    }
                    else if (h.second.Component == 2)
                    {
                        allPos.emplace_back(XMVector3Transform(XMLoadFloat3(&vstore[i2].position), XMLoadFloat4x4(&w)));
                    }
                }
            }
        };
    

    for (const auto& ref : refOnRender) 
    {
        auto& geo = ref.second.begin()->get().Geo;
        auto vertices = reinterpret_cast<VertexStructs::VertexPositionNormalTextureTangentU*>(geo->VertexBufferCPU->GetBufferPointer());

        if (selectMode == selection::face)
            addPositions(ref.second.back().get().HoldTriangle, ref.second.front().get().Instances[0].World, vertices);
        else if (selectMode == selection::edge)
            addPositions(ref.second.back().get().HoldEdge, ref.second.front().get().Instances[0].World, vertices);
        else if (selectMode == selection::vertex)
            addPositions(ref.second.back().get().HoldVertex, ref.second.front().get().Instances[0].World, vertices);
    }

    if (allPos.empty())
    {
        m_onOff = false;
    }
    else
    {
        m_onOff = true;

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = minX, maxY = maxX;
        float minZ = minX, maxZ = maxX;

        for (const auto& pos : allPos)
        {
            minX = std::min(minX, pos.m128_f32[0]); maxX = std::max(maxX, pos.m128_f32[0]);
            minY = std::min(minY, pos.m128_f32[1]); maxY = std::max(maxY, pos.m128_f32[1]);
            minZ = std::min(minZ, pos.m128_f32[2]); maxZ = std::max(maxZ, pos.m128_f32[2]);
        }

        XMFLOAT3 center = { (minX + maxX) * 0.5f, (minY + maxY) * 0.5f, (minZ + maxZ) * 0.5f };

        m_attributes.Quaternion = refOnRender.begin()->second.front().get().Attributes[0].Quaternion;

        m_attributes.TranslateX = center.x;
        m_attributes.TranslateY = center.y;
        m_attributes.TranslateZ = center.z;

        m_pivotTr.PlaceThePivotInTheDesiredPosition(m_attributes);
        m_pivotRt.PlaceThePivotInTheDesiredPosition(m_attributes);
        m_pivotSc.PlaceThePivotInTheDesiredPosition(m_attributes, camerasLook);
    }   
}

void LisaApp::Pivot::PlaceThePivotInTheDesiredPosition(
    const RefOnRender& refOnRender, 
    const std::pair<std::uint32_t, std::uint32_t>& lastID,
    const DirectX::XMVECTOR& camerasLook
)
{
    SetPivotDirty(true);

    if (refOnRender.contains({ lastID.first, lastID.second }))
    {
        const auto& primitives = refOnRender.at({ lastID.first, lastID.second });
        Item::Attributes attr = primitives.front().get().Attributes[lastID.second];

        m_attributes.Quaternion = attr.Quaternion;

        m_attributes.TranslateX = attr.TranslateX;
        m_attributes.TranslateY = attr.TranslateY;
        m_attributes.TranslateZ = attr.TranslateZ;

        m_pivotTr.PlaceThePivotInTheDesiredPosition(m_attributes);
        m_pivotRt.PlaceThePivotInTheDesiredPosition(m_attributes);
        m_pivotSc.PlaceThePivotInTheDesiredPosition(m_attributes, camerasLook);
    } 
}

void LisaApp::Pivot::OnLButtonUp()
{
    using namespace LisaApp::Global;

    SetClickOnPivot(false);
    SetColorModeSelectedComponent(PivotColorMode::Default);
    ClearRefStorage();

    // Tr.

    m_pivotTr.LButtonUp(m_attributes);

    // Rt.

    m_pivotRt.LButtonUp(m_attributes);

    // Sc.

    m_pivotSc.LButtonUp(m_attributes);
}

bool LisaApp::Pivot::OnLButtonDown(
    const DirectX::XMMATRIX& getViev, 
    const DirectX::XMFLOAT4X4& proj4x4f, 
    std::int32_t sx,
    std::int32_t sy,
    std::int32_t screenWidth,
    std::int32_t screenHeight
)
{
    using namespace LisaApp::Global;

    bool isPivotSelected{};

    if (m_onOff)
    {
        isPivotSelected = Pick(getViev, proj4x4f, sx, sy, screenWidth, screenHeight);

        if (isPivotSelected)
        {
            m_clickOnPivot = true;
            SetColorModeSelectedComponent(PivotColorMode::Select);

            std::uint32_t objectID{};

            switch (m_mode)
            {
            case PivotMode::Translate:
                m_pivotTr.LButtonDown(m_attributes, m_refWrapOnRender, getViev, proj4x4f, sx, sy, screenWidth, screenHeight);
                objectID = m_pivotTr.GetObjectID();
            break;
            case PivotMode::Rotate:
                m_pivotRt.LButtonDown(m_attributes, m_refWrapOnRender, getViev, proj4x4f, sx, sy, screenWidth, screenHeight);
                objectID = m_pivotRt.GetObjectID();
            break;
            case PivotMode::Scale:
                m_pivotSc.LButtonDown(m_attributes, m_refWrapOnRender, getViev, proj4x4f, sx, sy, screenWidth, screenHeight);
                objectID = m_pivotSc.GetObjectID();
            break;
            default:
                break;
            }

            m_activeAxis = ProcessingObjectID(objectID);
        }
        
        // If we click past the pivot, it needs to be removed.
        if (!isPivotSelected)
        {
            m_onOff = false;
        }
    }

    return isPivotSelected;
}

bool LisaApp::Pivot::OnLButtonMove(
    const DirectX::XMMATRIX& getViev,
    const DirectX::XMFLOAT4X4& proj4x4f,
    std::int32_t sx,
    std::int32_t sy,
    std::int32_t screenWidth,
    std::int32_t screenHeight,
    IPivot::Attributes& attributes
)
{
    using namespace LisaApp::Global;

    bool r{};

    if (m_onOff)
    {
        switch (m_mode)
        {
        case PivotMode::Translate:
            SetPivotDirty(true);
            r = m_pivotTr.LButtonMove(getViev, proj4x4f, sx, sy, screenWidth, screenHeight, m_attributes);
            attributes = m_attributes;
            break;
        case PivotMode::Rotate:
            SetPivotDirty(false);
            r = m_pivotRt.LButtonMove(getViev, proj4x4f, sx, sy, screenWidth, screenHeight, m_attributes);
            attributes = m_attributes;
            break;
        case PivotMode::Scale:
            SetPivotDirty(false);
            r = m_pivotSc.LButtonMove(getViev, proj4x4f, sx, sy, screenWidth, screenHeight, m_attributes);
            attributes = m_attributes;
            break;
        default:
            break;
        }
    }

    return r;
}

bool LisaApp::Pivot::OnKeyDown(
    HWND hwnd, 
    WPARAM wParam, 
    std::uint32_t selectMode,
    bool isSelectObject, 
    const RefOnRender& refOnRender, 
    const std::pair<std::uint32_t, std::uint32_t>& lastID,
    const DirectX::XMVECTOR& camerasLook
)
{
    using namespace LisaApp::Global;

    if (wParam == 0x51) // Q
    {
        m_onOff = false;
        m_mode = PivotMode::Off;
    }
    else if (wParam == 0x57) // W
    {
        m_mode = PivotMode::Translate;

        if (isSelectObject)
        {
            m_onOff = true;

            if (selectMode == selection::mesh)
                PlaceThePivotInTheDesiredPosition(refOnRender, lastID, camerasLook);

            else if (selectMode == selection::vertex || selectMode == selection::edge || selectMode == selection::face)
                PlaceThePivotInTheDesiredPositionComponent(refOnRender, camerasLook, selectMode);
        }
    }
    else if (wParam == 0x45) // E
    {
        m_mode = PivotMode::Rotate;

        if (isSelectObject)
        {
            m_onOff = true;
            
            if (selectMode == selection::mesh)
                PlaceThePivotInTheDesiredPosition(refOnRender, lastID, camerasLook);

            else if (selectMode == selection::vertex || selectMode == selection::edge || selectMode == selection::face)
                PlaceThePivotInTheDesiredPositionComponent(refOnRender, camerasLook, selectMode);
        }
    }
    else if (wParam == 0x52) // R
    {
        m_mode = PivotMode::Scale;

        if (isSelectObject)
        {
            m_onOff = true;

            if (selectMode == selection::mesh)
                PlaceThePivotInTheDesiredPosition(refOnRender, lastID, camerasLook);

            else if (selectMode == selection::vertex || selectMode == selection::edge || selectMode == selection::face)
                PlaceThePivotInTheDesiredPositionComponent(refOnRender, camerasLook, selectMode);
        }
    }
    else
    {
        return false;
    }

    InvalidateRect(hwnd, NULL, false);

    return true;
}

IPivot::Axis LisaApp::Pivot::ProcessingObjectID(std::uint32_t objectID)
{
    using namespace LisaApp::Global;

    IPivot::Axis pa;

    switch (objectID)
    {
    case pivot_components::cone_x:
    case pivot_components::cube_x:
    case pivot_components::line_x:
        pa.AxisX = true;
        break;
    case pivot_components::cone_y:
    case pivot_components::cube_y:
    case pivot_components::line_y:
        pa.AxisY = true;
        break;
    case pivot_components::cone_z:
    case pivot_components::cube_z:
    case pivot_components::line_z:
        pa.AxisZ = true;
        break;
    case pivot_components::plane_x:
        pa.AxisX = true; pa.AxisY = true;
        break;
    case pivot_components::plane_y:
        pa.AxisX = true; pa.AxisZ = true;
        break;
    case pivot_components::plane_z:
        pa.AxisY = true; pa.AxisZ = true;
        break;
    case pivot_components::center_frame:
        pa.AxisX = true; pa.AxisY = true; pa.AxisZ = true;
        break;
    case pivot_components::circle_x:
        pa.AxisX = true;
        break;
    case pivot_components::circle_y:
        pa.AxisY = true;
        break;
    case pivot_components::circle_z:
        pa.AxisZ = true;
        break;
    case pivot_components::outer_circle:
        pa.OuterCircle = true;
        break;
    case pivot_components::sphere:
        pa.Sphere = true;
        break;
    case pivot_components::center_cube:
        pa.AxisX = true; pa.AxisY = true; pa.AxisZ = true;
        break;
    default:
        break;
    }

    return pa;
}

void LisaApp::Pivot::OnOffStep(bool onOff)
{
    using namespace LisaApp::Global;

    switch (m_mode)
    {
    case PivotMode::Translate:
        break;
    case PivotMode::Rotate:
        m_pivotRt.OnOffStep(onOff);
        break;
    case PivotMode::Scale:
        break;
    default:
        break;
    }
}

//wchar_t msg[128]{};
//swprintf_s(msg, L"x: % f\n", m_pivotStart.m128_f32[0]);
//OutputDebugString(msg);
//swprintf_s(msg, L"y: % f\n", m_pivotStart.m128_f32[1]);
//OutputDebugString(msg);
//swprintf_s(msg, L"z: % f\n", m_pivotStart.m128_f32[2]);
//OutputDebugString(msg);