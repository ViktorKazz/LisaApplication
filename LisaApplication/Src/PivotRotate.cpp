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

#include "PivotRotate.h"
#include <HelperMath.h>
#include "AppColors.h"

void LisaApp::PivotRotateImpl::Create(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    XMFLOAT4 outerCircleColor{ AppColors::Color::PivotOuterCircle };
    XMFLOAT4 innerCircleColor{ AppColors::Color::PivotInnerCircle };
    XMFLOAT4 sphereColor{ AppColors::Color::PivotSphere };
    XMFLOAT4 xAxisColor{ AppColors::Color::PivotXAxis };
    XMFLOAT4 yAxisColor{ AppColors::Color::PivotYAxis };
    XMFLOAT4 zAxisColor{ AppColors::Color::PivotZAxis };
    XMFLOAT4 triangleColor{ AppColors::Color::PivotAuxiliaryTriangle };

    std::unique_ptr<PolygonPrimitives> pOuterCircle = PolygonPrimitives::CreateLineCircle(device, commandList, gDefaultPivotRadius + 1.0f, 1, outerCircleColor);
    pOuterCircle->OnRender[0].ID = pivot_components::outer_circle;
    std::unique_ptr<PolygonPrimitives> pInnerCircle = PolygonPrimitives::CreateLineCircle(device, commandList, gDefaultPivotRadius, 1, innerCircleColor);
    pInnerCircle->OnRender[0].ID = pivot_components::inner_circle;

    // To create similar primitives, we use object instances.

    std::unique_ptr<PolygonPrimitives> pCircle = PolygonPrimitives::CreateLineCircle(device, commandList, gDefaultPivotRadius, 1, xAxisColor);
    pCircle->OnRender[0].ID = pivot_components::circle;
    pCircle->AddInstances(1);
    pCircle->AddInstances(2);
    pCircle->OnRender[0].Instances[1].Color = yAxisColor;
    pCircle->OnRender[0].Instances[2].Color = zAxisColor;

    std::unique_ptr<PolygonPrimitives> pSphere = PolygonPrimitives::CreateGeoSphereEasy(device, commandList, gDefaultPivotRadius - 0.05f, 3, sphereColor);
    pSphere->OnRender[0].ID = pivot_components::sphere;


    const XMFLOAT3 linePointA{ 0.0f, 0.0f, 0.0f };
    const XMFLOAT3 linePointB{ 0.0f, 0.0f, gDefaultPivotRadius };

    std::unique_ptr<PolygonPrimitives> pLine = PolygonPrimitives::CreateLine(device, commandList, linePointA, linePointB);
    pLine->OnRender[0].ID = pivot_components::aux_line;
    pLine->AddInstances(1);

    std::unique_ptr<PolygonPrimitives> pTriangle = PolygonPrimitives::CreateTriangle(
        device, commandList, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, triangleColor);
    pTriangle->OnRender[0].ID = pivot_components::aux_triangle;


    // Save the object matrices and arrange the objects.

    m_tools.SetAttributes(pivot_components::outer_circle, pOuterCircle->OnRender[0].Instances[0].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::inner_circle, pInnerCircle->OnRender[0].Instances[0].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::circle_x, pCircle->OnRender[0].Instances[0].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -90.0f);
    m_tools.SetAttributes(pivot_components::circle_y, pCircle->OnRender[0].Instances[1].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::circle_z, pCircle->OnRender[0].Instances[2].World, 0.0f, 0.0f, 0.0f, 90.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::sphere, pSphere->OnRender[0].Instances[0].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    m_tools.SetAttributes(pivot_components::aux_sphere_x, m_sphereX, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -90.0f);
    m_tools.SetAttributes(pivot_components::aux_sphere_y, m_sphereY, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::aux_sphere_z, m_sphereZ, 0.0f, 0.0f, 0.0f, 90.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::aux_sphere_xyz, m_sphereXYZ, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::aux_line_start, pLine->OnRender[0].Instances[0].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::aux_line_end, pLine->OnRender[0].Instances[1].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_tools.SetAttributes(pivot_components::aux_triangle, pTriangle->OnRender[0].Instances[0].World, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);


    m_storageRotate.try_emplace(pivot_components::outer_circle, std::move(pOuterCircle));
    m_storageRotate.try_emplace(pivot_components::circle, std::move(pCircle));
    m_storageRotate.try_emplace(pivot_components::sphere, std::move(pSphere));

    m_innerCircle.try_emplace(pivot_components::inner_circle, std::move(pInnerCircle));

    m_storageRotateAux.try_emplace(pivot_components::aux_line, std::move(pLine));
    m_storageRotateAux.try_emplace(pivot_components::aux_triangle, std::move(pTriangle));

    // Save information about vertices and indices for hiding and showing the object.

    m_tools.VertexIndexStore(m_storageRotateAux, pivot_components::aux_line);
    m_tools.VertexIndexStore(m_storageRotateAux, pivot_components::aux_triangle);

    // Initially, the auxiliary components are hidden.
    // They are displayed when a pivot component is selected and hidden when deselected.

    m_tools.HideOrVisibleComponents(m_storageRotateAux, pivot_components::aux_line, false);
    m_tools.HideOrVisibleComponents(m_storageRotateAux, pivot_components::aux_triangle, false);
}

void LisaApp::PivotRotateImpl::Scale(const IPivot::Attributes& attributes, const DirectX::XMVECTOR& cameraPosition)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    m_cameraPosition = cameraPosition;

    XMVECTOR vScaling{ attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ, 0.0f };
    XMVECTOR translation{ attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ, 0.0f };


    XMMATRIX matrix = XMMatrixAffineTransformation(vScaling, m_rotationOrigin, attributes.Quaternion, translation);

    XMStoreFloat4x4(&m_storageRotate[pivot_components::circle]->OnRender[0].Instances[0].World, 
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::circle_x), matrix));
    XMStoreFloat4x4(&m_storageRotate[pivot_components::circle]->OnRender[0].Instances[1].World, 
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::circle_y), matrix));
    XMStoreFloat4x4(&m_storageRotate[pivot_components::circle]->OnRender[0].Instances[2].World, 
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::circle_z), matrix));

    // There is no need to rotate the sphere.

    matrix = XMMatrixAffineTransformation(vScaling, m_rotationOrigin, XMQuaternionIdentity(), translation);

    XMStoreFloat4x4(&m_storageRotate[pivot_components::sphere]->OnRender[0].Instances[0].World, 
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::sphere), matrix));

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

    matrix = XMMatrixMultiply(XMMatrixMultiply(scaling, rotation), m_billboardingMatrix);

    XMStoreFloat4x4(&m_storageRotate[pivot_components::outer_circle]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::outer_circle), matrix));
    XMStoreFloat4x4(&m_innerCircle[pivot_components::inner_circle]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::inner_circle), matrix));

    // The auxiliary matrices also need to be changed.

    matrix = XMMatrixAffineTransformation(vScaling, m_rotationOrigin, attributes.Quaternion, translation);

    m_sphereX = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_x), matrix);
    m_sphereY = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_y), matrix);
    m_sphereZ = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_z), matrix);


    matrix = XMMatrixAffineTransformation(vScaling, m_rotationOrigin, XMQuaternionIdentity(), translation);

    m_sphereXYZ = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_xyz), matrix);
}

// The function allows the rotation pivot to be rotated by the same angles as the selected object.
void LisaApp::PivotRotateImpl::PlaceThePivotInTheDesiredPosition(const IPivot::Attributes& attributes)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    XMMATRIX matrix = XMMatrixAffineTransformation(
        XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
        XMVectorZero(),
        attributes.Quaternion,
        XMVectorSet(
            attributes.TranslateX,
            attributes.TranslateY,
            attributes.TranslateZ,
            0.0f
        )
    );

    XMStoreFloat4x4(&m_storageRotate[pivot_components::outer_circle]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::outer_circle), matrix));
    XMStoreFloat4x4(&m_innerCircle[pivot_components::inner_circle]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::inner_circle), matrix));

    XMStoreFloat4x4(&m_storageRotate[pivot_components::circle]->OnRender[0].Instances[0].World, 
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::circle_x), matrix));
    XMStoreFloat4x4(&m_storageRotate[pivot_components::circle]->OnRender[0].Instances[1].World, 
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::circle_y), matrix));
    XMStoreFloat4x4(&m_storageRotate[pivot_components::circle]->OnRender[0].Instances[2].World, 
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::circle_z), matrix));

    XMStoreFloat4x4(&m_storageRotate[pivot_components::sphere]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::sphere), matrix));
    

    m_sphereX = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_x), matrix);
    m_sphereY = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_y), matrix);
    m_sphereZ = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_z), matrix);

    matrix = XMMatrixAffineTransformation(
        XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
        XMVectorZero(),
        XMQuaternionIdentity(),
        XMVectorSet(
            attributes.TranslateX,
            attributes.TranslateY,
            attributes.TranslateZ,
            0.0f
        )
    );

    m_sphereXYZ = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_xyz), matrix);
}

void LisaApp::PivotRotateImpl::LButtonUp(const IPivot::Attributes& attributes)
{
    using namespace LisaApp::Global;
    using namespace DirectX;

    // Hide the auxiliary components and move them to the pivot location.

    m_tools.HideOrVisibleComponents(m_storageRotateAux, pivot_components::aux_line, false);
    m_tools.HideOrVisibleComponents(m_storageRotateAux, pivot_components::aux_triangle, false);

    // When the selected axis rotates, the auxiliary matrix of this axis does not rotate.
    // Here this matrix takes the same rotation as its axis.

    XMVECTOR translation{ attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ, 0.0f };
    XMVECTOR scaling{ attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ, 0.0f };

    XMMATRIX matrix = XMMatrixAffineTransformation(scaling, m_rotationOrigin, attributes.Quaternion, translation);

    if (m_objectID == pivot_components::circle_x)
    {
        m_sphereX = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_x), matrix);
    }
    else if (m_objectID == pivot_components::circle_y)
    {
        m_sphereY = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_y), matrix);
    }
    else if (m_objectID == pivot_components::circle_z)
    {
        m_sphereZ = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_z), matrix);
    }
}

// The function calculates the intersection of a ray and a plane, as well as a sphere.
DirectX::XMVECTOR LisaApp::PivotRotateImpl::PointOnThePlaneAndSphere(
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

    // Will use BoundingSphere as the auxiliary sphere that the ray will intersect.
    DirectX::BoundingSphere bb;
    bb.Center = { 0.0f, 0.0f, 0.0f };
    bb.Radius = gDefaultPivotRadius;

    float dist{};

    DirectX::XMVECTOR intersect{};

    auto const CalculatingIntersect = [&](XMMATRIX iMatrix)
        {
            CalculatingRays(getViev, proj4x4f, iMatrix, rayOrigin, rayDir, sx, sy, screenWidth, screenHeight);

            if (bb.Intersects(rayOrigin, rayDir, dist))
            {
                intersect = XMVectorMultiplyAdd(rayDir, XMVectorSet(dist, dist, dist, 0.0f), rayOrigin);

                // Write down the current distance to the intersection point.
                // When the ray goes beyond the sphere, we can use this to find the coordinates,
                // which means the rotation of the pivot will not be interrupted.
                m_bufferDist = dist;
            }
            else
            {
                intersect = XMVectorMultiplyAdd(rayDir, XMVectorSet(m_bufferDist, m_bufferDist, m_bufferDist, 0.0f), rayOrigin);
            }
        };

    switch (m_objectID)
    {
    case pivot_components::outer_circle:
    {
        XMMATRIX m = XMLoadFloat4x4(&m_storageRotate[pivot_components::outer_circle]->OnRender[0].Instances[0].World);
        CalculatingRays(getViev, proj4x4f, m, rayOrigin, rayDir, sx, sy, screenWidth, screenHeight);

        XMVECTOR planeXYZ = XMPlaneFromPointNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
        intersect = IntersectRayPlane(rayOrigin, rayDir, planeXYZ);
    }
    break;
    case pivot_components::circle_x:
    {
        CalculatingIntersect(m_sphereX);
    }
    break;
    case pivot_components::circle_y:
    {
        CalculatingIntersect(m_sphereY);
    }
    break;
    case pivot_components::circle_z:
    {
        CalculatingIntersect(m_sphereZ);
    }
    break;
    case pivot_components::sphere:
    {
        CalculatingIntersect(m_sphereXYZ);
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

void LisaApp::PivotRotateImpl::LButtonDown(
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

    // Determine which axis was selected.

    XMVECTOR axis{};

    XMMATRIX updateAuxMatrix{};
    XMMATRIX updateTriangleMatrix{};

    m_quaternionAux = XMQuaternionIdentity();
    m_quaternionAuxTriangle = XMQuaternionIdentity();

    m_objectID = 0;

    const auto& it = refWrapOnRender.begin();

    auto& [objectID, instancesID] = it->first;
    auto& onRender = it->second.front().get();

    // Since instances are used, the object ID needs to be updated.

    if (onRender.ID == pivot_components::circle)
    {
        if (instancesID == 0)
            m_objectID = pivot_components::circle_x;
        if (instancesID == 1)
            m_objectID = pivot_components::circle_y;
        if (instancesID == 2)
            m_objectID = pivot_components::circle_z;
    }
    else
    {
        m_objectID = onRender.ID;
    }

    XMMATRIX m = XMMatrixRotationQuaternion(XMQuaternionNormalize(m_quaternionAux));

    // The auxiliary matrices must correspond to the matrices of the pivot components at the time of their creation.

    switch (m_objectID)
    {
    case pivot_components::circle_x:
    {
        m_pivotStart = PointOnThePlaneAndSphere(getViev, proj4x4f, sx, sy, screenWidth, screenHeight);
        m_intersectionWithAux = XMVectorSet(m_pivotStart.m128_f32[0], 0.0f, m_pivotStart.m128_f32[2], 0.0f);

        axis = XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), m);
        updateAuxMatrix = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_line_start), XMMatrixRotationZ(-1.57f));
        updateTriangleMatrix = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_triangle), XMMatrixRotationZ(-1.57f));
    }
    break;
    case pivot_components::circle_y:
    {
        m_pivotStart = PointOnThePlaneAndSphere(getViev, proj4x4f, sx, sy, screenWidth, screenHeight);
        m_intersectionWithAux = { m_pivotStart.m128_f32[0], 0.0f, m_pivotStart.m128_f32[2] };

        axis = XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), m);
        updateAuxMatrix = m_tools.GetMatrixFromStorage(pivot_components::aux_line_start);
        updateTriangleMatrix = m_tools.GetMatrixFromStorage(pivot_components::aux_triangle);
    }
    break;
    case pivot_components::circle_z:
    {
        m_pivotStart = PointOnThePlaneAndSphere(getViev, proj4x4f, sx, sy, screenWidth, screenHeight);
        m_intersectionWithAux = { m_pivotStart.m128_f32[0], 0.0f, m_pivotStart.m128_f32[2] };

        axis = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), m);
        updateAuxMatrix = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_line_start), XMMatrixRotationX(1.57f));
        updateTriangleMatrix = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_triangle), XMMatrixRotationX(1.57f));
    }
    break;
    case pivot_components::outer_circle:
    {
        m_pivotStart = PointOnThePlaneAndSphere(getViev, proj4x4f, sx, sy, screenWidth, screenHeight);
        m_intersectionWithAux = { m_pivotStart.m128_f32[0], 0.0f, m_pivotStart.m128_f32[2] };

        axis = XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), m);

        float s = 1.127f;
        XMMATRIX scaling = XMMatrixScaling(s, s, s);

        updateAuxMatrix = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_line_start), scaling);
        updateTriangleMatrix = m_tools.GetMatrixFromStorage(pivot_components::aux_triangle);
    }
    break;
    case pivot_components::sphere:
    {
        m_pivotStart = PointOnThePlaneAndSphere(getViev, proj4x4f, sx, sy, screenWidth, screenHeight);
        m_intersectionWithAux = m_pivotStart;

        updateAuxMatrix = m_tools.GetMatrixFromStorage(pivot_components::aux_line_start);
        updateTriangleMatrix = m_tools.GetMatrixFromStorage(pivot_components::aux_triangle);
    }
    break;
    default:
        break;
    }

    // Find the angle by which we rotate the first auxiliary segment.

    XMVECTOR p0 = XMVectorZero();
    XMVECTOR v0 = XMVectorSubtract(m_intersectionWithAux, p0);
    XMVECTOR v1 = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    float radians = FindingTheAngleBetweenVectors(v0, v1);


    XMVECTOR cross = XMVector3Cross(XMVector3Normalize(v0), XMVector3Normalize(v1));

    if (m_objectID != pivot_components::sphere)
    {
        if (XMVectorGetY(cross) > 0)
        {
            // Here, and in other similar places, 
            // multiplying by -1 allows you to get the positive or negative angle 
            // needed to get the components to rotate correctly.
            radians = radians * -1;
        }
    }

    if (m_objectID == pivot_components::sphere)
    {
        axis = XMVectorNegate(cross);
    }


    m_quaternionAux = XMQuaternionMultiply(m_quaternionAux, XMQuaternionRotationNormal(XMVector3Normalize(axis), radians));

    XMMATRIX matrixLine{};
    XMMATRIX matrixTriangle{};

    XMVECTOR vScaling{ attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ, 0.0f };
    XMVECTOR translation{ attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ, 0.0f };

    if (m_objectID == pivot_components::circle_x || m_objectID == pivot_components::circle_y || m_objectID == pivot_components::circle_z)
    {
        m_quaternionAux = XMQuaternionMultiply(m_quaternionAux, attributes.Quaternion);

        matrixLine = XMMatrixAffineTransformation(vScaling, m_rotationOrigin, m_quaternionAux, translation);
        matrixTriangle = XMMatrixAffineTransformation(vScaling, m_rotationOrigin, attributes.Quaternion, translation);
    }
    else if (m_objectID == pivot_components::outer_circle)
    {
        // The billboard matrix already contains a translation.
        matrixLine = XMMatrixAffineTransformation(vScaling, m_rotationOrigin, m_quaternionAux, { 0.0f, 0.0f, 0.0f, 0.0f });

        // It is necessary that the object (circle) looks at the camera not as a line, but as a circle.
        XMMATRIX rotation = XMMatrixRotationX(XMConvertToRadians(90.0f));
        matrixLine = XMMatrixMultiply(XMMatrixMultiply(matrixLine, rotation), m_billboardingMatrix);


        XMMATRIX scaling = XMMatrixScaling(attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ);
        matrixTriangle = XMMatrixMultiply(XMMatrixMultiply(scaling, rotation), m_billboardingMatrix);
    }
    else if (m_objectID == pivot_components::sphere)
    {
        matrixLine = XMMatrixAffineTransformation(vScaling, m_rotationOrigin, m_quaternionAux, translation);
        matrixTriangle = XMMatrixAffineTransformation(vScaling, m_rotationOrigin, XMQuaternionIdentity(), translation);
    }

    XMStoreFloat4x4(&m_storageRotateAux[pivot_components::aux_line]->OnRender[0].Instances[0].World, XMMatrixMultiply(updateAuxMatrix, matrixLine));
    XMStoreFloat4x4(&m_storageRotateAux[pivot_components::aux_line]->OnRender[0].Instances[1].World, XMMatrixMultiply(updateAuxMatrix, matrixLine));
    XMStoreFloat4x4(&m_storageRotateAux[pivot_components::aux_triangle]->OnRender[0].Instances[0].World, XMMatrixMultiply(updateTriangleMatrix, matrixTriangle));

    m_tools.HideOrVisibleComponents(m_storageRotateAux, pivot_components::aux_line, true);
    // If you do not return the previous data to the IndexCount variable, the componentCount variable will be equal to zero.
    m_tools.HideOrVisibleComponents(m_storageRotateAux, pivot_components::aux_triangle, true);

    {
        auto& geo = m_storageRotateAux[pivot_components::aux_triangle]->OnRender[0].Geo;
        auto vertices = reinterpret_cast<VertexStructs::VertexPositionColor*>(geo->VertexBufferCPU->GetBufferPointer());

        XMStoreFloat3(&vertices[0].position, m_intersectionWithAux);
        // Central vertex.
        XMStoreFloat3(&vertices[1].position, XMVectorZero());
        // The vertex that moves.
        XMStoreFloat3(&vertices[2].position, XMVectorZero());

        const std::uint32_t componentCount = m_storageRotateAux[pivot_components::aux_triangle]->OnRender[0].IndexCount;
        const UINT vbByteSize = componentCount * sizeof(VertexStructs::VertexPositionColor);

        m_storageRotateAux[pivot_components::aux_triangle]->MappedData(vertices, vbByteSize);

        if (m_storageRotateAux[pivot_components::aux_triangle]->OnRender)
        {
            m_storageRotateAux[pivot_components::aux_triangle]->OnRender[0].Geo->VertexBufferGPU = 
                m_storageRotateAux[pivot_components::aux_triangle]->GetUploadBuffer();
        }
    }
}

bool LisaApp::PivotRotateImpl::LButtonMove(
    const DirectX::XMMATRIX& getViev,
    const DirectX::XMFLOAT4X4& proj4x4f,
    std::int32_t sx,
    std::int32_t sy,
    std::int32_t screenWidth,
    std::int32_t screenHeight,
    IPivot::Attributes& attributes
)
{
    using namespace LisaApp::HelperMath;
    using namespace LisaApp::Global;
    using namespace DirectX;


    m_pivotEnd = PointOnThePlaneAndSphere(getViev, proj4x4f, sx, sy, screenWidth, screenHeight);

    // The object itself moves in world space. 
    // But the intersection coordinates are always local. 
    // Therefore, the origin will always be 0.0f, 0.0f, 0.0f,
    // since the object always remains in the same place relative to its local matrix.

    XMVECTOR p0 = XMVectorZero();
    XMVECTOR v0 = XMVectorSubtract(m_pivotStart, p0);
    XMVECTOR v1 = XMVectorSubtract(m_pivotEnd, p0);

    float radians = FindingTheAngleBetweenVectors(v0, v1);

    // Rotation with a given step.

    if (m_isStep)
    {
        if (radians >= m_radiansStep)
        {
            radians = m_radiansStep;
            m_pivotStart = m_pivotEnd;
        }
        else
        {
            return false;
        }
    }
    
    XMVECTOR nv0 = XMVector3Normalize(v0);
    XMVECTOR nv1 = XMVector3Normalize(v1);

    if (m_objectID != pivot_components::sphere)
    {
        // All objects initially lie in the X-Z plane. 
        // And this is immutable.
        // Therefore, using the Y axis is unnecessary.
        // This will also improve the accuracy of further calculations.
        nv0 = XMVectorSet(nv0.m128_f32[0], 0.0f, nv0.m128_f32[2], 0.0f);
        nv1 = XMVectorSet(nv1.m128_f32[0], 0.0f, nv1.m128_f32[2], 0.0f);
    }

    XMVECTOR cross = XMVector3Cross(nv0, nv1);

    if (m_objectID != pivot_components::sphere)
    {
        if (XMVectorGetY(cross) < 0)
        {
            radians = radians * -1;
        }
    }


    XMVECTOR axis{};
    XMMATRIX updateAuxMatrix{};

    XMVECTOR translation{ attributes.TranslateX, attributes.TranslateY, attributes.TranslateZ, 0.0f };

    XMMATRIX m = XMMatrixRotationQuaternion(XMQuaternionNormalize(attributes.Quaternion));

    // The auxiliary matrices must correspond to the matrices of the pivot components at the time of their creation.

    if (m_objectID == pivot_components::circle_x)
    {
        axis = XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), m);
        updateAuxMatrix = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_line_start), XMMatrixRotationZ(-1.57f));
    }
    else if (m_objectID == pivot_components::circle_y)
    {
        axis = XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), m);
        updateAuxMatrix = m_tools.GetMatrixFromStorage(pivot_components::aux_line_start);
    }
    else if (m_objectID == pivot_components::circle_z)
    {
        axis = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), m);
        updateAuxMatrix = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_line_start), XMMatrixRotationX(1.57f));
    }
    else if (m_objectID == pivot_components::outer_circle)
    {
        // Vector from the camera to the center of the object.
        axis = XMVectorSubtract(translation, m_cameraPosition);
        attributes.AxisNorm = XMVector3Normalize(axis);

        float s = 1.127f;
        XMMATRIX scaling = XMMatrixScaling(s, s, s);

        updateAuxMatrix = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_line_start), scaling);
    }
    else if (m_objectID == pivot_components::sphere)
    {
        axis = cross;
        attributes.AxisNorm = XMVector3Normalize(axis);

        updateAuxMatrix = m_tools.GetMatrixFromStorage(pivot_components::aux_line_start);
    }

    attributes.Radian = radians;
    attributes.Quaternion = XMQuaternionMultiply(attributes.Quaternion, XMQuaternionRotationNormal(XMVector3Normalize(axis), radians));

    // Obtain angles from the rotation matrix for each axis.
    {
        //XMVECTOR axisX = XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), m);
        //XMVECTOR axisY = XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), m);
        //XMVECTOR axisZ = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), m);

        //float rotateX = std::atan2f(axisZ.m128_f32[1], axisZ.m128_f32[2]);
        //float rotateY = std::atan2f(-axisZ.m128_f32[0], std::sqrtf(axisZ.m128_f32[1] * axisZ.m128_f32[1] + axisZ.m128_f32[2] * axisZ.m128_f32[2]));
        //float rotateZ = std::atan2f(axisX.m128_f32[1], axisY.m128_f32[1]);


        //// Here atan2 is the same arc tangent function, with quadrant checking, you typically find in C or Matlab.
        //// Note: Care must be taken if the angle around the y-axis is exactly +/-90°. 
        //// In that case all elements in the first column and last row, except the one in the lower corner, 
        //// which is either 1 or -1, will be 0 (cos(1)=0). 
        //// One solution would be to fix the rotation around the x-axis at 180° 
        //// and compute the angle around the z-axis from: atan2(r_12, -r_22).
        //// See also https://www.geometrictools.com/Documentation/EulerAngles.pdf, 
        //// which includes implementations for six different orders of Euler angles.

        //wchar_t msg[128]{};

        //swprintf_s(msg, L"rt x: % f\n", XMConvertToDegrees(rotateX));
        //OutputDebugString(msg);
        //swprintf_s(msg, L"rt y: % f\n", XMConvertToDegrees(rotateY));
        //OutputDebugString(msg);
        //swprintf_s(msg, L"rt z: % f\n", XMConvertToDegrees(rotateZ));
        //OutputDebugString(msg);
    }


    XMVECTOR scaling{ attributes.ScaleX, attributes.ScaleY, attributes.ScaleZ, 0.0f };

    XMMATRIX matrix = XMMatrixAffineTransformation(scaling, m_rotationOrigin, attributes.Quaternion, translation);

    if (m_objectID == pivot_components::circle_x)
    {
        m_sphereY = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_y), matrix);
        m_sphereZ = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_z), matrix);
    }
    else if (m_objectID == pivot_components::circle_y)
    {
        m_sphereX = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_x), matrix);
        m_sphereZ = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_z), matrix);
    }
    else if (m_objectID == pivot_components::circle_z)
    {
        m_sphereX = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_x), matrix);
        m_sphereY = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_y), matrix);
    }
    else if (m_objectID == pivot_components::outer_circle)
    {
        m_sphereX = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_x), matrix);
        m_sphereY = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_y), matrix);
        m_sphereZ = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_z), matrix);
    }
    else if (m_objectID == pivot_components::sphere)
    {
        m_sphereX = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_x), matrix);
        m_sphereY = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_y), matrix);
        m_sphereZ = XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::aux_sphere_z), matrix);
    }

    XMStoreFloat4x4(&m_storageRotate[pivot_components::circle]->OnRender[0].Instances[0].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::circle_x), matrix));
    XMStoreFloat4x4(&m_storageRotate[pivot_components::circle]->OnRender[0].Instances[1].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::circle_y), matrix));
    XMStoreFloat4x4(&m_storageRotate[pivot_components::circle]->OnRender[0].Instances[2].World,
        XMMatrixMultiply(m_tools.GetMatrixFromStorage(pivot_components::circle_z), matrix));


    if (m_objectID == pivot_components::outer_circle)
    {
        axis = cross;

        if (XMVectorGetY(cross) < 0)
        {
            radians = radians * -1;
        }
    }

    m_quaternionAux = XMQuaternionMultiply(m_quaternionAux, XMQuaternionRotationNormal(XMVector3Normalize(axis), radians));

    if (m_objectID == pivot_components::outer_circle)
    {
        // The billboard matrix already contains a translation.
        matrix = XMMatrixAffineTransformation(scaling, m_rotationOrigin, m_quaternionAux, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));

        // It is necessary that the object (circle) looks at the camera not as a line, but as a circle.
        XMMATRIX rotation = XMMatrixRotationX(XMConvertToRadians(90.0f));

        matrix = XMMatrixMultiply(XMMatrixMultiply(matrix, rotation), m_billboardingMatrix);
    }
    else
    {
        matrix = XMMatrixAffineTransformation(scaling, m_rotationOrigin, m_quaternionAux, translation);
    }

    XMStoreFloat4x4(&m_storageRotateAux[pivot_components::aux_line]->OnRender[0].Instances[1].World, XMMatrixMultiply(updateAuxMatrix, matrix));


    if (m_objectID == pivot_components::outer_circle)
    {
        if (XMVectorGetY(cross) < 0)
        {
            radians = radians * -1;
        }
    }

    if (m_objectID != pivot_components::sphere)
    {
        axis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    }

    {
        m_quaternionAuxTriangle = XMQuaternionMultiply(m_quaternionAuxTriangle, XMQuaternionRotationNormal(XMVector3Normalize(axis), radians));

        auto& geo = m_storageRotateAux[pivot_components::aux_triangle]->OnRender[0].Geo;
        auto vertices = reinterpret_cast<VertexStructs::VertexPositionColor*>(geo->VertexBufferCPU->GetBufferPointer());

        XMVECTOR p = XMVector3Rotate(m_intersectionWithAux, m_quaternionAuxTriangle);

        // The vertex that moves.
        XMStoreFloat3(&vertices[2].position, p);

        const std::uint32_t componentCount = m_storageRotateAux[pivot_components::aux_triangle]->OnRender[0].IndexCount;
        const UINT vbByteSize = componentCount * sizeof(VertexStructs::VertexPositionColor);

        m_storageRotateAux[pivot_components::aux_triangle]->MappedData(vertices, vbByteSize);

        if (m_storageRotateAux[pivot_components::aux_triangle]->OnRender)
        {
            m_storageRotateAux[pivot_components::aux_triangle]->OnRender[0].Geo->VertexBufferGPU =
                m_storageRotateAux[pivot_components::aux_triangle]->GetUploadBuffer();
        }
    }

    m_pivotStart = m_pivotEnd;

    return true;
}

void LisaApp::PivotRotateImpl::Draw(
    _In_ ID3D12GraphicsCommandList* commandList,
    _In_ ID3D12PipelineState* line,
    _In_ ID3D12PipelineState* mesh,
    _In_ ID3D12PipelineState* lineCircle,
    _In_ ID3D12PipelineState* colorRotationAngle
)
{
    using namespace LisaApp::Global;

    m_storageRotate[pivot_components::sphere]->Draw(commandList, { mesh });

    m_storageRotate[pivot_components::outer_circle]->Draw(commandList, { line });
    m_innerCircle[pivot_components::inner_circle]->Draw(commandList, { line });
    m_storageRotate[pivot_components::circle]->Draw(commandList, { lineCircle });

    m_storageRotateAux[pivot_components::aux_line]->Draw(commandList, { line });
    m_storageRotateAux[pivot_components::aux_triangle]->Draw(commandList, { colorRotationAngle });
}

void LisaApp::PivotRotateImpl::UpdateCB(
    const DirectX::XMMATRIX& getViev,
    const DirectX::BoundingFrustum& camFrustum,
    bool frustumCullingEnabled
)
{
    for (const auto& po : m_storageRotate)
        po.second->UpdateBuffer(getViev, camFrustum, frustumCullingEnabled);

    for (const auto& po : m_innerCircle)
        po.second->UpdateBuffer(getViev, camFrustum, frustumCullingEnabled);

    for (const auto& po : m_storageRotateAux)
        po.second->UpdateBuffer(getViev, camFrustum, frustumCullingEnabled);
}