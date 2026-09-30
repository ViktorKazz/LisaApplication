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

#include "PivotTranslate.h"
#include "HelperMath.h"
#include "AppColors.h"

void LisaApp::PivotTranslateImpl::Create(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    // Create planes to intersect with the ray.
    // This will be needed to move the pivot.
    m_tools.PlanesParallelToTheAxes();

    XMFLOAT4 pointColor{ AppColors::Color::PivotPoint };
    XMFLOAT4 frameColor{ AppColors::Color::PivotFrame };
    XMFLOAT4 xAxisColor{ AppColors::Color::PivotXAxis };
    XMFLOAT4 yAxisColor{ AppColors::Color::PivotYAxis };
    XMFLOAT4 zAxisColor{ AppColors::Color::PivotZAxis };

    const float coneRadius{ 0.22f };
    const float coneHeight{ 0.8f };

    const XMFLOAT3 linePointA{ 0.0f, 0.6f, 0.0f };
    const XMFLOAT3 linePointB{ 0.0f, gDefaultPivotRadius, 0.0f };

    const float width{ 0.6f };
    const float depth{ 0.6f };

    std::unique_ptr<PolygonPrimitives> pCenterFrame = PolygonPrimitives::CreatePlaneEasy(device, commandList, 1.0f, 1.0f, frameColor);
    pCenterFrame->OnRender[0].ID = pivot_components::center_frame;

    // To create similar primitives, we use object instances.

    std::unique_ptr<PolygonPrimitives> pCone = PolygonPrimitives::CreateConeEasy(device, commandList, coneRadius, coneHeight, 6, xAxisColor);
    pCone->OnRender[0].ID = pivot_components::cone;
    pCone->AddInstances(1);
    pCone->AddInstances(2);
    pCone->OnRender[0].Instances[1].Color = yAxisColor;
    pCone->OnRender[0].Instances[2].Color = zAxisColor;

    std::unique_ptr<PolygonPrimitives> pLine = PolygonPrimitives::CreateLine(device, commandList, linePointA, linePointB, xAxisColor);
    pLine->OnRender[0].ID = pivot_components::line;
    pLine->AddInstances(1);
    pLine->AddInstances(2);
    pLine->OnRender[0].Instances[1].Color = yAxisColor;
    pLine->OnRender[0].Instances[2].Color = zAxisColor;

    std::unique_ptr<PolygonPrimitives> pPlane = PolygonPrimitives::CreatePlaneEasy(device, commandList, width, depth, zAxisColor);
    pPlane->OnRender[0].ID = pivot_components::plane;
    pPlane->AddInstances(1);
    pPlane->AddInstances(2);
    pPlane->OnRender[0].Instances[1].Color = yAxisColor;
    pPlane->OnRender[0].Instances[2].Color = xAxisColor;

    // Create auxiliary components.
    // They are needed to show where the object was before its transformation.

    std::unique_ptr<PolygonPrimitives> pAuxiliaryLine = PolygonPrimitives::CreateLine(device, commandList, {}, { 0.0f , gDefaultPivotRadius, 0.0f });
    pAuxiliaryLine->OnRender[0].ID = pivot_components::aux_line;
    pAuxiliaryLine->AddInstances(1);
    pAuxiliaryLine->AddInstances(2);


    // Save the object matrices and arrange the objects.

    float move{ gDefaultPivotRadius };

    m_tools.SetAttributes(pivot_components::center_frame, pCenterFrame->OnRender[0].Instances[0].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    m_tools.SetAttributes(pivot_components::cone_x, pCone->OnRender[0].Instances[0].World, move, 0.0f, 0.0f, 0.0f, 0.0f, -90.0f);
    m_tools.SetAttributes(pivot_components::cone_y, pCone->OnRender[0].Instances[1].World, 0.0f, move, 0.0f, 0.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::cone_z, pCone->OnRender[0].Instances[2].World, 0.0f, 0.0f, -move, -90.0f, 0.0f, 0.0f);

    m_tools.SetAttributes(pivot_components::line_x, pLine->OnRender[0].Instances[0].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -90.0f);
    m_tools.SetAttributes(pivot_components::line_y, pLine->OnRender[0].Instances[1].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::line_z, pLine->OnRender[0].Instances[2].World, 0.0f, 0.0f, 0.0f, -90.0f, 0.0f, 0.0f);

    move = move * (gDefaultPivotRadius * 0.1f);

    m_tools.SetAttributes(pivot_components::plane_x, pPlane->OnRender[0].Instances[0].World, move, move, 0.0f, 90.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::plane_y, pPlane->OnRender[0].Instances[1].World, move, 0.0f, -move, 0.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::plane_z, pPlane->OnRender[0].Instances[2].World, 0.0f, move, -move, 0.0f, 0.0f, 90.0f);

    m_tools.SetAttributes(pivot_components::aux_line_x, pAuxiliaryLine->OnRender[0].Instances[0].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -90.0f);
    m_tools.SetAttributes(pivot_components::aux_line_y, pAuxiliaryLine->OnRender[0].Instances[1].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::aux_line_z, pAuxiliaryLine->OnRender[0].Instances[2].World, 0.0f, 0.0f, 0.0f, -90.0f, 0.0f, 0.0f);

    // Save the pivot components to a map.

    //m_storageTranslate.try_emplace(pivot_components::center_point, std::move(pCenterPoint));
    m_storageTranslate.try_emplace(pivot_components::center_frame, std::move(pCenterFrame));
    m_storageTranslate.try_emplace(pivot_components::cone, std::move(pCone));
    m_storageTranslate.try_emplace(pivot_components::line, std::move(pLine));
    m_storageTranslate.try_emplace(pivot_components::plane, std::move(pPlane));

    // For convenience, we place the auxiliary lines in a separate container.

    m_storageTranslateAux.try_emplace(pivot_components::aux_line, std::move(pAuxiliaryLine));

    // Initially, the auxiliary components are hidden.
    // They are displayed when a pivot component is selected and hidden when deselected.

    m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 0, true);
    m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 1, true);
    m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 2, true);
}

void LisaApp::PivotTranslateImpl::ScaleCenterFrame(const IPivot::Attributes& attributes, const DirectX::XMVECTOR& cameraPosition)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    XMVECTOR translation{ attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ, 0.0f };

    // The billboard method is used for rotation.

    // Vector from the camera to the center of the object.
    XMVECTOR look = XMVector3Normalize(XMVectorSubtract(translation, cameraPosition));
    // This is the global vector (0, 1, 0) (the vertical "top" of the scene).
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    // Constructing an orthogonal right and up axis using the cross product.
    XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, look));
    // Recalculate up for orthogonality.
    up = XMVector3Cross(look, right);
    // Forming a billboarding matrix.
    m_billboardingMatrix =
    {
        right.m128_f32[0], right.m128_f32[1], right.m128_f32[2], 0.0f,
        up.m128_f32[0],    up.m128_f32[1],    up.m128_f32[2],    0.0f,
        look.m128_f32[0],  look.m128_f32[1],  look.m128_f32[2],  0.0f,
        translation.m128_f32[0],   translation.m128_f32[1],   translation.m128_f32[2],   1.0f
    };

    // It is necessary that the object (circle) looks at the camera not as a line, but as a circle.
    XMMATRIX rotation = XMMatrixRotationX(XMConvertToRadians(90.0f));
    XMMATRIX scaling = XMMatrixScaling(attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ);

    XMMATRIX matrix = XMMatrixMultiply(XMMatrixMultiply(scaling, rotation), m_billboardingMatrix);

    XMStoreFloat4x4(&m_storageTranslate[pivot_components::center_frame]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::center_frame), matrix));
}

void LisaApp::PivotTranslateImpl::SelectingAdjacentPivotObjects(RefOnRender& refWrapOnRender) const
{
    using namespace LisaApp::Global;

    if (!refWrapOnRender.empty())
    {
        const auto& it = refWrapOnRender.begin();

        auto& [objectID, instancesID] = it->first;
        auto& onRender = it->second.front().get();

        if (onRender.ID == pivot_components::cone && instancesID == 0)
        {
            refWrapOnRender.insert({
                { m_storageTranslate.at(pivot_components::line)->OnRender[0].ID, instancesID },
                {m_storageTranslate.at(pivot_components::line)->OnRender[0]} });
        }
        else if (onRender.ID == pivot_components::line && instancesID == 0)
        {
            refWrapOnRender.insert({ 
                { m_storageTranslate.at(pivot_components::cone)->OnRender[0].ID, instancesID },
                {m_storageTranslate.at(pivot_components::cone)->OnRender[0]} });
        }
        else if (onRender.ID == pivot_components::cone && instancesID == 1)
        {
            refWrapOnRender.insert({ 
                { m_storageTranslate.at(pivot_components::line)->OnRender[0].ID, instancesID },
                {m_storageTranslate.at(pivot_components::line)->OnRender[0]} });
        }
        else if (onRender.ID == pivot_components::line && instancesID == 1)
        {
            refWrapOnRender.insert({ 
                { m_storageTranslate.at(pivot_components::cone)->OnRender[0].ID, instancesID },
                {m_storageTranslate.at(pivot_components::cone)->OnRender[0]} });
        }
        else if (onRender.ID == pivot_components::cone && instancesID == 2)
        {
            refWrapOnRender.insert({ 
                { m_storageTranslate.at(pivot_components::line)->OnRender[0].ID, instancesID },
                {m_storageTranslate.at(pivot_components::line)->OnRender[0]} });
        }
        else if (onRender.ID == pivot_components::line && instancesID == 2)
        {
            refWrapOnRender.insert({ 
                { m_storageTranslate.at(pivot_components::cone)->OnRender[0].ID, instancesID },
                {m_storageTranslate.at(pivot_components::cone)->OnRender[0]} });
        }
    }   
}

void LisaApp::PivotTranslateImpl::UpdateMatrix(const DirectX::XMMATRIX& matrix)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    XMStoreFloat4x4(&m_storageTranslate[pivot_components::cone]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::cone_x), matrix));
    XMStoreFloat4x4(&m_storageTranslate[pivot_components::cone]->OnRender[0].Instances[1].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::cone_y), matrix));
    XMStoreFloat4x4(&m_storageTranslate[pivot_components::cone]->OnRender[0].Instances[2].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::cone_z), matrix));

    XMStoreFloat4x4(&m_storageTranslate[pivot_components::line]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::line_x), matrix));
    XMStoreFloat4x4(&m_storageTranslate[pivot_components::line]->OnRender[0].Instances[1].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::line_y), matrix));
    XMStoreFloat4x4(&m_storageTranslate[pivot_components::line]->OnRender[0].Instances[2].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::line_z), matrix));

    XMStoreFloat4x4(&m_storageTranslate[pivot_components::plane]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::plane_x), matrix));
    XMStoreFloat4x4(&m_storageTranslate[pivot_components::plane]->OnRender[0].Instances[1].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::plane_y), matrix));
    XMStoreFloat4x4(&m_storageTranslate[pivot_components::plane]->OnRender[0].Instances[2].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::plane_z), matrix));
}

void LisaApp::PivotTranslateImpl::Scale(const IPivot::Attributes& attributes, const DirectX::XMVECTOR& cameraPosition)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    XMMATRIX scaling = XMMatrixScaling(attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ);
    XMMATRIX translation = XMMatrixTranslation(attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ);

    XMMATRIX matrix = scaling * translation;

    UpdateMatrix(matrix);

    ScaleCenterFrame(attributes, cameraPosition);
}

// Function for hiding pivot components depending on the camera angle.
void LisaApp::PivotTranslateImpl::HidingPivotComponents(const DirectX::XMVECTOR& camerasLook)
{
    using namespace LisaApp::HelperMath;
    using namespace LisaApp::Global;
    using namespace DirectX;

    XMVECTOR direction = XMVectorSet(-camerasLook.m128_f32[0], -camerasLook.m128_f32[1], -camerasLook.m128_f32[2], 0.0f);
    XMVECTOR normalDirection = XMVector3Normalize(direction);

    float angleXY = FindingTheAngleBetweenRayAndPlane(normalDirection, m_tools.GetPlaneXY());
    float angleYZ = FindingTheAngleBetweenRayAndPlane(normalDirection, m_tools.GetPlaneYZ());
    float angleXZ = FindingTheAngleBetweenRayAndPlane(normalDirection, m_tools.GetPlaneXZ());

    angleXY = XMConvertToDegrees(angleXY);
    angleYZ = XMConvertToDegrees(angleYZ);
    angleXZ = XMConvertToDegrees(angleXZ);

    m_tools.Hide(m_storageTranslate, angleXY, angleXZ, pivot_components::cone, pivot_components::line, 0, 0, m_hideX);
    m_tools.Hide(m_storageTranslate, angleXY, angleYZ, pivot_components::cone, pivot_components::line, 1, 1, m_hideY);
    m_tools.Hide(m_storageTranslate, angleYZ, angleXZ, pivot_components::cone, pivot_components::line, 2, 2, m_hideZ);

    m_tools.Hide(m_storageTranslate, angleXY, pivot_components::plane, 0, m_hidePlaneX);
    m_tools.Hide(m_storageTranslate, angleXZ, pivot_components::plane, 1, m_hidePlaneY);
    m_tools.Hide(m_storageTranslate, angleYZ, pivot_components::plane, 2, m_hidePlaneZ);
}

// The pivot will be placed in the center of the selected object.
void LisaApp::PivotTranslateImpl::PlaceThePivotInTheDesiredPosition(const IPivot::Attributes& attributes)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    XMMATRIX translation = XMMatrixTranslation(attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ);
    
    // Update the plane matrix.
    // When a new object is created, the pivot moves to this object and, 
    // accordingly, the matrix must also change position.

    m_planeMatrix = translation;

    XMStoreFloat4x4(&m_storageTranslate[pivot_components::center_frame]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::center_frame), translation));

    UpdateMatrix(translation);
}

void LisaApp::PivotTranslateImpl::LButtonUp(const IPivot::Attributes& attributes)
{
    using namespace LisaApp::Global;

    // Hide the auxiliary components and move them to the pivot location.

    m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 0, true);
    m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 1, true);
    m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 2, true);

    // Update the plane matrix. 
    // This will allow the pivot to always be in the center (at the base) of the planes.

    m_planeMatrix.r[3].m128_f32[0] = attributes.TranslateX;
    m_planeMatrix.r[3].m128_f32[1] = attributes.TranslateY;
    m_planeMatrix.r[3].m128_f32[2] = attributes.TranslateZ;
}

// The function calculates the intersection point between a ray and a plane.
// The plane is determined based on the selected pivot component.
DirectX::XMVECTOR LisaApp::PivotTranslateImpl::PointOnThePlane(
    const IPivot::Attributes& attributes,
    const DirectX::XMMATRIX& getViev,
    const DirectX::XMFLOAT4X4& proj4x4f,
    std::int32_t sx,
    std::int32_t sy,
    std::int32_t screenWidth,
    std::int32_t screenHeight
)
{
    using namespace LisaApp::HelperMath;
    using namespace LisaApp::Global;
    using namespace DirectX;

    DirectX::XMVECTOR rayOrigin;
    DirectX::XMVECTOR rayDir;

    CalculatingRays(getViev, proj4x4f, m_planeMatrix, rayOrigin, rayDir, sx, sy, screenWidth, screenHeight);


    std::unordered_map<float, XMVECTOR> planeStore;

    // Why calculate the angle between a plane and a ray?
    // To select the plane that is less inclined relative to the camera.
    // With such a plane, the calculation of the intersection with the ray is more correct.

    float angleXY = FindingTheAngleBetweenRayAndPlane(rayDir, m_tools.GetPlaneXY());
    angleXY = XMConvertToDegrees(angleXY);
    planeStore.insert({ fabs(angleXY), m_tools.GetPlaneXY() });

    float angleYZ = FindingTheAngleBetweenRayAndPlane(rayDir, m_tools.GetPlaneYZ());
    angleYZ = XMConvertToDegrees(angleYZ);
    planeStore.insert({ fabs(angleYZ), m_tools.GetPlaneYZ() });

    float angleXZ = FindingTheAngleBetweenRayAndPlane(rayDir, m_tools.GetPlaneXZ());
    angleXZ = XMConvertToDegrees(angleXZ);
    planeStore.insert({ fabs(angleXZ), m_tools.GetPlaneXZ() });


    DirectX::XMVECTOR intersect{};

    // Find the maximum obtuse angle between the ray and the plane.
    float angle{};

    switch (m_objectID)
    {
    case pivot_components::cone_x:
    case pivot_components::line_x:
        angle = std::max(fabs(angleXZ), fabs(angleXY));
        intersect = IntersectRayPlane(rayOrigin, rayDir, planeStore[angle]);
        break;
    case pivot_components::cone_y:
    case pivot_components::line_y:
        angle = std::max(fabs(angleYZ), fabs(angleXY));
        intersect = IntersectRayPlane(rayOrigin, rayDir, planeStore[angle]);
        break;
    case pivot_components::cone_z:
    case pivot_components::line_z:
        angle = std::max(fabs(angleXZ), fabs(angleYZ));
        intersect = IntersectRayPlane(rayOrigin, rayDir, planeStore[angle]);
        break;
    case pivot_components::plane_x:
        angle = fabs(angleXY);
        intersect = IntersectRayPlane(rayOrigin, rayDir, planeStore[angle]);
        break;
    case pivot_components::plane_y:
        angle = fabs(angleXZ);
        intersect = IntersectRayPlane(rayOrigin, rayDir, planeStore[angle]);
        break;
    case pivot_components::plane_z:
        angle = fabs(angleYZ);
        intersect = IntersectRayPlane(rayOrigin, rayDir, planeStore[angle]);
        break;
    case pivot_components::center_frame:
    {
        // The normal for the plane will be the camera's viewing direction vector. 
        // A minus sign before the vector's components indicates that the normal
        // will point in the opposite direction from the viewing vector. 
        // That is, if the camera is "looking" in the direction of the gaze vector, 
        // the normal with a negative sign will "look" in the direction from which the camera's gaze comes.
        XMVECTOR nrm = XMVectorSet(
            -getViev.r[0].m128_f32[2],
            -getViev.r[1].m128_f32[2],
            -getViev.r[2].m128_f32[2],
            0.0f
        );
        XMVECTOR n = XMVector3Normalize(nrm);
        XMVECTOR planeXYZ = XMPlaneFromPointNormal(
            XMVectorSet(attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ, 0.0f), n);
        intersect = IntersectRayPlane(rayOrigin, rayDir, planeXYZ);
    }
    break;
    default:
        break;
    }

    // The function will return a zero vector if the ray is parallel to the plane, 
    // lies on the plane, or the plane has a small angle relative to the camera.
    bool result = XMVector3Equal(intersect, XMVectorZero());

    if (result)
        return XMVectorZero();

    return intersect;
}

void LisaApp::PivotTranslateImpl::LButtonDown(
    const IPivot::Attributes& attributes,
    const RefOnRender& refWrapOnRender,
    const DirectX::XMMATRIX& getViev,
    const DirectX::XMFLOAT4X4& proj4x4f,
    std::int32_t sx,
    std::int32_t sy,
    std::int32_t screenWidth,
    std::int32_t screenHeight
)
{
    using namespace LisaApp::HelperMath;
    using namespace LisaApp::Global;
    using namespace DirectX;

    // Update the matrix of auxiliary components before they appear.
    {
        XMMATRIX scaling = XMMatrixScaling(attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ);
        XMMATRIX translation = XMMatrixTranslation(attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ);

        XMMATRIX matrix = scaling * translation;

        for (const auto& st : m_storageTranslateAux)
        {
            auto& [objectID, primitives] = st;

            for (auto&& [iter, instance] : primitives.get()->OnRender[0].Instances | std::views::enumerate)
                XMStoreFloat4x4(
                    &instance.World, 
                    XMMatrixMultiply(m_tools.GetMatrixFromStorage(objectID + (static_cast<std::uint32_t>(iter) + 1)), matrix)
                );
        }
    }

    // Determine which axis was selected.

    m_objectID = 0;

    const auto& it = refWrapOnRender.begin();
    
    auto& [objectID, instancesID] = it->first;
    auto& onRender = it->second.front().get();
    
    // Since instances are used, the object ID needs to be updated.

    if (onRender.ID == pivot_components::cone)
    {
        if (instancesID == 0)
            m_objectID = pivot_components::cone_x;
        if (instancesID == 1)
            m_objectID = pivot_components::cone_y;
        if (instancesID == 2)
            m_objectID = pivot_components::cone_z;
    }
    else if (onRender.ID == pivot_components::line)
    {
        if (instancesID == 0)
            m_objectID = pivot_components::line_x;
        if (instancesID == 1)
            m_objectID = pivot_components::line_y;
        if (instancesID == 2)
            m_objectID = pivot_components::line_z;
    }
    else if (onRender.ID == pivot_components::plane)
    {
        if (instancesID == 0)
            m_objectID = pivot_components::plane_x;
        if (instancesID == 1)
            m_objectID = pivot_components::plane_y;
        if (instancesID == 2)
            m_objectID = pivot_components::plane_z;
    }
    else
    {
        m_objectID = onRender.ID;
    } 

    switch (m_objectID)
    {
    case pivot_components::cone_x:
    case pivot_components::line_x:
        m_axisX = true;
        m_axisY = false;
        m_axisZ = false;
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 1, false);
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 2, false);
        break;
    case pivot_components::cone_y:
    case pivot_components::line_y:
        m_axisX = false;
        m_axisY = true;
        m_axisZ = false;
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 0, false);
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 2, false);
        break;
    case pivot_components::cone_z:
    case pivot_components::line_z:
        m_axisX = false;
        m_axisY = false;
        m_axisZ = true;
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 0, false);
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 1, false);
        break;
    case pivot_components::plane_x:
        m_axisX = true;
        m_axisY = true;
        m_axisZ = false;
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 0, false);
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 1, false);
        break;
    case pivot_components::plane_y:
        m_axisX = true;
        m_axisY = false;
        m_axisZ = true;
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 0, false);
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 2, false);
        break;
    case pivot_components::plane_z:
        m_axisX = false;
        m_axisY = true;
        m_axisZ = true;
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 1, false);
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 2, false);
        break;
    case pivot_components::center_frame:
        m_axisX = true;
        m_axisY = true;
        m_axisZ = true;
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 0, false);
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 1, false);
        m_tools.HideOrVisibleComponents(m_storageTranslateAux, pivot_components::aux_line, 2, false);
        break;
    default:
        break;
    }

    m_pivotStart = PointOnThePlane(attributes, getViev, proj4x4f, sx, sy, screenWidth, screenHeight);
}

bool LisaApp::PivotTranslateImpl::LButtonMove(
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
    using namespace DirectX;

    m_pivotEnd = PointOnThePlane(attributes, getViev, proj4x4f, sx, sy, screenWidth, screenHeight);

    XMVECTOR difference = XMVectorSubtract(m_pivotEnd, m_pivotStart);

    // Update the pivot attributes.

    if (m_axisX)
    {
        float x{ XMVectorGetX(difference) };
        attributes.TranslateX += x;
        attributes.OutputTranslateX = x;
    }
    if (m_axisY)
    {
        float y{ XMVectorGetY(difference) };
        attributes.TranslateY += y;
        attributes.OutputTranslateY = y;
    }
    if (m_axisZ)
    {
        float z{ XMVectorGetZ(difference) };
        attributes.TranslateZ += z;
        attributes.OutputTranslateZ = z;
    }

    XMMATRIX scaling = XMMatrixScaling(attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ);
    XMMATRIX translation = XMMatrixTranslation(attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ);

    XMMATRIX matrix = scaling * translation;

    XMStoreFloat4x4(&m_storageTranslate[pivot_components::center_frame]->OnRender[0].Instances[0].World, 
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::center_frame), matrix));

    UpdateMatrix(matrix);

    m_pivotStart = m_pivotEnd;

    return true;
}

void LisaApp::PivotTranslateImpl::Draw(
    _In_ ID3D12GraphicsCommandList* commandList,
    _In_ ID3D12PipelineState* centerFrame,
    _In_ ID3D12PipelineState* mesh,
    _In_ ID3D12PipelineState* line
)
{
    using namespace LisaApp::Global;

    m_storageTranslate[pivot_components::center_frame]->Draw(commandList, { centerFrame });
    m_storageTranslate[pivot_components::cone]->Draw(commandList, { mesh });
    m_storageTranslate[pivot_components::line]->Draw(commandList, { line });
    m_storageTranslate[pivot_components::plane]->Draw(commandList, { mesh });

    m_storageTranslateAux[pivot_components::aux_line]->Draw(commandList, { line });
}

void LisaApp::PivotTranslateImpl::UpdateCB(
    const DirectX::XMMATRIX& getViev,
    const DirectX::BoundingFrustum& camFrustum,
    bool frustumCullingEnabled
)
{
    for (const auto& po : m_storageTranslate)
        po.second->UpdateBuffer(getViev, camFrustum, frustumCullingEnabled);

    for (const auto& po : m_storageTranslateAux)
        po.second->UpdateBuffer(getViev, camFrustum, frustumCullingEnabled);
}