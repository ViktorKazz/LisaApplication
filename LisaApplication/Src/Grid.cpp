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

#include "Grid.h"
#include "HelperUtilities.h"

void LisaApp::Grid::ArrangementOfLinesAndSymbols(float lengthAndWidth, float gridLinesEvery, size_t subdivisions)
{
    using namespace DirectX;

    const float gridSize{ 1.0f * lengthAndWidth };
    const XMMATRIX scaling = XMMatrixScaling(gridSize, gridSize, gridSize);

    int ZDebug{};

    auto const Lines = [&](size_t numInstances, float lineSpacing, Axis xOrZ)
        {
            XMVECTOR p = xOrZ ? XMVECTOR{ 0.0f, 0.0f, lineSpacing } : XMVECTOR{ lineSpacing, 0.0f, 0.0f };
            XMMATRIX translation = XMMatrixTranslationFromVector(p);

            if (xOrZ)
            {
                wchar_t msg[128]{};
                swprintf_s(msg, L"Grid debug Z axis: % d\n", ZDebug);
                OutputDebugString(msg);
                ZDebug++;
            }

            XMFLOAT4X4& target = xOrZ ?
                m_centerLineZ->OnRender[0].Instances[numInstances].World :
                m_centerLineX->OnRender[0].Instances[numInstances].World;
            XMStoreFloat4x4(&target, XMMatrixMultiply(scaling, translation));
        };


    m_positiveGridLinesEveryIndex.clear();
    m_negativegridLinesEveryIndex.clear();

    m_positiveSubdivisionsIndex.clear();
    m_negativeSubdivisionsIndex.clear();


    // Create a grid lines every

    const size_t numGridLines{ static_cast<size_t>(std::floorf(lengthAndWidth / gridLinesEvery)) };

    for (size_t i = 1; i <= numGridLines; i++)
        m_positiveGridLinesEveryIndex.emplace_back(i);

    for (size_t i = 1; i <= numGridLines; i++)
        m_negativegridLinesEveryIndex.emplace_back(numGridLines + i);

    for (size_t i = 1; i <= numGridLines * 2; i++)
    {
        m_centerLineX->AddInstances(i);
        m_centerLineZ->AddInstances(i);
        m_centerLineX->OnRender[0].Instances[i].Color = m_gridLinesEveryColor;
        m_centerLineZ->OnRender[0].Instances[i].Color = m_gridLinesEveryColor;
    }

    for (bool isPositive : {true, false})
    {
        const size_t& indices = isPositive ? 0 : numGridLines;
        const float sign = isPositive ? 1.0f : -1.0f;

        for (size_t i = 1; i <= numGridLines; ++i)
        {
            const float offset = sign * gridLinesEvery * i;

            Lines(indices + i, offset, Axis::X);
            Lines(indices + i, offset, Axis::Z);
        }
    }


    // Create a subdivisions.

    const float remainder{ lengthAndWidth - (gridLinesEvery * numGridLines) };
    const float subSegment{ gridLinesEvery / subdivisions };
    const size_t numRemainderLines{ static_cast<size_t>(std::floorf(remainder / subSegment)) };

    // Get the fractional part.
    const float rem = std::fmodf(remainder, subSegment);

    size_t subdivisionsSize = (((subdivisions - 1) * numGridLines) + numRemainderLines) * 2;
    const size_t allLines{ m_centerLineX->OnRender[0].Instances.size() };

    if (rem != 0) subdivisionsSize += 2;

    for (size_t i = 0; i < subdivisionsSize; i++)
    {
        m_centerLineX->AddInstances(allLines + i);
        m_centerLineZ->AddInstances(allLines + i);
        m_centerLineX->OnRender[0].Instances[allLines + i].Color = m_subdivisionsColor;
        m_centerLineZ->OnRender[0].Instances[allLines + i].Color = m_subdivisionsColor;
    }

    // For convenience, we will store the indices in containers.

    for (size_t i = 0; i < subdivisionsSize / 2; i++)
        m_positiveSubdivisionsIndex.emplace_back(allLines + i);

    for (size_t i = subdivisionsSize / 2; i < subdivisionsSize; i++)
        m_negativeSubdivisionsIndex.emplace_back(allLines + i);

    // Arrange the instances in the desired order.

    for (bool isPositive : {true, false})
    {
        const auto& indices = isPositive ? m_positiveSubdivisionsIndex : m_negativeSubdivisionsIndex;
        const float sign = isPositive ? 1.0f : -1.0f;

        size_t add = 0;
        for (size_t i = 0; i < indices.size(); ++i)
        {
            if (i == (subdivisions - 1) * (add + 1)) add += 1;

            float offset = sign * (subSegment * (i + 1) + subSegment * add);

            // Draw if there is a fractional part.
            if (i == indices.size() - 1 && rem) offset -= (subSegment - rem) * sign;

            Lines(indices[i], offset, Axis::X);
            Lines(indices[i], offset, Axis::Z);
        }
    }

    // Create a symbols.

    //There are four sides in total: -x, +x, -z, +z.
    const std::uint32_t numberSides{ 4 };
    const std::uint32_t iGridLines{ static_cast<std::uint32_t>(std::floorf(lengthAndWidth / gridLinesEvery)) };
    const std::uint32_t iTempGridLines = iGridLines;

    // Add space in the array for the zero symbol at the origin.
    const std::uint32_t zero{ 1 };
    const std::uint32_t newSize{ iTempGridLines * numberSides + zero };

    m_gridCharWorld.clear();
    m_gridCharScreen.clear();

    // The coordinates of the intersection of the lines are zero.
    m_gridCharWorld.push_back({ 0.0f, 0.0f });
    // +X
    for (size_t i = 1; i <= iTempGridLines; i++)
    {
        m_gridCharWorld.push_back({ gridLinesEvery * i, 0.0f });
    }
    // -X
    for (size_t i = 1; i <= iTempGridLines; i++)
    {
        m_gridCharWorld.push_back({ -gridLinesEvery * i, 0.0f });
    }
    // +Z
    for (size_t i = 1; i <= iTempGridLines; i++)
    {
        m_gridCharWorld.push_back({ 0.0f, gridLinesEvery * i });
    }
    // -Z
    for (size_t i = 1; i <= iTempGridLines; i++)
    {
        m_gridCharWorld.push_back({ 0.0f, -gridLinesEvery * i });
    }

    m_gridCharScreen.resize(newSize);
}

void LisaApp::Grid::Create(
    _In_ ID3D12Device3* device, 
    _In_ ID3D12GraphicsCommandList* commandList,
    _In_ ID3D12RootSignature* rootSignature,
    std::uint32_t sampleCount,
    std::uint32_t sampleQuality,
    float lengthAndWidth,
    float gridLinesEvery,
    size_t subdivisions
)
{
    CreatePso(device, rootSignature, sampleCount, sampleQuality);


    m_centerLineX = PolygonPrimitives::CreateLine(device, commandList, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f });
    m_centerLineZ = PolygonPrimitives::CreateLine(device, commandList, { 1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f });

    const float gridSize{ 1.0f * lengthAndWidth };
    const DirectX::XMMATRIX scaling = DirectX::XMMatrixScaling(gridSize, gridSize, gridSize);

    XMStoreFloat4x4(&m_centerLineX->OnRender[0].Instances[0].World, scaling);
    XMStoreFloat4x4(&m_centerLineZ->OnRender[0].Instances[0].World, scaling);

    m_centerLineX->OnRender[0].Instances[0].Color = m_centerColor;
    m_centerLineZ->OnRender[0].Instances[0].Color = m_centerColor;

    ArrangementOfLinesAndSymbols(lengthAndWidth, gridLinesEvery, subdivisions);
}

void LisaApp::Grid::CreatePso(
    _In_ ID3D12Device3* device,
    _In_ ID3D12RootSignature* rootSignature,
    std::uint32_t sampleCount,
    std::uint32_t sampleQuality
)
{
    // Unzipping shaders.
    //m_newShadersPath = HelperUtilities::Unzipping("Resources\\Shaders\\rs.ls");
    m_shader.SetPathToShaders(LisaApp::HelperPath("..\\LisaApplication\\Resources\\Shaders")  /*m_newShadersPath*/);

    m_pso = { rootSignature, VertexStructs::VertexPositionColor::InputLayout };

    LisaApp::PSOConfig::EffectInitial einit;

    einit.pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    einit.pso.SampleDesc = { sampleCount, sampleQuality };
    
    const D3D_SHADER_MACRO coloringPerInstances[] = { "COLORING_PER_INSTANCES", "1", NULL, NULL };
    m_pso.CreatePipelineState(einit.pso, device, m_shader.SimpleColoring(coloringPerInstances, nullptr), Line);
}

void LisaApp::Grid::SettingUpGridSymbols(
    ID3D12Device3* device,
    ID3D12CommandQueue* commandQueue,
    const DXGI_FORMAT& backBufferFormat,
    const DXGI_FORMAT& depthBufferFormat
)
{
    m_resourceDescriptors = std::make_unique<DirectX::DescriptorHeap>(device, Descriptors::Count);

    DirectX::ResourceUploadBatch resourceUpload(device);
    resourceUpload.Begin();

    m_font = std::make_unique<DirectX::SpriteFont>(device, resourceUpload,
        LisaApp::HelperPath(L"Resources\\Font\\verdana_10.spritefont").c_str(),
        m_resourceDescriptors->GetCpuHandle(Descriptors::UIFont),
        m_resourceDescriptors->GetGpuHandle(Descriptors::UIFont));


    DirectX::RenderTargetState rtState(backBufferFormat, depthBufferFormat);

    DirectX::SpriteBatchPipelineStateDescription pd(rtState);
    m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(device, resourceUpload, pd);

    auto uploadResourcesFinished = resourceUpload.End(commandQueue);
    uploadResourcesFinished.wait();
}

void LisaApp::Grid::SettingUpGraphicsMemory(ID3D12Device3* device)
{
    m_graphicsMemory = std::make_unique<DirectX::GraphicsMemory>(device);
};

void LisaApp::Grid::CommitGraphicsMemory(ID3D12CommandQueue* commandQueue)
{
    m_graphicsMemory->Commit(commandQueue);
}

void LisaApp::Grid::UpdateSpriteBatch(const D3D12_VIEWPORT& viewport)
{
    m_spriteBatch->SetViewport(viewport);
}

void LisaApp::Grid::UpdateGrid(
    const Camera& camera, 
    const D3D12_VIEWPORT& viewport
)
{    
    if (!camera.mViewDirty)
    {
        for (size_t i = 0; i < m_gridCharWorld.size(); i++)
        {
            m_gridCharScreen[i] = ConvertWorldSpaceToViewSpace(
                camera.GetView(),
                camera.GetProj4x4f(),
                viewport.Width,
                viewport.Height,
                m_gridCharWorld[i].first,
                0.0f,
                m_gridCharWorld[i].second
            );
        }
    }
}

void LisaApp::Grid::UpdateCB(
    const DirectX::XMMATRIX& getViev,
    const DirectX::BoundingFrustum& camFrustum,
    bool frustumCullingEnabled)
{
    m_centerLineX->UpdateBuffer(getViev, camFrustum, frustumCullingEnabled);
    m_centerLineZ->UpdateBuffer(getViev, camFrustum, frustumCullingEnabled);
};

// Drawing a coordinate grid in the viewport
void LisaApp::Grid::Draw(ID3D12GraphicsCommandList* commandList)
{
    m_centerLineX->Draw(commandList, { m_pso.GetPipeline(Line) });
    m_centerLineZ->Draw(commandList, { m_pso.GetPipeline(Line) });
}

// Drawing symbols on a coordinate grid.
void LisaApp::Grid::DrawSymbolsOnAGrid(
    _In_ ID3D12GraphicsCommandList* commandList,
    const D3D12_VIEWPORT& viewport,
    const D3D12_RECT& scissorRect,
    const CD3DX12_CPU_DESCRIPTOR_HANDLE& rtvDescriptor,
    ID3D12DescriptorHeap* descriptorHeaps[]
)
{
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    // Unbind depth/stencil for UI.
    //auto rtvDescriptor = object.m_deviceResources->GetRenderTargetView();
    //auto dsvDescriptor = object.m_deviceResources->GetDepthStencilView();
    // Specify the buffers we are going to render to.
    commandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, nullptr);

    descriptorHeaps[0] = { m_resourceDescriptors->Heap() };
    commandList->SetDescriptorHeaps(1, descriptorHeaps);

    m_spriteBatch->Begin(commandList);

    // We draw a zero symbol in the center of coordinates.
    wchar_t z[4]{ L"0" };
    DirectX::SimpleMath::Vector2 origin = m_font->MeasureString(z);
    origin.x = origin.x / 2;

    m_font->DrawString(
        m_spriteBatch.get(),
        z,
        DirectX::XMFLOAT2(
            m_gridCharScreen[0].first,
            m_gridCharScreen[0].second
        ),
        DirectX::XMLoadFloat4(&m_symbolsColor),
        0.0f,
        origin
    );

    //
    const size_t zeroSymbols{ 1 };
    const size_t numAxis{ 2 };
    const size_t numSymbolsPerAxis{ (m_gridCharScreen.size() - zeroSymbols) / numAxis };

    for (size_t i = 1; i <= m_gridCharScreen.size() - zeroSymbols; i++)
    {
        wchar_t wcs1[64]{};
        wchar_t wcs2[64]{};

        // Drawing along the x-axis
        if (i <= numSymbolsPerAxis)
        {
            swprintf_s(wcs1, L"%f\n", m_gridCharWorld[i].first);
        }
        // Drawing along the z-axis
        else
        {
            swprintf_s(wcs1, L"%f\n", m_gridCharWorld[i].second);
        }

        // Trim three characters from the end.
        // But since we have a zero terminator in the line, 
        // we subtract the value not 3 but 4.
        wcsncat_s(wcs2, wcs1, wcslen(wcs1) - 4);
        // Add the X symbol.
        if (i <= numSymbolsPerAxis)
        {
            wcsncat_s(wcs2, L"x", wcslen(L"x"));
        }
        // Add the Z symbol.
        else
        {
            wcsncat_s(wcs2, L"z", wcslen(L"z"));
        }
        origin = m_font->MeasureString(wcs2);
        origin.x = origin.x / 2;

        m_font->DrawString(
            m_spriteBatch.get(),
            wcs2,
            DirectX::XMFLOAT2(
                m_gridCharScreen[i].first,
                m_gridCharScreen[i].second
            ),
            DirectX::XMLoadFloat4(&m_symbolsColor),
            0.0f,
            origin
        );
    }

    m_spriteBatch->End();
}

std::pair<float, float> LisaApp::Grid::ConvertWorldSpaceToViewSpace(
    const DirectX::XMMATRIX& getViev,
    const DirectX::XMFLOAT4X4& proj4x4f,
    float ScreenViewportX,
    float ScreenViewportY,
    float x,
    float y,
    float z
)
{
    std::pair<float, float> output;

    float ScreenX{};
    float ScreenY{};
    const DirectX::XMMATRIX V = getViev;
    const DirectX::XMFLOAT4X4 P = proj4x4f;
    const DirectX::XMMATRIX Pp = DirectX::XMMatrixSet(
        P._11, P._12, P._13, P._14, P._21, P._22, P._23, P._24, P._31, P._32, P._33, P._34, P._41, P._42, P._43, P._44);
    const DirectX::XMMATRIX VP = DirectX::XMMatrixMultiply(V, Pp);

    const DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1);

    DirectX::XMVECTOR result = DirectX::XMVector3TransformCoord(pos, VP);

    if (result.m128_f32[2] < 1) {
        ScreenX = (result.m128_f32[0] + 1.0f) * ScreenViewportX / 2.0f;
        ScreenY = (1.0f - result.m128_f32[1]) * ScreenViewportY / 2.0f;
    }
    output = { ScreenX, ScreenY };

    return output;
}

void LisaApp::Grid::ColorChange(
    const DirectX::XMFLOAT4& centerColor, 
    const DirectX::XMFLOAT4& gridLinesEveryColor,
    const DirectX::XMFLOAT4& subdivisionsColor
)
{
    m_centerColor = centerColor;
    m_gridLinesEveryColor = gridLinesEveryColor;
    m_subdivisionsColor = subdivisionsColor;
    m_symbolsColor = gridLinesEveryColor;

    // Center color change.

    m_centerLineX->OnRender[0].Instances[0].Color = m_centerColor;
    m_centerLineZ->OnRender[0].Instances[0].Color = m_centerColor;

    // GridLinesEvery color change.

    for (const auto& i : m_positiveGridLinesEveryIndex)
        m_centerLineX->OnRender[0].Instances[i].Color = m_gridLinesEveryColor;
    for (const auto& i : m_negativegridLinesEveryIndex)
        m_centerLineX->OnRender[0].Instances[i].Color = m_gridLinesEveryColor;

    for (const auto& i : m_positiveGridLinesEveryIndex)
        m_centerLineZ->OnRender[0].Instances[i].Color = m_gridLinesEveryColor;
    for (const auto& i : m_negativegridLinesEveryIndex)
        m_centerLineZ->OnRender[0].Instances[i].Color = m_gridLinesEveryColor;

    // Subdivisions color change.

    for (const auto& i : m_positiveSubdivisionsIndex)
        m_centerLineX->OnRender[0].Instances[i].Color = m_subdivisionsColor;
    for (const auto& i : m_negativeSubdivisionsIndex)
        m_centerLineX->OnRender[0].Instances[i].Color = m_subdivisionsColor;

    for (const auto& i : m_positiveSubdivisionsIndex)
        m_centerLineZ->OnRender[0].Instances[i].Color = m_subdivisionsColor;
    for (const auto& i : m_negativeSubdivisionsIndex)
        m_centerLineZ->OnRender[0].Instances[i].Color = m_subdivisionsColor;
}

void LisaApp::Grid::Resize(float lengthAndWidth, float gridLinesEvery, size_t subdivisions)
{
    using namespace DirectX;

    size_t sz = m_centerLineX->OnRender[0].Instances.size();

    m_centerLineX->OnRender[0].Instances.erase(
        m_centerLineX->OnRender[0].Instances.begin() + 1, 
        m_centerLineX->OnRender[0].Instances.begin() + sz
    );
    m_centerLineZ->OnRender[0].Instances.erase(
        m_centerLineZ->OnRender[0].Instances.begin() + 1, 
        m_centerLineZ->OnRender[0].Instances.begin() + sz
    );

    m_centerLineX->OnRender[0].Attributes.clear();
    m_centerLineZ->OnRender[0].Attributes.clear();

    const float gridSize{ 1.0f * lengthAndWidth };
    const XMMATRIX scaling = XMMatrixScaling(gridSize, gridSize, gridSize);

    XMStoreFloat4x4(&m_centerLineX->OnRender[0].Instances[0].World, scaling);
    XMStoreFloat4x4(&m_centerLineZ->OnRender[0].Instances[0].World, scaling);

    ArrangementOfLinesAndSymbols(lengthAndWidth, gridLinesEvery, subdivisions);
}
