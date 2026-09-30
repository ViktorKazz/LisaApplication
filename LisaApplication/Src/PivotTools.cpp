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

#include "PivotTools.h"

// The function creates planes through which the ray will pass.
// The intersection of the ray and the plane will allow the pivot to move.
// Also, such an intersection will be useful when scaling objects.
void LisaApp::PivotTools::PlanesParallelToTheAxes()
{
    using namespace DirectX;

    // Plane parallel to the X-Y axes.
    m_planeXY = XMPlaneFromPointNormal(XMVectorSet(10.0f, 10.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
    // Plane parallel to the Z-Y axes.
    m_planeYZ = XMPlaneFromPointNormal(XMVectorSet(0.0f, 10.0f, -10.0f, 0.0f), XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
    // Plane parallel to the X-Z axes.
    m_planeXZ = XMPlaneFromPointNormal(XMVectorSet(10.0f, 0.0f, -10.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
};

// The idea is that the object matrix is ​​immutable and in order to change the object itself, 
// you need to multiply the original matrix by a new matrix.
void LisaApp::PivotTools::SetAttributes(std::uint32_t key, DirectX::XMFLOAT4X4& world,
    float tx, float ty, float tz, float rx, float ry, float rz
)
{
    using namespace DirectX;

    XMMATRIX rotation =
        XMMatrixRotationX(XMConvertToRadians(rx)) *
        XMMatrixRotationY(XMConvertToRadians(ry)) *
        XMMatrixRotationZ(XMConvertToRadians(rz));

    XMStoreFloat4x4(&world, XMMatrixMultiply(rotation, XMMatrixTranslation(tx, ty, tz)));

    // It will be easier to work with the finished matrix in the future.
    XMMATRIX m = XMLoadFloat4x4(&world);
    m_matrixStore.insert({ key, m });
};

void LisaApp::PivotTools::SetAttributes(std::uint32_t key, DirectX::XMMATRIX& world,
    float tx, float ty, float tz, float rx, float ry, float rz
)
{
    using namespace DirectX;

    XMMATRIX rotation =
        XMMatrixRotationX(XMConvertToRadians(rx)) *
        XMMatrixRotationY(XMConvertToRadians(ry)) *
        XMMatrixRotationZ(XMConvertToRadians(rz));

    world = XMMatrixMultiply(rotation, XMMatrixTranslation(tx, ty, tz));

    // It will be easier to work with the finished matrix in the future.
    m_matrixStore.insert({ key, world });
};

// The function allows you to save information about the vertices and indices 
// of an object so that you can hide it by changing them.
void LisaApp::PivotTools::VertexIndexStore(SceneObjects& storage, std::uint32_t key)
{
    m_vertexIndexStore.insert({ key,
        {
            storage[key]->OnRender[0].IndexCount,
            storage[key]->OnRender[0].BaseVertexLocation,
            storage[key]->OnRender[0].StartIndexLocation
        }
        }
    );
};

// The function shows or hides an object by changing the data about its vertices and indices.
void LisaApp::PivotTools::HideOrVisibleComponents(SceneObjects& storage, std::uint32_t key, bool isHide)
{
    if (!isHide)
    {
        storage[key]->OnRender[0].IndexCount = 0;
        storage[key]->OnRender[0].BaseVertexLocation = 0;
        storage[key]->OnRender[0].StartIndexLocation = 0;
    }
    else
    {
        const auto& [indexCount, baseVertexLocation, startIndexLocation] = m_vertexIndexStore.at(key);

        storage[key]->OnRender[0].IndexCount = indexCount;
        storage[key]->OnRender[0].BaseVertexLocation = baseVertexLocation;
        storage[key]->OnRender[0].StartIndexLocation = startIndexLocation;
    }
};

// Overloading this function has the same meaning, but uses a shader for hiding and showing.
// It is also possible to change the visibility of individual instances.
void LisaApp::PivotTools::HideOrVisibleComponents(SceneObjects& storage, std::uint32_t key, std::uint32_t instances, bool isHide)
{
    if (!isHide)
    {
        storage[key]->OnRender[0].Instances[instances].Hide = isHide;
    }
    else
    {
        storage[key]->OnRender[0].Instances[instances].Hide = isHide;
    }
};

// The function shows or hides an object by changing the data about its vertices and indices.
void LisaApp::PivotTools::Hide(SceneObjects& storage, float angleA, float angleB, std::uint32_t keyA, std::uint32_t keyB, bool& lock)
{
    float threshold = 15.0f;

    if (fabs(angleA) < threshold && fabs(angleB) < threshold && !lock)
    {
        lock = true;

        HideOrVisibleComponents(storage, keyA, false);
        HideOrVisibleComponents(storage, keyB, false);
    }
    // To show the component again, one suitable angle is enough.
    else if ((fabs(angleA) > threshold || fabs(angleB) > threshold) && lock)
    {
        lock = false;

        HideOrVisibleComponents(storage, keyA, true);
        HideOrVisibleComponents(storage, keyB, true);
    }
};

// The function shows or hides an object by changing the data about its vertices and indices.
void LisaApp::PivotTools::Hide(SceneObjects& storage, float angle, std::uint32_t key, bool& lock)
{
    // Set a value below which the pivot component will be hidden.
    float threshold = 15.0f;

    if (fabs(angle) < threshold && !lock)
    {
        lock = true;
        HideOrVisibleComponents(storage, key, false);
    }
    // To show the component again, one suitable angle is enough.
    else if (fabs(angle) > threshold && lock)
    {
        lock = false;
        HideOrVisibleComponents(storage, key, true);
    }
};

// Overloading this function has the same meaning, but uses a shader for hiding and showing.
// It is also possible to change the visibility of individual instances.
void LisaApp::PivotTools::Hide(
    SceneObjects& storage, 
    float angleA,
    float angleB, 
    std::uint32_t keyA, 
    std::uint32_t keyB, 
    std::uint32_t instancesA,
    std::uint32_t instancesB,
    bool& lock
)
{
    float threshold = 15.0f;

    if (fabs(angleA) < threshold && fabs(angleB) < threshold && !lock)
    {
        lock = true;

        storage[keyA]->OnRender[0].Instances[instancesA].Hide = 1;
        storage[keyB]->OnRender[0].Instances[instancesB].Hide = 1;
    }
    // To show the component again, one suitable angle is enough.
    else if ((fabs(angleA) > threshold || fabs(angleB) > threshold) && lock)
    {
        lock = false;

        storage[keyA]->OnRender[0].Instances[instancesA].Hide = 0;
        storage[keyB]->OnRender[0].Instances[instancesB].Hide = 0;
    }
};

// Overloading this function has the same meaning, but uses a shader for hiding and showing.
// It is also possible to change the visibility of individual instances.
void LisaApp::PivotTools::Hide(SceneObjects& storage, float angle, std::uint32_t key, std::uint32_t instances, bool& lock)
{
    // Set a value below which the pivot component will be hidden.
    float threshold = 15.0f;

    if (fabs(angle) < threshold && !lock)
    {
        lock = true;
        storage[key]->OnRender[0].Instances[instances].Hide = 1;
    }
    // To show the component again, one suitable angle is enough.
    else if (fabs(angle) > threshold && lock)
    {
        lock = false;
        storage[key]->OnRender[0].Instances[instances].Hide = 0;
    }
};