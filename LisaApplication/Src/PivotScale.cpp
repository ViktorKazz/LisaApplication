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

#include "PivotScale.h"
#include "HelperMath.h"
#include "AppColors.h"

void LisaApp::PivotScaleImpl::Create(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    // Create planes to intersect with the ray.
    // This will be needed to move the pivot.
    m_tools.PlanesParallelToTheAxes();

    XMFLOAT4 auxLineColor{ AppColors::Color::DarkBlue };
    XMFLOAT4 frameColor{ AppColors::Color::PivotFrame };
    XMFLOAT4 xAxisColor{ AppColors::Color::PivotXAxis };
    XMFLOAT4 yAxisColor{ AppColors::Color::PivotYAxis };
    XMFLOAT4 zAxisColor{ AppColors::Color::PivotZAxis };

    const XMFLOAT3 linePointA{ 0.0f, 0.0f, 0.0f };
    const XMFLOAT3 linePointB{ 0.0f, gDefaultPivotRadius, 0.0f };

    const float width{ 0.6f };
    const float height{ 0.6f };
    const float depth{ 0.6f };


    std::unique_ptr<PolygonPrimitives> pCenterCube = PolygonPrimitives::CreateBoxEasy(device, commandList, width, height, depth, frameColor);
    pCenterCube->OnRender[0].ID = pivot_components::center_cube;

    // To create similar primitives, we use object instances.

    std::unique_ptr<PolygonPrimitives> pCube = PolygonPrimitives::CreateBoxEasy(device, commandList, width, height, depth, xAxisColor);
    pCube->OnRender[0].ID = pivot_components::cube;
    pCube->AddInstances(1);
    pCube->AddInstances(2);
    pCube->OnRender[0].Instances[1].Color = yAxisColor;
    pCube->OnRender[0].Instances[2].Color = zAxisColor;

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

    std::unique_ptr<PolygonPrimitives> pAuxiliaryLine = PolygonPrimitives::CreateLine(device, commandList, {}, { 0.0f , gDefaultPivotRadius, 0.0f }, auxLineColor);
    pAuxiliaryLine->OnRender[0].ID = pivot_components::aux_line;
    pAuxiliaryLine->AddInstances(1);
    pAuxiliaryLine->AddInstances(2);
    pAuxiliaryLine->OnRender[0].Instances[1].Color = auxLineColor;
    pAuxiliaryLine->OnRender[0].Instances[2].Color = auxLineColor;

    // Save the object matrices and arrange the objects.

    float move{ gDefaultPivotRadius };

    m_tools.SetAttributes(pivot_components::center_cube, pCenterCube->OnRender[0].Instances[0].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    m_tools.SetAttributes(pivot_components::cube_x, pCube->OnRender[0].Instances[0].World, move, 0.0f, 0.0f, 0.0f, 0.0f, -90.0f);
    m_tools.SetAttributes(pivot_components::cube_y, pCube->OnRender[0].Instances[1].World, 0.0f, move, 0.0f, 0.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::cube_z, pCube->OnRender[0].Instances[2].World, 0.0f, 0.0f, -move, -90.0f, 0.0f, 0.0f);

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

    m_tools.SetAttributes(pivot_components::aux_plane, m_plane, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    // Save the pivot components to a map.

    m_storageScale.try_emplace(pivot_components::center_cube, std::move(pCenterCube));
    m_storageScale.try_emplace(pivot_components::cube, std::move(pCube));
    m_storageScale.try_emplace(pivot_components::line, std::move(pLine));
    m_storageScale.try_emplace(pivot_components::plane, std::move(pPlane));

    // For convenience, we place the auxiliary lines in a separate container.

    m_storageScaleAux.try_emplace(pivot_components::aux_line, std::move(pAuxiliaryLine));

    // Initially, the auxiliary components are hidden.
    // They are displayed when a pivot component is selected and hidden when deselected.

    m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 0, true);
    m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 1, true);
    m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 2, true);
}

void LisaApp::PivotScaleImpl::SelectingAdjacentPivotObjects(RefOnRender& refWrapOnRender)
{
    using namespace LisaApp::Global;

    if (!refWrapOnRender.empty())
    {
        const auto& it = refWrapOnRender.begin();

        auto& [objectID, instancesID] = it->first;
        auto& onRender = it->second.front().get();

        if (onRender.ID == pivot_components::cube && instancesID == 0)
        {
            refWrapOnRender.insert({ 
                { m_storageScale[pivot_components::line]->OnRender[0].ID, instancesID }, 
                {m_storageScale[pivot_components::line]->OnRender[0]} });
        }
        else if (onRender.ID == pivot_components::line && instancesID == 0)
        {
            refWrapOnRender.insert({ 
                { m_storageScale[pivot_components::cube]->OnRender[0].ID, instancesID }, 
                {m_storageScale[pivot_components::cube]->OnRender[0]} });
        }
        else if (onRender.ID == pivot_components::cube && instancesID == 1)
        {
            refWrapOnRender.insert({ 
                { m_storageScale[pivot_components::line]->OnRender[0].ID, instancesID }, 
                {m_storageScale[pivot_components::line]->OnRender[0]} });
        }
        else if (onRender.ID == pivot_components::line && instancesID == 1)
        {
            refWrapOnRender.insert({ 
                { m_storageScale[pivot_components::cube]->OnRender[0].ID, instancesID }, 
                {m_storageScale[pivot_components::cube]->OnRender[0]} });
        }
        else if (onRender.ID == pivot_components::cube && instancesID == 2)
        {
            refWrapOnRender.insert({ 
                { m_storageScale[pivot_components::line]->OnRender[0].ID, instancesID }, 
                {m_storageScale[pivot_components::line]->OnRender[0]} });
        }
        else if (onRender.ID == pivot_components::line && instancesID == 2)
        {
            refWrapOnRender.insert({ 
                { m_storageScale[pivot_components::cube]->OnRender[0].ID, instancesID }, 
                {m_storageScale[pivot_components::cube]->OnRender[0]} });
        }
    }
}

void LisaApp::PivotScaleImpl::UpdateMatrix(const DirectX::XMMATRIX& matrix)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    XMStoreFloat4x4(&m_storageScale[pivot_components::center_cube]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::center_cube), matrix));

    XMStoreFloat4x4(&m_storageScale[pivot_components::cube]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::cube_x), matrix));
    XMStoreFloat4x4(&m_storageScale[pivot_components::cube]->OnRender[0].Instances[1].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::cube_y), matrix));
    XMStoreFloat4x4(&m_storageScale[pivot_components::cube]->OnRender[0].Instances[2].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::cube_z), matrix));

    XMStoreFloat4x4(&m_storageScale[pivot_components::line]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::line_x), matrix));
    XMStoreFloat4x4(&m_storageScale[pivot_components::line]->OnRender[0].Instances[1].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::line_y), matrix));
    XMStoreFloat4x4(&m_storageScale[pivot_components::line]->OnRender[0].Instances[2].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::line_z), matrix));

    XMStoreFloat4x4(&m_storageScale[pivot_components::plane]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::plane_x), matrix));
    XMStoreFloat4x4(&m_storageScale[pivot_components::plane]->OnRender[0].Instances[1].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::plane_y), matrix));
    XMStoreFloat4x4(&m_storageScale[pivot_components::plane]->OnRender[0].Instances[2].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::plane_z), matrix));
}

void LisaApp::PivotScaleImpl::Scale(const IPivot::Attributes& attributes, const DirectX::XMVECTOR& cameraPosition)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    m_cameraPosition = cameraPosition;

    XMMATRIX matrix = XMMatrixAffineTransformation(
        XMVectorSet(attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ, 0.0f),
        XMVectorZero(),
        attributes.Quaternion,
        XMVectorSet(attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ, 0.0f)
    );

    UpdateMatrix(matrix);
}

// Function for hiding pivot components depending on the camera angle.
void LisaApp::PivotScaleImpl::HidingPivotComponents(const DirectX::XMVECTOR& camerasLook)
{
    using namespace LisaApp::HelperMath;
    using namespace LisaApp::Global;
    using namespace DirectX;

    XMVECTOR direction = XMVectorSet(-camerasLook.m128_f32[0], -camerasLook.m128_f32[1], -camerasLook.m128_f32[2], 0.0f);
    XMVECTOR normalDirection = XMVector3Normalize(direction);

    // Since the scaling pivot rotates, to correctly hide the components we will convert the planes to a matrix m_plane.

    DirectX::XMMATRIX M = m_plane;
    DirectX::XMVECTOR MM{ XMMatrixDeterminant(M) };
    DirectX::XMMATRIX invWorld = DirectX::XMMatrixInverse(&MM, M);

    invWorld = DirectX::XMMatrixTranspose(invWorld);

    XMVECTOR plXY = XMPlaneTransform(m_tools.GetPlaneXY(), invWorld);
    XMVECTOR plYZ = XMPlaneTransform(m_tools.GetPlaneYZ(), invWorld);
    XMVECTOR plXZ = XMPlaneTransform(m_tools.GetPlaneXZ(), invWorld);

    float angleXY = FindingTheAngleBetweenRayAndPlane(normalDirection, plXY);
    float angleYZ = FindingTheAngleBetweenRayAndPlane(normalDirection, plYZ);
    float angleXZ = FindingTheAngleBetweenRayAndPlane(normalDirection, plXZ);

    angleXY = XMConvertToDegrees(angleXY);
    angleYZ = XMConvertToDegrees(angleYZ);
    angleXZ = XMConvertToDegrees(angleXZ);

    m_tools.Hide(m_storageScale, angleXY, angleXZ, pivot_components::cube, pivot_components::line, 0, 0, m_hideX);
    m_tools.Hide(m_storageScale, angleXY, angleYZ, pivot_components::cube, pivot_components::line, 1, 1, m_hideY);
    m_tools.Hide(m_storageScale, angleYZ, angleXZ, pivot_components::cube, pivot_components::line, 2, 2, m_hideZ);

    m_tools.Hide(m_storageScale, angleXY, pivot_components::plane, 0, m_hidePlaneX);
    m_tools.Hide(m_storageScale, angleXZ, pivot_components::plane, 1, m_hidePlaneY);
    m_tools.Hide(m_storageScale, angleYZ, pivot_components::plane, 2, m_hidePlaneZ);
}

// The pivot will be placed in the center of the selected object.
void LisaApp::PivotScaleImpl::PlaceThePivotInTheDesiredPosition(
    const IPivot::Attributes& attributes, 
    const DirectX::XMVECTOR& camerasLook
)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    XMMATRIX matrix = XMMatrixAffineTransformation(
        XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
        XMVectorZero(),
        attributes.Quaternion,
        XMVectorSet(attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ, 0.0f));

    UpdateMatrix(matrix);

    // The object can have a rotation, therefore the m_plane matrix must also be rotated
    m_plane = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_plane), matrix);

    // The pivot components were hidden when it was on one object,
    // but on another object the same components can be shown because the scaling pivot has a rotation.

    HidingPivotComponents(camerasLook);
}

void LisaApp::PivotScaleImpl::ComputeDotAndScale(
    const IPivot::Attributes& attributes,
    const DirectX::XMFLOAT4X4& matrixf, 
    const DirectX::XMVECTOR& normal,
    float& dot, 
    float& scale
)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    XMVECTOR translate{ attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ };

    XMVECTOR v = XMVectorSubtract(XMVectorSet(matrixf._41, matrixf._42, matrixf._43, 0.0f), translate);
    XMVECTOR l = XMVector3Length(v);

    XMVECTOR nv = XMVector3Normalize(v);
    XMVECTOR d = XMVector3Dot(normal, nv);

    dot = XMVectorGetX(d);
    scale = XMVectorGetX(l) / gDefaultPivotRadius;
}

// The function allows you to move the axis cubes and scale the axis lines.
void LisaApp::PivotScaleImpl::MovementCubeMatrix(const IPivot::Attributes& attributes, std::int32_t x, std::int32_t y, std::int32_t z)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    XMMATRIX matrix = XMMatrixAffineTransformation(
        XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
        XMVectorZero(),
        attributes.Quaternion,
        XMVectorSet(attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ, 0.0f));

    XMMATRIX scaling = XMMatrixScaling(attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ);
    
    XMMATRIX m{};
    XMMATRIX ms{};

    // If the dot is negative, the line is drawn in the opposite direction.
    float dot{};
    float scale{};

    // If the above condition is met, then the dot and scale variables 
    // can be reused without a new call to the ComputeDotAndScale function.

    if (x == 0)
    {
        m = XMMatrixMultiply(XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::cube_x), scaling), XMMatrixTranslation(m_offsetX, 0.0f, 0.0f));
        XMStoreFloat4x4(&m_storageScale[pivot_components::cube]->OnRender[0].Instances[0].World, XMMatrixMultiply(m, matrix));

        ComputeDotAndScale(attributes, m_storageScale[pivot_components::cube]->OnRender[0].Instances[0].World, 
            m_normalAuxLineX, dot, scale);

        if (dot < 0)
            ms = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::line_x), XMMatrixScaling(-1, 1, -1));
        else
            ms = m_tools.GetMatrixFromStorage(pivot_components::line_x);
        

        ms = XMMatrixMultiply(ms, XMMatrixScaling(scale, scale, scale));
        XMStoreFloat4x4(&m_storageScale[pivot_components::line]->OnRender[0].Instances[0].World, XMMatrixMultiply(ms, matrix));
    }
    if (y == 1)
    {
        m = XMMatrixMultiply(XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::cube_y), scaling), XMMatrixTranslation(0.0f, m_offsetY, 0.0f));
        XMStoreFloat4x4(&m_storageScale[pivot_components::cube]->OnRender[0].Instances[1].World, XMMatrixMultiply(m, matrix));
        
        if (x != 0)
            ComputeDotAndScale(attributes, m_storageScale[pivot_components::cube]->OnRender[0].Instances[1].World, 
                m_normalAuxLineY, dot, scale);

        if (dot < 0)
            ms = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::line_y), XMMatrixScaling(1, -1, -1));
        else
            ms = m_tools.GetMatrixFromStorage(pivot_components::line_y);


        ms = XMMatrixMultiply(ms, XMMatrixScaling(scale, scale, scale));
        XMStoreFloat4x4(&m_storageScale[pivot_components::line]->OnRender[0].Instances[1].World, XMMatrixMultiply(ms, matrix));
    }
    if (z == 2)
    {
        m = XMMatrixMultiply(XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::cube_z), scaling), XMMatrixTranslation(0.0f, 0.0f, m_offsetZ));
        XMStoreFloat4x4(&m_storageScale[pivot_components::cube]->OnRender[0].Instances[2].World, XMMatrixMultiply(m, matrix));

        if (x != 0 || y != 1)
            ComputeDotAndScale(attributes, m_storageScale[pivot_components::cube]->OnRender[0].Instances[2].World, 
                m_normalAuxLineZ, dot, scale);

        if (dot < 0)
            ms = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::line_z), XMMatrixScaling(-1, 1, -1));
        else
            ms = m_tools.GetMatrixFromStorage(pivot_components::line_z);


        ms = XMMatrixMultiply(ms, XMMatrixScaling(scale, scale, scale));
        XMStoreFloat4x4(&m_storageScale[pivot_components::line]->OnRender[0].Instances[2].World, XMMatrixMultiply(ms, matrix));
    }
}

void LisaApp::PivotScaleImpl::LButtonUp(const IPivot::Attributes& attributes)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    // Hide the auxiliary components and move them to the pivot location.

    m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 0, true);
    m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 1, true);
    m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 2, true);

    // 

    m_offsetX = 0.0f;
    m_offsetY = 0.0f;
    m_offsetZ = 0.0f;

    // Set the vectors to zero so that the scalar product is equal to zero.

    m_normalAuxLineX = XMVectorZero();
    m_normalAuxLineY = XMVectorZero();
    m_normalAuxLineZ = XMVectorZero();

    // Return the lines and cubes to their normal state.

    MovementCubeMatrix(attributes, 0, -1, -1);
    MovementCubeMatrix(attributes, -1, 1, -1);
    MovementCubeMatrix(attributes, -1, -1, 2);
}

// The function calculates the intersection point between a ray and a plane.
// The plane is determined based on the selected pivot component.
DirectX::XMVECTOR LisaApp::PivotScaleImpl::PointOnThePlane(
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

    CalculatingRays(getViev, proj4x4f, m_plane, rayOrigin, rayDir, sx, sy, screenWidth, screenHeight);


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
    case pivot_components::cube_x:
    case pivot_components::line_x:
        angle = std::max(fabs(angleXZ), fabs(angleXY));
        intersect = IntersectRayPlane(rayOrigin, rayDir, planeStore[angle]);
        break;
    case pivot_components::cube_y:
    case pivot_components::line_y:
        angle = std::max(fabs(angleYZ), fabs(angleXY));
        intersect = IntersectRayPlane(rayOrigin, rayDir, planeStore[angle]);
        break;
    case pivot_components::cube_z:
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
    case pivot_components::center_cube:
    {
        // The billboard method is used for rotation.

        XMVECTOR translation{ attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ, 0.0f };

        // Vector from the camera to the center of the object.
        XMVECTOR look = XMVector3Normalize(XMVectorSubtract(translation, m_cameraPosition));
        // This is the global vector (0, 1, 0) (the vertical "top" of the scene).
        XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        // Constructing an orthogonal right and up axis using the cross product.
        XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, look));
        // Recalculate up for orthogonality.
        up = XMVector3Cross(look, right);
        // Forming a billboarding matrix.
        XMMATRIX billboardingMatrix =
        {
            right.m128_f32[0], right.m128_f32[1], right.m128_f32[2], 0.0f,
            up.m128_f32[0],    up.m128_f32[1],    up.m128_f32[2],    0.0f,
            look.m128_f32[0],  look.m128_f32[1],  look.m128_f32[2],  0.0f,
            translation.m128_f32[0],   translation.m128_f32[1],   translation.m128_f32[2],   1.0f
        };

        // It is necessary that the object (circle) looks at the camera not as a line, but as a circle.
        XMMATRIX rotation = XMMatrixRotationX(XMConvertToRadians(90.0f));
        XMMATRIX scaling = XMMatrixScaling(attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ);

        XMMATRIX matrix = XMMatrixMultiply(XMMatrixMultiply(scaling, rotation), billboardingMatrix);

        CalculatingRays(getViev, proj4x4f, matrix, rayOrigin, rayDir, sx, sy, screenWidth, screenHeight);

        // The normal for the plane will be the camera's viewing direction vector. 
        // A minus sign before the vector's components indicates that the normal
        // will point in the opposite direction from the viewing vector. 
        // That is, if the camera is "looking" in the direction of the gaze vector, 
        // the normal with a negative sign will "look" in the direction from which the camera's gaze comes.


        XMVECTOR planeXYZ = XMPlaneFromPointNormal(XMVectorSet(10.0f, 0.0f, -10.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
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

void LisaApp::PivotScaleImpl::LButtonDown(
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

    // Create normals for the auxiliary lines.
    // This is necessary for drawing lines from the reverse side (or with the opposite sign).

    XMFLOAT4X4 mX = m_storageScale[pivot_components::cube]->OnRender[0].Instances[0].World;
    XMFLOAT4X4 mY = m_storageScale[pivot_components::cube]->OnRender[0].Instances[1].World;
    XMFLOAT4X4 mZ = m_storageScale[pivot_components::cube]->OnRender[0].Instances[2].World;

    XMVECTOR translate = { attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ };

    m_normalAuxLineX = XMVector3Normalize(XMVectorSubtract({ mX._41, mX._42, mX._43 }, translate));
    m_normalAuxLineY = XMVector3Normalize(XMVectorSubtract({ mY._41, mY._42, mY._43 }, translate));
    m_normalAuxLineZ = XMVector3Normalize(XMVectorSubtract({ mZ._41, mZ._42, mZ._43 }, translate));

    // Update the matrix of auxiliary components before they appear.
    {
        XMMATRIX matrix = XMMatrixAffineTransformation(
            XMVectorSet(attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ, 0.0f),
            XMVectorZero(),
            attributes.Quaternion,
            translate);

        for (const auto& st : m_storageScaleAux)
        {
            auto& [objectID, primitives] = st;

            for (auto&& [iter, instance] : primitives.get()->OnRender[0].Instances | std::views::enumerate)
                XMStoreFloat4x4(&instance.World, XMMatrixMultiply(m_tools.GetMatrixFromStorage(objectID + (static_cast<std::uint32_t>(iter) + 1)), matrix));
        }
    }

    // Determine which axis was selected.

    m_objectID = 0;

    const auto& it = refWrapOnRender.begin();

    auto& [objectID, instancesID] = it->first;
    auto& onRender = it->second.front().get();

    // Since instances are used, the object ID needs to be updated.

    if (onRender.ID == pivot_components::cube)
    {
        if (instancesID == 0)
            m_objectID = pivot_components::cube_x;
        if (instancesID == 1)
            m_objectID = pivot_components::cube_y;
        if (instancesID == 2)
            m_objectID = pivot_components::cube_z;
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
    case pivot_components::cube_x:
    case pivot_components::line_x:
        m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 0, false);
        break;
    case pivot_components::cube_y:
    case pivot_components::line_y:
        m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 1, false);
        break;
    case pivot_components::cube_z:
    case pivot_components::line_z:
        m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 2, false);
        break;
    case pivot_components::plane_x:
        m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 0, false);
        m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 1, false);
        break;
    case pivot_components::plane_y:
        m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 0, false);
        m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 2, false);
        break;
    case pivot_components::plane_z:
        m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 1, false);
        m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 2, false);
        break;
    case pivot_components::center_cube:
        m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 0, false);
        m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 1, false);
        m_tools.HideOrVisibleComponents(m_storageScaleAux, pivot_components::aux_line, 2, false);
        break;
    default:
        break;
    }

    m_pivotStart = PointOnThePlane(attributes, getViev, proj4x4f, sx, sy, screenWidth, screenHeight);
}

bool LisaApp::PivotScaleImpl::LButtonMove(
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

    
    XMVECTOR v1{};
    XMVECTOR v2 = XMVectorSubtract(m_pivotEnd, m_pivotStart);
    XMVECTOR l = XMVector3Length(v2);
    float s = XMVectorGetX(l);

    if (m_objectID == pivot_components::plane_x)
        v1 = XMVectorSubtract(XMVectorSet(m_pivotStart.m128_f32[0] + 1.0f, m_pivotStart.m128_f32[1], m_pivotStart.m128_f32[2], 0.0f), m_pivotStart);
    
    else if (m_objectID == pivot_components::plane_y)
        v1 = XMVectorSubtract(XMVectorSet(m_pivotStart.m128_f32[0] + 1.0f, m_pivotStart.m128_f32[1], m_pivotStart.m128_f32[2], 0.0f), m_pivotStart);

    else if (m_objectID == pivot_components::plane_z)
        v1 = XMVectorSubtract(XMVectorSet(m_pivotStart.m128_f32[0], m_pivotStart.m128_f32[1], m_pivotStart.m128_f32[2] - 1.0f, 0.0f), m_pivotStart);
    
    else if (m_objectID == pivot_components::center_cube)
        v1 = XMVectorSubtract(XMVectorSet(m_pivotStart.m128_f32[0], m_pivotStart.m128_f32[1], m_pivotStart.m128_f32[2] + 1.0f, 0.0f), m_pivotStart);

    XMVECTOR nv1 = XMVector3Normalize(v1);
    XMVECTOR nv2 = XMVector3Normalize(v2);

    // By using the dot product we have more freedom to scale the object.
    XMVECTOR dot = XMVector3Dot(nv1, nv2);

    // Update the pivot attributes.

    if (m_objectID == pivot_components::cube_x || m_objectID == pivot_components::line_x)
    {
        float x{ XMVectorGetX(v2) };
        attributes.OutputScaleX = x;
        m_offsetX += x;
    }
    else if (m_objectID == pivot_components::cube_y || m_objectID == pivot_components::line_y)
    {
        float y{ XMVectorGetY(v2) };
        attributes.OutputScaleY = y;
        m_offsetY += y;
    }
    else if (m_objectID == pivot_components::cube_z || m_objectID == pivot_components::line_z)
    {
        float z{ XMVectorGetZ(v2) };
        attributes.OutputScaleZ = -z;
        m_offsetZ += z;
    }
    else if (m_objectID == pivot_components::plane_x)
    {
        if (XMVectorGetX(dot) > 0)
        {
            attributes.OutputScaleX = s;
            attributes.OutputScaleY = s;
            m_offsetX += s;
            m_offsetY += s;
        }
        else if (XMVectorGetX(dot) < 0)
        {
            attributes.OutputScaleX = -s;
            attributes.OutputScaleY = -s;
            m_offsetX -= s;
            m_offsetY -= s;
        }
    }
    else if (m_objectID == pivot_components::plane_y)
    {
        if (XMVectorGetX(dot) > 0)
        {
            attributes.OutputScaleX = s;
            attributes.OutputScaleZ = s;
            m_offsetX += s;
            m_offsetZ -= s;
        }
        else if (XMVectorGetX(dot) < 0)
        {
            attributes.OutputScaleX = -s;
            attributes.OutputScaleZ = -s;
            m_offsetX -= s;
            m_offsetZ += s;
        }
    }
    else if (m_objectID == pivot_components::plane_z)
    {
        if (XMVectorGetX(dot) > 0)
        {
            attributes.OutputScaleY = s;
            attributes.OutputScaleZ = s;
            m_offsetY += s;
            m_offsetZ -= s;
        }
        else if (XMVectorGetX(dot) < 0)
        {
            attributes.OutputScaleY = -s;
            attributes.OutputScaleZ = -s;
            m_offsetY -= s;
            m_offsetZ += s;
        }
    }
    else if (m_objectID == pivot_components::center_cube)
    {
        if (XMVectorGetX(dot) > 0)
        {
            attributes.OutputScaleX = s;
            attributes.OutputScaleY = s;
            attributes.OutputScaleZ = s;
            m_offsetX += s;
            m_offsetY += s;
            m_offsetZ -= s;
        }
        else if (XMVectorGetX(dot) < 0)
        {
            attributes.OutputScaleX = -s;
            attributes.OutputScaleY = -s;
            attributes.OutputScaleZ = -s;
            m_offsetX -= s;
            m_offsetY -= s;
            m_offsetZ += s;
        }
    }

    // Move cubes and scale lines when we scale an object.

    if (m_objectID == pivot_components::cube_x || m_objectID == pivot_components::line_x)
    {
        MovementCubeMatrix(attributes, 0, -1, -1);
    }
    else if (m_objectID == pivot_components::cube_y || m_objectID == pivot_components::line_y)
    {
        MovementCubeMatrix(attributes, -1, 1, -1);
    }
    else if (m_objectID == pivot_components::cube_z || m_objectID == pivot_components::line_z)
    {
        MovementCubeMatrix(attributes, -1, -1, 2);
    }
    else if (m_objectID == pivot_components::plane_x)
    {
        MovementCubeMatrix(attributes, 0, -1, -1);
        MovementCubeMatrix(attributes, -1, 1, -1);
    }
    else if (m_objectID == pivot_components::plane_y)
    {
        MovementCubeMatrix(attributes, 0, -1, -1);
        MovementCubeMatrix(attributes, -1, -1, 2);
    }
    else if (m_objectID == pivot_components::plane_z)
    {
        MovementCubeMatrix(attributes, -1, 1, -1);
        MovementCubeMatrix(attributes, -1, -1, 2);
    }
    else if (m_objectID == pivot_components::center_cube)
    {
        MovementCubeMatrix(attributes, 0, -1, -1);
        MovementCubeMatrix(attributes, -1, 1, -1);
        MovementCubeMatrix(attributes, -1, -1, 2);
    }

    m_pivotStart = m_pivotEnd;

    return true;
}

void LisaApp::PivotScaleImpl::Draw(
    _In_ ID3D12GraphicsCommandList* commandList,
    _In_ ID3D12PipelineState* mesh,
    _In_ ID3D12PipelineState* line
)
{
    using namespace LisaApp::Global;

    m_storageScale[pivot_components::cube]->Draw(commandList, { mesh });
    m_storageScale[pivot_components::line]->Draw(commandList, { line });
    m_storageScale[pivot_components::plane]->Draw(commandList, { mesh });

    // Draw a cube here so that it covers the lines.

    m_storageScale[pivot_components::center_cube]->Draw(commandList, { mesh });
    m_storageScaleAux[pivot_components::aux_line]->Draw(commandList, { line });
}

void LisaApp::PivotScaleImpl::UpdateCB(
    const DirectX::XMMATRIX& getViev,
    const DirectX::BoundingFrustum& camFrustum,
    bool frustumCullingEnabled
)
{
    for (const auto& po : m_storageScale)
        po.second->UpdateBuffer(getViev, camFrustum, frustumCullingEnabled);

    for (const auto& po : m_storageScaleAux)
        po.second->UpdateBuffer(getViev, camFrustum, frustumCullingEnabled);
}