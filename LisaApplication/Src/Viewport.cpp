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

#include "Viewport.h"
#include "Globals.h"

std::unique_ptr<Viewport> Viewport::Perspective(
    HINSTANCE hInstance,
    const RECT& rect,
    const std::variant<std::wstring, std::pair<std::wstring, std::wstring>>& varName,
    UI::ui_type type,
    UI::ui_modes style,
    UI::ui_transform transform,
    HWND parentHwnd
)
{
    using namespace UI;

    HelperWTools tools;

    DWORD dwExStyle{ NULL };
    DWORD dwStyle{ WS_VISIBLE | WS_CHILD };

    INT l{ rect.left }, t{ rect.top }, r{ rect.right }, b{ rect.bottom };

    std::pair<std::wstring, std::wstring> wstr{};

    if (std::holds_alternative<std::wstring>(varName))
        wstr.first = std::get<std::wstring>(varName);

    else if (std::holds_alternative<std::pair<std::wstring, std::wstring>>(varName))
        wstr = std::get<std::pair<std::wstring, std::wstring>>(varName);

    std::wstring wClass{ tools.CreateClass(type, wstr.first) };

    UINT wndClassStyle{};
    if (style & ui_modes::dblclks)
        wndClassStyle = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    else
        wndClassStyle = CS_HREDRAW | CS_VREDRAW;

    std::unique_ptr<Viewport> element(new Viewport(l, t, r, b, hInstance, wClass.c_str(), wstr.second.c_str(), wndClassStyle, dwExStyle, dwStyle));

    element->m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    element->m_depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // Initialising DirectX 12 resources.
    element->m_deviceResources = std::make_unique<DX::DeviceResources>(
        element->m_backBufferFormat,
        element->m_depthBufferFormat, /* If we were only doing MSAA rendering, we could skip the non-MSAA depth/stencil buffer with DXGI_FORMAT_UNKNOWN */
        2);

    element->m_deviceResources->RegisterDeviceNotify(this);

    element->m_style = style;
    element->m_transform = transform;
    element->m_parentHwnd = parentHwnd;
    element->m_targetSampleCount = 8;


    HWND hwnd = element->Initialize(*element, element->ViewportWindowProc, parentHwnd);

    return element;
}

Viewport::~Viewport()
{
    if (m_deviceResources)
    {
        m_deviceResources->WaitForGpu();
    }
    
    {
        // Delete the created directory with resources.
        std::filesystem::file_status s = std::filesystem::file_status{};
        if (std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(m_newShadersPath))
            std::filesystem::remove_all(m_newShadersPath);
        else
            std::cout << "Directory " << m_newShadersPath << " not found.\n";
    }
}

bool Viewport::InitializationResources(HWND hwnd, int width, int height)
{

    m_deviceResources->SetWindow(hwnd, width, height);

    m_deviceResources->CreateDeviceResources();

    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
  
    CreateWindowSizeDependentResources();

    // Estimate the scene bounding sphere manually since we know how the scene was constructed.
    // The grid is the "widest object" with a width of 20 and depth of 30.0f, and centered at
    // the world space origin.  In general, you need to loop over every world space vertex
    // position and compute the bounding sphere.
    mSceneBounds.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    mSceneBounds.Radius = sqrtf(10.0f * 10.0f + 15.0f * 15.0f);

    DX::ThrowIfFailed(m_deviceResources->GetCommandAllocator()->Reset());
    DX::ThrowIfFailed(m_deviceResources->GetCommandList()->Reset(m_deviceResources->GetCommandAllocator(), nullptr));

    // Camera

    DirectX::XMVECTOR camPos{ 28.0f, 21.0f, -28.0f };

    DirectX::XMVECTOR l = DirectX::XMVector3Length(camPos);

    m_camera.SetCameraTranslateZ(-DirectX::XMVectorGetX(l));

    m_camera.SetCameraRotateX(27.938f);
    m_camera.SetCameraRotateY(-45.0f);

    m_camera.SetPosition(camPos.m128_f32[0], camPos.m128_f32[1], camPos.m128_f32[2]);

    m_camera.RotateX(DirectX::XMConvertToRadians(m_camera.GetCameraRotateX()));
    m_camera.RotateY(DirectX::XMConvertToRadians(m_camera.GetCameraRotateY()));


    auto device = m_deviceResources->GetD3DDevice();
    auto commandList = m_deviceResources->GetCommandList();

    mShadowMap = std::make_unique<ShadowMap>(device, 2048, 2048);
    mSsao = std::make_unique<Ssao>(device, commandList, width, height);

    m_PassCB = std::make_unique<UploadBuffer<Constants::Scene>>(device, 2, true);
    m_SsaoCB = std::make_unique<UploadBuffer<Constants::Ssao>>(device, 1, true);
    m_materials->SetMaterialBuffer(device, 1, false);


    m_rootSignature.RootSignaturePrepareSSAO(device);
    m_rootSignature.RootSignatureSSAO(device);


    BuildDescriptorHeaps();
    BuildMaterials();

    // Unzipping shaders.
    //m_newShadersPath = HelperUtilities::Unzipping("Resources\\Shaders\\rs.ls");

    m_shader.SetPathToShaders(LisaApp::HelperPath("..\\LisaApplication\\Resources\\Shaders")  /*m_newShadersPath*/);

    m_creatingPrimitives->CreatePso(device, m_rootSignature.GetPrepareSsaoRootSignature(), 
        Ssao::NormalMapFormat, m_depthBufferFormat, m_sampleCount, m_sampleQuality);

    BuildPSOs();

    mSsao->SetPSOs(m_pipelineStateSSAO.GetPipeline(SSAO), m_pipelineStateSSAO.GetPipeline(SSAOBlur));

    // Let's create a background with a sky texture.
    m_background->Create(device, commandList, m_rootSignature.GetPrepareSsaoRootSignature(), 
        m_sampleCount, m_sampleQuality, m_materials->GetMaterialIndex(L"sky"));

    // Create a grid in the viewport.
    m_grid->Create(device, commandList, m_rootSignature.GetPrepareSsaoRootSignature(), m_sampleCount, m_sampleQuality);

    // Create a pivot.

    // To ensure that the pivot scale is updated in the Update function once during window loading.
    m_pivot->SetPivotDirty(true);
    m_pivot->Create(device, commandList, m_rootSignature.GetPrepareSsaoRootSignature(), m_sampleCount, m_sampleQuality);

    // Close the command list and execute it to begin the initial GPU setup.
    DX::ThrowIfFailed(m_deviceResources->GetCommandList()->Close());
    ID3D12CommandList* ppCommandLists[] = { m_deviceResources->GetCommandList() };
    m_deviceResources->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    return true;
}

void Viewport::OnDeviceLost()
{
    m_deviceResources.reset();

    m_msaaRenderTarget.Reset();
    m_msaaDepthStencil.Reset();

    m_msaaRTVDescriptorHeap.Reset();
    m_msaaDSVDescriptorHeap.Reset();

    //m_srvDescriptorHeap.Reset();

    //m_font.reset();
    //m_resourceDescriptors.reset();
    //m_spriteBatch.reset();
}

void Viewport::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}

void Viewport::CreateDeviceDependentResources(this Viewport& object)
{
    auto device = object.m_deviceResources->GetD3DDevice();

    // For text
    object.m_grid->SettingUpGraphicsMemory(device);
    
    //end

    // Create descriptor heaps for MSAA render target views and depth stencil views.
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
    rtvDescriptorHeapDesc.NumDescriptors = 1;
    rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    DX::ThrowIfFailed(device->CreateDescriptorHeap(&rtvDescriptorHeapDesc,
        IID_PPV_ARGS(object.m_msaaRTVDescriptorHeap.ReleaseAndGetAddressOf())));

    D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
    dsvDescriptorHeapDesc.NumDescriptors = 1;
    dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    DX::ThrowIfFailed(device->CreateDescriptorHeap(&dsvDescriptorHeapDesc,
        IID_PPV_ARGS(object.m_msaaDSVDescriptorHeap.ReleaseAndGetAddressOf())));


    // Add +1 for screen normal map, +2 for ambient maps.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.NumDescriptors = 2 + 3;// SwapChainBufferCount + 3;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    DX::ThrowIfFailed(device->CreateDescriptorHeap(
        &rtvHeapDesc, IID_PPV_ARGS(object.m_SSAORTVDescriptorHeap.GetAddressOf())));

    // Add +1 DSV for shadow map.
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
    dsvHeapDesc.NumDescriptors = 2;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    DX::ThrowIfFailed(device->CreateDescriptorHeap(
        &dsvHeapDesc, IID_PPV_ARGS(object.m_SSAODSVDescriptorHeap.GetAddressOf())));

    // Check for MSAA support.
    // Note that 4x MSAA and 8x MSAA is required for Direct3D Feature Level 11.0 or better.

    for (object.m_sampleCount = object.m_targetSampleCount; object.m_sampleCount > 1; object.m_sampleCount--)
    {
        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS levels = { object.m_backBufferFormat, object.m_sampleCount };
        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &levels, sizeof(levels))))
            continue;

        if (levels.NumQualityLevels > 0)
            break;
    }


    // Setup text...
    object.m_grid->SettingUpGridSymbols(
        device,
        object.m_deviceResources->GetCommandQueue(),
        object.m_deviceResources->GetBackBufferFormat(),
        object.m_deviceResources->GetDepthBufferFormat()
    );
    //End setup text
}
 
void Viewport::OnWindowSizeChanged(this Viewport& object, INT width, INT height)
{
    // Because the window has a pop-up style and 
    // as I understood when creating it has a width and height of 0,
    // so that there would be no error because of this I added one.

    // Of course, this is not the best option.When I have the opportunity, 
    // I will find a better solution.

    if (!object.m_deviceResources->WindowSizeChanged(width + 1, height + 1))
        return;

    object.CreateWindowSizeDependentResources();

}

float Viewport::AspectRatio(this Viewport& object)
{
    return static_cast<float>(object.m_width) / object.m_height;
}


void Viewport::CreateWindowSizeDependentResources(this Viewport& object)
{
    auto output = object.m_deviceResources->GetOutputSize();

    // Determine the render target size in pixels.
    UINT backBufferWidth = std::max<UINT>(output.right - output.left, 1);
    UINT backBufferHeight = std::max<UINT>(output.bottom - output.top, 1);

    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

    // Create the MSAA depth/stencil buffer.
    auto depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        object.m_depthBufferFormat,
        backBufferWidth,
        backBufferHeight,
        1, // This depth stencil view has only one texture.
        1, // Use a single mipmap level
        object.m_sampleCount,
        object.m_sampleQuality
    );
    depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = object.m_depthBufferFormat;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    auto device = object.m_deviceResources->GetD3DDevice();
    DX::ThrowIfFailed(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthOptimizedClearValue,
        IID_PPV_ARGS(object.m_msaaDepthStencil.ReleaseAndGetAddressOf())
    ));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = object.m_depthBufferFormat;// DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;

    device->CreateDepthStencilView(object.m_msaaDepthStencil.Get(), &dsvDesc,
        object.m_msaaDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    auto msaaRTDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        object.m_backBufferFormat,
        backBufferWidth,
        backBufferHeight,
        1, // This render target view has only one texture.
        1, // Use a single mipmap level
        object.m_sampleCount,
        object.m_sampleQuality
    );
    msaaRTDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE msaaOptimizedClearValue = {};
    msaaOptimizedClearValue.Format = object.m_backBufferFormat;
    memcpy(msaaOptimizedClearValue.Color, DirectX::Colors::Transparent, sizeof(float) * 4);

    DX::ThrowIfFailed(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &msaaRTDesc,
        D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
        &msaaOptimizedClearValue,
        IID_PPV_ARGS(object.m_msaaRenderTarget.ReleaseAndGetAddressOf())
    ));

    device->CreateRenderTargetView(object.m_msaaRenderTarget.Get(), nullptr,
        object.m_msaaRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());



    object.m_camera.SetLens(0.25f*MathHelper::Pi, object.AspectRatio(), 1.0f, 1000.0f);

    DirectX::BoundingFrustum::CreateFromMatrix(object.mCamFrustum, object.m_camera.GetProj());

    
    // For text
    object.m_grid->UpdateSpriteBatch(object.m_deviceResources->GetScreenViewport());
    object.m_grid->UpdateGrid(object.m_camera, object.m_deviceResources->GetScreenViewport());



    if(object.mSsao != nullptr)
    {
        object.mSsao->OnResize(object.m_width, object.m_height);

        // Resources changed, so need to rebuild descriptors.
        object.mSsao->RebuildDescriptors(object.m_deviceResources->GetDepthStencil());
    }
}

void Viewport::Update()
{
    OnKeyboardInput();

    m_camera.UpdateViewMatrix();
    //
    mLightRotationAngle = 0.25f;

    DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(mLightRotationAngle);
    
    DirectX::XMVECTOR lightDir = XMLoadFloat3(&mBaseLightDirections);
    lightDir = XMVector3TransformNormal(lightDir, R);
    XMStoreFloat3(&mRotatedLightDirections, lightDir);

    //
    m_grid->UpdateGrid(m_camera, m_deviceResources->GetScreenViewport());

    if (m_pivot->GetPivotDirty())
    {
        // The pivot should scale not only when it changes position itself, 
        // but also when the camera changes position and view.
        m_pivot->Scale(m_camera.GetView(), m_camera.GetProj4x4f(), m_camera.GetPosition(), m_width, m_height);
    }
    

    UpdateObjectCBs();
    UpdateMaterialBuffer();
    UpdateShadowTransform();
    UpdateMainPassCB();
    UpdateShadowPassCB();
    UpdateSsaoCB();
}

// Helper method to clear the back buffers.
void Viewport::ClearViews(this Viewport& object, bool clearRTVandDSV)
{
    auto commandList = object.m_deviceResources->GetCommandList();
    
    if (object.m_msaa)
    {
        const CD3DX12_RESOURCE_BARRIER barrier{
            barrier.Transition(
                object.m_msaaRenderTarget.Get(),
                D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
                D3D12_RESOURCE_STATE_RENDER_TARGET
            )
        };
        commandList->ResourceBarrier(1, &barrier);

        // Rather than operate on the swapchain render target, we set up to render the scene to our MSAA resources instead.
        auto rtvDescriptor = object.m_msaaRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        auto dsvDescriptor = object.m_msaaDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

        commandList->OMSetRenderTargets(1, &rtvDescriptor, true, &dsvDescriptor);

        if (clearRTVandDSV)
        {
            commandList->ClearRenderTargetView(rtvDescriptor, DirectX::Colors::Transparent, 0, nullptr);
            commandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        }
    }
    else
    {
        auto rtvDescriptor = object.m_deviceResources->GetRenderTargetView();
        auto dsvDescriptor = object.m_deviceResources->GetDepthStencilView();

        // Specify the buffers we are going to render to.
        commandList->OMSetRenderTargets(1, &rtvDescriptor, true, &dsvDescriptor);

        if (clearRTVandDSV)
        {
            commandList->ClearRenderTargetView(rtvDescriptor, DirectX::Colors::Transparent, 0, nullptr);
            commandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        }
    }

    // Set the viewport and scissor rect.
    auto viewport = object.m_deviceResources->GetScreenViewport();
    auto scissorRect = object.m_deviceResources->GetScissorRect();
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

}

void Viewport::CreateSceneObjects(const LisaApp::PrimitivesData& primitivesData, UINT primitives)
{
    auto commandList = m_deviceResources->GetCommandList();

    m_deviceResources->WaitForGpu();

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    m_deviceResources->Prepare(m_msaa ? D3D12_RESOURCE_STATE_RENDER_TARGET : D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET/*, m_pipelineState.GetPipeline(L"opaque")*/);//2, 3, parametr ?????

    ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    // Clear the views.
    ClearViews(false);

    // This version of the function is convenient for creating only one type of primitives. 
    // In order to create different types of primitives, 
    // it is necessary to rework the method of creating primitives and connecting materials to them.
    m_creatingPrimitives->Create(
        m_deviceResources->GetD3DDevice(),
        commandList,
        primitives,
        primitivesData,
        m_materials->GetMaterialIndex(L"shape")
    );

    MSAA();

    m_grid->DrawSymbolsOnAGrid(
        commandList,
        m_deviceResources->GetScreenViewport(),
        m_deviceResources->GetScissorRect(),
        m_deviceResources->GetRenderTargetView(),
        descriptorHeaps
    );

    m_deviceResources->Present();

    // If the pivot is enabled, move it to the created object.
    if (m_creatingPrimitives->IsSelect() && (m_pivot->GetMode() != LisaApp::Global::PivotMode::Off))
    {
        m_pivot->SetOnOff(true);
        m_pivot->PlaceThePivotInTheDesiredPosition(
            m_creatingPrimitives->GetRefWrapOnRender(), 
            m_creatingPrimitives->GetLastID(),
            m_camera.GetLook()
        );
    }
}

void Viewport::DeleteSceneObjects()
{
    auto commandList = m_deviceResources->GetCommandList();
    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    m_deviceResources->Prepare(m_msaa ? D3D12_RESOURCE_STATE_RENDER_TARGET : D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET/*, m_pipelineState.GetPipeline(L"opaque")*/);//2, 3, parametr ?????

    ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    // Clear the views.
    ClearViews(false);

    MSAA();

    m_grid->DrawSymbolsOnAGrid(
        commandList,
        m_deviceResources->GetScreenViewport(),
        m_deviceResources->GetScissorRect(),
        m_deviceResources->GetRenderTargetView(),
        descriptorHeaps
    );

    m_deviceResources->Present();

    m_creatingPrimitives->Delete();

    // Disable pivot when the object is deleted.

    m_pivot->SetOnOff(false);
}

void Viewport::CommandListClose()
{
    auto commandList = m_deviceResources->GetCommandList();
    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    m_deviceResources->Prepare(m_msaa ? D3D12_RESOURCE_STATE_RENDER_TARGET : D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET/*, m_pipelineState.GetPipeline(L"opaque")*/);//2, 3, parametr ?????

    ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    // Clear the views.
    ClearViews(false);

    MSAA();

    m_grid->DrawSymbolsOnAGrid(
        commandList,
        m_deviceResources->GetScreenViewport(),
        m_deviceResources->GetScissorRect(),
        m_deviceResources->GetRenderTargetView(),
        descriptorHeaps
    );

    m_deviceResources->Present();
}

void Viewport::Draw(this Viewport& object)
{
    auto commandList = object.m_deviceResources->GetCommandList();

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    object.m_deviceResources->Prepare(object.m_msaa ? D3D12_RESOURCE_STATE_RENDER_TARGET : D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET/*, object.m_pipelineState.GetPipeline(L"opaque")*/);//2, 3, parametr ?????

    ID3D12DescriptorHeap* descriptorHeaps[] = { object.mSrvDescriptorHeap.Get() };
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);


    commandList->SetGraphicsRootSignature(object.m_rootSignature.GetSsaoRootSignature());
    object.mSsao->ComputeSsao(commandList, object.m_SsaoCB->Resource()->GetGPUVirtualAddress(), 3);

    commandList->SetGraphicsRootSignature(object.m_rootSignature.GetPrepareSsaoRootSignature());

    // Clear the views.
    object.ClearViews();
    
    // Bind all the materials used in this scene.  For structured buffers, we can bypass the heap and 
    // set as a root descriptor.
    auto matBuffer = object.m_materials->GetMaterialBuffer();
    commandList->SetGraphicsRootShaderResourceView(1, matBuffer->GetGPUVirtualAddress());

    // Bind all the textures used in this scene.  Observe
    // that we only have to specify the first descriptor in the table.  
    // The root signature knows how many descriptors are expected in the table.
    commandList->SetGraphicsRootDescriptorTable(5, object.mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
       
    auto passCB = object.m_PassCB->Resource();
    commandList->SetGraphicsRootConstantBufferView(3, passCB->GetGPUVirtualAddress());
   
    // Bind the sky cube map.  For our demos, we just use one "world" cube map representing the environment
    // from far away, so all objects will use the same cube map and we only need to set it once per-frame.  
    // If we wanted to use "local" cube maps, we would have to change them per-object, or dynamically
    // index into an array of cube maps.
    CD3DX12_GPU_DESCRIPTOR_HANDLE skyTexDescriptor(object.mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    skyTexDescriptor.Offset(object.mSkyTexHeapIndex, object.m_deviceResources->GetCbvSrvUavDescriptorSize());
    commandList->SetGraphicsRootDescriptorTable(4, skyTexDescriptor);

    // Draw primitives

    object.m_creatingPrimitives->Draw(commandList);
    
    // Draw grid

    object.m_grid->Draw(commandList);

    // Draw debug

    commandList->SetPipelineState(object.m_pipelineState.GetPipeline(Debug));

    // Draw background

    // Can be disabled for the release version.
    object.m_background->Draw(commandList);
  
    // Draw pivot
    // Depth testing is disabled for the pivot and for better drawing the pivot is positioned after all objects.
    object.m_pivot->Draw(commandList);


    object.MSAA();

    // Bind null SRV for shadow map pass.
    commandList->SetGraphicsRootDescriptorTable(4, object.mNullSrv);
   
    object.DrawSceneToShadowMap(object.m_creatingPrimitives);

    object.DrawNormalsAndDepth(object.m_creatingPrimitives);

    //commandList->SetGraphicsRootSignature(object.m_rootSignature.GetSsaoRootSignature());
    //object.mSsao->ComputeSsao(commandList, object.m_SsaoCB->Resource()->GetGPUVirtualAddress(), 3);

    // Drawing symbols on a coordinate grid.
    object.m_grid->DrawSymbolsOnAGrid(
        commandList,
        object.m_deviceResources->GetScreenViewport(),
        object.m_deviceResources->GetScissorRect(),
        object.m_deviceResources->GetRenderTargetView(),
        descriptorHeaps
    );

    // Done recording commands.m_msaa ? D3D12_RESOURCE_STATE_RENDER_TARGET : D3D12_RESOURCE_STATE_PRESENT
    object.m_deviceResources->Present();

    object.m_grid->CommitGraphicsMemory(object.m_deviceResources->GetCommandQueue());
}

void Viewport::MSAA(this Viewport& object)
{
    auto commandList = object.m_deviceResources->GetCommandList();
    auto backBuffer = object.m_deviceResources->GetRenderTarget();

    if (object.m_msaa)
    {       
        // Resolve the MSAA render target.
        {
            D3D12_RESOURCE_BARRIER barriers[2] =
            {
                CD3DX12_RESOURCE_BARRIER::Transition(
                    object.m_msaaRenderTarget.Get(),
                    D3D12_RESOURCE_STATE_RENDER_TARGET,
                    D3D12_RESOURCE_STATE_RESOLVE_SOURCE),
                CD3DX12_RESOURCE_BARRIER::Transition(
                    backBuffer,
                    D3D12_RESOURCE_STATE_PRESENT,
                    D3D12_RESOURCE_STATE_RESOLVE_DEST)
            };

            commandList->ResourceBarrier(2, barriers);
        }

        commandList->ResolveSubresource(backBuffer,
            0, object.m_msaaRenderTarget.Get(), 0,
            object.m_backBufferFormat);
        {
            D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                backBuffer,
                D3D12_RESOURCE_STATE_RESOLVE_DEST,
                D3D12_RESOURCE_STATE_RENDER_TARGET);
            commandList->ResourceBarrier(1, &barrier);
        }
    }
}

void Viewport::OnMouseDown(LPARAM lParam, INT width, INT height, bool hasShift)
{
    using namespace LisaApp::Global;

    INT sx = GET_X_LPARAM(lParam);
    INT sy = GET_Y_LPARAM(lParam);

    bool isPivotSelected = m_pivot->OnLButtonDown(m_camera.GetView(), m_camera.GetProj4x4f(), sx, sy, width, height);

    // If a pivot is selected, then there is no need to be able to select an object.

    if (!isPivotSelected)
    {
        m_creatingPrimitives->UpdateContiguousIndexsContainer();

        m_creatingPrimitives->Pick(m_camera.GetView(), m_camera.GetProj4x4f(), sx, sy, width, height, hasShift);

        const std::uint32_t& mode = m_creatingPrimitives->GetPicking()->GetMode();

        if (mode == selection::face || mode == selection::edge || mode == selection::vertex)
        {
            m_pivot->SetOnOff(true);
            m_pivot->PlaceThePivotInTheDesiredPositionComponent(m_creatingPrimitives->GetRefWrapOnRender(), m_camera.GetLook(), mode);
        }

        if (m_creatingPrimitives->GetPicking()->GetMode() == selection::mesh)
        {
            // If the pivot is enabled, move it to the created object.
            if (m_creatingPrimitives->IsSelect() && (m_pivot->GetMode() != PivotMode::Off))
            {
                m_pivot->SetOnOff(true);
                m_pivot->PlaceThePivotInTheDesiredPosition(
                    m_creatingPrimitives->GetRefWrapOnRender(),
                    m_creatingPrimitives->GetLastID(),
                    m_camera.GetLook()
                );
            }
        }
    }
}

void Viewport::OnMouseUp()
{
    m_pivot->OnLButtonUp();
}

void Viewport::OnMouseMove(WPARAM btnState, LPARAM lParam, INT width, INT height)
{
    using namespace LisaApp::Global;

    INT sx = GET_X_LPARAM(lParam);
    INT sy = GET_Y_LPARAM(lParam);

    if (!m_pivot->GetClickOnPivot())
    {
        m_pivot->Pick(m_camera.GetView(), m_camera.GetProj4x4f(), sx, sy, width, height);
        m_pivot->SetColorModeSelectedComponent(PivotColorMode::PreSelect);
    }

    const std::uint32_t mode = m_creatingPrimitives->GetPicking()->GetMode();

    if (m_pivot->GetClickOnPivot())
    {
        IPivot::Attributes attr;
        bool move = m_pivot->OnLButtonMove(m_camera.GetView(), m_camera.GetProj4x4f(), sx, sy, width, height, attr);

        if (move)
        {
            if (mode == selection::mesh)
            {
                m_creatingPrimitives->EditingAttributes(attr, m_pivot->GetActiveAxis(), m_pivot->GetMode());
            }
            else if (mode == selection::face || mode == selection::edge || mode == selection::vertex)
            {
                m_creatingPrimitives->EditingComponents(attr, m_pivot->GetActiveAxis(), m_pivot->GetMode(), mode);
            }
        }
    }

    if (mode == selection::face || mode == selection::edge || mode == selection::vertex)
    {
        m_creatingPrimitives->PrePick(m_camera.GetView(), m_camera.GetProj4x4f(), sx, sy, width, height);
    }
}

bool Viewport::OnKeyDown(HWND hwnd, WPARAM wParam)
{
    using namespace LisaApp::Global;

    auto device = m_deviceResources->GetD3DDevice();

    bool ret = m_pivot->OnKeyDown(hwnd, wParam,
        m_creatingPrimitives->GetPicking()->GetMode(),
        m_creatingPrimitives->IsSelect(),
        m_creatingPrimitives->GetRefWrapOnRender(),
        m_creatingPrimitives->GetLastID(),
        m_camera.GetLook()
    );

    if (ret) return true;


    if (wParam == VK_DELETE)
    {
        DeleteSceneObjects();
    }
    else if (wParam == 0x46) // F
    {
        m_pivot->SetPivotDirty(true);

        //
        m_camera.BringCloserToObject(m_pivot->GetPosition());
        m_camera.UpdateViewMatrix();
    }
    else if (wParam == VK_ADD)
    {
        m_creatingPrimitives->CreateInstances(m_materials->GetMaterialIndex(L"shape"));
        m_pivot->PlaceThePivotInTheDesiredPosition(
            m_creatingPrimitives->GetRefWrapOnRender(),
            m_creatingPrimitives->GetLastID(),
            m_camera.GetLook()
        );
    }
    else if (wParam == 0x5A) // Z
    {
        if (m_creatingPrimitives->IsSelect() && (m_pivot->GetMode() != PivotMode::Off))
        {
            m_pivot->SetOnOff(true);
            m_pivot->PlaceThePivotInTheDesiredPosition(
                m_creatingPrimitives->GetRefWrapOnRender(),
                m_creatingPrimitives->GetLastID(),
                m_camera.GetLook()
            );
        }

        m_creatingPrimitives->UpdateContiguousIndexsContainer();
        m_creatingPrimitives->PreparingEditComponents(selection::mesh, m_creatingPrimitives->GetLastID());
    }
    else if (wParam == 0x58) // X
    {
        m_creatingPrimitives->UpdateContiguousIndexsContainer();
        m_creatingPrimitives->PreparingEditComponents(selection::face);
        m_pivot->PlaceThePivotInTheDesiredPositionComponent(m_creatingPrimitives->GetRefWrapOnRender(), m_camera.GetLook(), selection::face);
    }
    else if (wParam == 0x43) // C
    {
        m_creatingPrimitives->UpdateContiguousIndexsContainer();
        m_creatingPrimitives->PreparingEditComponents(selection::edge);
        m_pivot->PlaceThePivotInTheDesiredPositionComponent(m_creatingPrimitives->GetRefWrapOnRender(), m_camera.GetLook(), selection::edge);
    }
    else if (wParam == 0x56) // V
    {
        m_creatingPrimitives->UpdateContiguousIndexsContainer();
        m_creatingPrimitives->PreparingEditComponents(selection::vertex);
        m_pivot->PlaceThePivotInTheDesiredPositionComponent(m_creatingPrimitives->GetRefWrapOnRender(), m_camera.GetLook(), selection::vertex);
    }
    else if (wParam == 0x4F) // O
    {
        m_grid->ColorChange({ 0.0f,0.0f,0.0f,1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f,1.0f });
    }
    else if (wParam == 0x50) // P
    {
        m_grid->Resize(14.3f, 4.0f, 4);
    }

    InvalidateRect(hwnd, NULL, false);

    return true;
}

void Viewport::OnKeyboardInput()
{
    // j key.
    if (GetAsyncKeyState(0x4A) & 0x8000)
    {
        m_pivot->OnOffStep(true);
    }
    else
    {
        m_pivot->OnOffStep(false);
    }

    // left arrow key.
    //if (GetAsyncKeyState(0x25) & 0x8000)
    //{
    //    object.m_creatingPrimitives->EditingAttributes({ -0.1f, 0.0f, 0.0f });
    //}
    //// up arrow key. 
    //if (GetAsyncKeyState(0x26) & 0x8000)
    //{
    //    object.m_creatingPrimitives->EditingAttributes({ 0.0f, 0.0f, 0.1f });
    //}
    //// down arrow key.
    //if (GetAsyncKeyState(0x28) & 0x8000)
    //{
    //    object.m_creatingPrimitives->EditingAttributes({ 0.0f, 0.0f, -0.1f });
    //}
    //// PAGE UP key.
    //if (GetAsyncKeyState(VK_PRIOR) & 0x8000)
    //{
    //    object.m_creatingPrimitives->EditingAttributes({ 0.0f, 0.1f, 0.0f });
    //}
    //// PAGE DOWN key.
    //if (GetAsyncKeyState(VK_NEXT) & 0x8000)
    //{
    //    object.m_creatingPrimitives->EditingAttributes({ 0.0f, -0.1f, 0.0f });
    //}
    //
    //object.m_camera.UpdateViewMatrix();
}

void Viewport::UpdateSceneObjects(this Viewport& object)
{
    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    object.m_deviceResources->Prepare(object.m_msaa ? D3D12_RESOURCE_STATE_RENDER_TARGET : D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET/*, object.m_pipelineState.GetPipeline(L"opaque")*/);//2, 3, parametr ?????


    // Clear the views.
    object.ClearViews(false);

    object.MSAA();

    object.m_deviceResources->Present();
    
    //object.m_creatingPrimitives->Update();
}


void Viewport::UpdateObjectCBs(this Viewport& object)
{   
    object.m_creatingPrimitives->UpdateCB(object.m_camera.GetView(), object.mCamFrustum, false);
    object.m_background->UpdateCB(object.m_camera.GetView(), object.mCamFrustum, true);
    object.m_grid->UpdateCB(object.m_camera.GetView(), object.mCamFrustum, false);
    object.m_pivot->UpdateCB(object.m_camera.GetView(), object.mCamFrustum, false);
}

void Viewport::UpdateMaterialBuffer(this Viewport& object)
{
    Item::Material m;
    
    m = {
        .DiffuseSrvHeapIndex = 3,
        .NormalSrvHeapIndex = 4,
        .DiffuseAlbedo = DirectX::XMVECTORF32{ { { 1.000000000f, 1.000000000f, 1.000000000f, 1.f } } },
        .FresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f),
        .Roughness = 1.0f
    };

    object.m_materials->UpdateMaterial(L"sky", m);

    m = { .DiffuseSrvHeapIndex = 0, .NormalSrvHeapIndex = 0, .DiffuseAlbedo = {0.52f, 0.44f, 0.54f, 1.0f},
        .FresnelR0 = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), .Roughness = 0.99f
    };
    object.m_materials->UpdateMaterial(L"shape", m);

    m = { .DiffuseSrvHeapIndex = 1, .NormalSrvHeapIndex = 2, .DiffuseAlbedo = AppColors::HDR::TurquoiseBlue,
        .FresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f), .Roughness = 0.9f
    };
    object.m_materials->UpdateMaterial(L"shapeInst", m);
}

void Viewport::UpdateShadowTransform(this Viewport& object)
{
    using namespace DirectX;
    
    // Only the first "main" light casts a shadow.
    XMVECTOR lightDir = XMLoadFloat3(&object.mRotatedLightDirections);
    XMVECTOR lightPos = -2.0f* object.mSceneBounds.Radius*lightDir;
    XMVECTOR targetPos = XMLoadFloat3(&object.mSceneBounds.Center);
    XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

    XMStoreFloat3(&object.mLightPosW, lightPos);

    // Transform bounding sphere to light space.
    XMFLOAT3 sphereCenterLS;
    XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

    // Ortho frustum in light space encloses scene.
    float l = sphereCenterLS.x - object.mSceneBounds.Radius;
    float b = sphereCenterLS.y - object.mSceneBounds.Radius;
    float n = sphereCenterLS.z - object.mSceneBounds.Radius;
    float r = sphereCenterLS.x + object.mSceneBounds.Radius;
    float t = sphereCenterLS.y + object.mSceneBounds.Radius;
    float f = sphereCenterLS.z + object.mSceneBounds.Radius;

    object.mLightNearZ = n;
    object.mLightFarZ = f;
    XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

    // Transform NDC space [-1,+1]^2 to texture space [0,1]^2
    XMMATRIX T(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    XMMATRIX S = lightView*lightProj*T;
    DirectX::XMStoreFloat4x4(&object.mLightView, lightView);
    DirectX::XMStoreFloat4x4(&object.mLightProj, lightProj);
    DirectX::XMStoreFloat4x4(&object.mShadowTransform, S);
}

void Viewport::UpdateMainPassCB(this Viewport& object)
{
    DirectX::XMMATRIX view = object.m_camera.GetView();
    DirectX::XMMATRIX proj = object.m_camera.GetProj();

    DirectX::XMMATRIX viewProj = XMMatrixMultiply(view, proj);

    DirectX::XMVECTOR v{ XMMatrixDeterminant(view) };
    DirectX::XMMATRIX invView = XMMatrixInverse(&v, view);
    DirectX::XMVECTOR p{ XMMatrixDeterminant(proj) };
    DirectX::XMMATRIX invProj = XMMatrixInverse(&p, proj);
    DirectX::XMVECTOR vp{ XMMatrixDeterminant(viewProj) };
    DirectX::XMMATRIX invViewProj = XMMatrixInverse(&vp, viewProj);

    // Transform NDC space [-1,+1]^2 to texture space [0,1]^2
    DirectX::XMMATRIX T(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    DirectX::XMMATRIX viewProjTex = XMMatrixMultiply(viewProj, T);
    DirectX::XMMATRIX shadowTransform = DirectX::XMLoadFloat4x4(&object.mShadowTransform);

	XMStoreFloat4x4(&object.mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&object.mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&object.mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&object.mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&object.mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&object.mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
    XMStoreFloat4x4(&object.mMainPassCB.ViewProjTex, XMMatrixTranspose(viewProjTex));
    XMStoreFloat4x4(&object.mMainPassCB.ShadowTransform, XMMatrixTranspose(shadowTransform));
    object.mMainPassCB.EyePosW = object.m_camera.GetPosition3f();
    object.mMainPassCB.RenderTargetSize = DirectX::XMFLOAT2(static_cast<float>(object.m_width), static_cast<float>(object.m_height));
    object.mMainPassCB.InvRenderTargetSize = DirectX::XMFLOAT2(1.0f / object.m_width, 1.0f / object.m_height);
    object.mMainPassCB.NearZ = 1.0f;
    object.mMainPassCB.FarZ = 1000.0f;
    object.mMainPassCB.TotalTime = 0.0f;
    object.mMainPassCB.DeltaTime = 0.0f;
    object.mMainPassCB.AmbientLight = { 0.4f, 0.4f, 0.6f, 1.0f };
    object.mMainPassCB.Lights[0].Direction = object.mRotatedLightDirections;
    object.mMainPassCB.Lights[0].Strength = { 0.1f, 0.1f, 0.125f };
	//mMainPassCB.Lights[1].Direction = mRotatedLightDirections[1];
	//mMainPassCB.Lights[1].Strength = { 0.1f, 0.1f, 0.1f };
	//mMainPassCB.Lights[2].Direction = mRotatedLightDirections[2];
	//mMainPassCB.Lights[2].Strength = { 0.0f, 0.0f, 0.0f };
 
	auto currPassCB = object.m_PassCB.get();
	currPassCB->CopyData(0, object.mMainPassCB);
}

void Viewport::UpdateShadowPassCB(this Viewport& object)
{
    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&object.mLightView);
    DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&object.mLightProj);

    DirectX::XMMATRIX viewProj = XMMatrixMultiply(view, proj);

    DirectX::XMVECTOR v{ XMMatrixDeterminant(view) };
    DirectX::XMMATRIX invView = XMMatrixInverse(&v, view);
    DirectX::XMVECTOR p{ XMMatrixDeterminant(proj) };
    DirectX::XMMATRIX invProj = XMMatrixInverse(&p, proj);
    DirectX::XMVECTOR vp{ XMMatrixDeterminant(viewProj) };
    DirectX::XMMATRIX invViewProj = XMMatrixInverse(&vp, viewProj);

    UINT w = object.mShadowMap->Width();
    UINT h = object.mShadowMap->Height();

    XMStoreFloat4x4(&object.mShadowPassCB.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&object.mShadowPassCB.InvView, XMMatrixTranspose(invView));
    XMStoreFloat4x4(&object.mShadowPassCB.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&object.mShadowPassCB.InvProj, XMMatrixTranspose(invProj));
    XMStoreFloat4x4(&object.mShadowPassCB.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&object.mShadowPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
    object.mShadowPassCB.EyePosW = object.mLightPosW;
    object.mShadowPassCB.RenderTargetSize = DirectX::XMFLOAT2((float)w, (float)h);
    object.mShadowPassCB.InvRenderTargetSize = DirectX::XMFLOAT2(1.0f / w, 1.0f / h);
    object.mShadowPassCB.NearZ = object.mLightNearZ;
    object.mShadowPassCB.FarZ = object.mLightFarZ;

    auto currPassCB = object.m_PassCB.get();
    currPassCB->CopyData(1, object.mShadowPassCB);
}

void Viewport::UpdateSsaoCB(this Viewport& object)
{
    Constants::Ssao ssaoCB;

    DirectX::XMMATRIX P = object.m_camera.GetProj();

    // Transform NDC space [-1,+1]^2 to texture space [0,1]^2
    DirectX::XMMATRIX T(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    ssaoCB.Proj = object.mMainPassCB.Proj;
    ssaoCB.InvProj = object.mMainPassCB.InvProj;
    XMStoreFloat4x4(&ssaoCB.ProjTex, XMMatrixTranspose(P*T));

    object.mSsao->GetOffsetVectors(ssaoCB.OffsetVectors);

    auto blurWeights = object.mSsao->CalcGaussWeights(2.5f);
    ssaoCB.BlurWeights[0] = DirectX::XMFLOAT4(&blurWeights[0]);
    ssaoCB.BlurWeights[1] = DirectX::XMFLOAT4(&blurWeights[4]);
    ssaoCB.BlurWeights[2] = DirectX::XMFLOAT4(&blurWeights[8]);

    ssaoCB.InvRenderTargetSize = DirectX::XMFLOAT2(1.0f / object.mSsao->SsaoMapWidth(), 1.0f / object.mSsao->SsaoMapHeight());

    // Coordinates given in view space.
    ssaoCB.OcclusionRadius = 0.5f;
    ssaoCB.OcclusionFadeStart = 0.2f;
    ssaoCB.OcclusionFadeEnd = 1.0f;
    ssaoCB.SurfaceEpsilon = 0.05f;
 
    auto currSsaoCB = object.m_SsaoCB.get();
    currSsaoCB->CopyData(0, ssaoCB);
}


void Viewport::BuildDescriptorHeaps(this Viewport& object)
{
    auto device = object.m_deviceResources->GetD3DDevice();
    auto commandList = object.m_deviceResources->GetCommandList();

    // Adding images to the crv heap.   

    object.m_textures.insert({ L"custom", Textures::LoadTexture(device, commandList) });

    object.m_textures.insert({ L"white_color", Textures::LoadTexture(device, commandList,
        LisaApp::HelperPath(L"Resources\\Textures\\bricks2.dds")) });

    object.m_textures.insert({ L"default_nmap", Textures::LoadTexture(device, commandList,
        LisaApp::HelperPath(L"Resources\\Textures\\bricks2_nmap.dds")) });

    object.m_textures.insert({ L"SkyBoxAnime", Textures::LoadTexture(device, commandList,
        LisaApp::HelperPath(L"Resources\\Textures\\SkyBoxAnime.dds"), D3D12_SRV_DIMENSION_TEXTURECUBE)});

    UINT sumDescriptors = static_cast<UINT>(object.m_textures.size());

    // Create the SRV heap. 
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
    srvHeapDesc.NumDescriptors = sumDescriptors+11;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    DX::ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(object.mSrvDescriptorHeap.ReleaseAndGetAddressOf())));

    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptor{ object.mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
    auto Increment = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);


    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    //UINT srvDimension{};

    // The first texture in the heap has no offset.
    // Subsequent textures are offset by one.

    for (INT offset{ 0 }; const auto & t : object.m_textures)
    {
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = t.second->GetSrvDimension();

        const auto& gr = t.second->GetResource();
        
        srvDesc.Format = gr->GetDesc().Format;

        if (t.second->GetSrvDimension() == D3D12_SRV_DIMENSION_TEXTURE2DMS)
        {
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
            srvDesc.Texture2D.MipLevels = gr->GetDesc().MipLevels;
        }
        else if(t.second->GetSrvDimension() == D3D12_SRV_DIMENSION_TEXTURECUBE)
        {
            srvDesc.TextureCube.MostDetailedMip = 0;
            srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
            srvDesc.TextureCube.MipLevels = gr->GetDesc().MipLevels;
        }

        descriptor.Offset(offset ? 1 : 0, Increment);
        offset++;

        device->CreateShaderResourceView(gr, &srvDesc, descriptor);
    }

	
    object.mSkyTexHeapIndex = (UINT)object.m_textures.size() - 1; // = 3
    object.mShadowMapHeapIndex = object.mSkyTexHeapIndex + 1;//4
    object.mSsaoHeapIndexStart = object.mShadowMapHeapIndex + 1;//5
    object.mSsaoAmbientMapIndex = object.mSsaoHeapIndexStart + 3;//8
    object.mNullCubeSrvIndex = object.mSsaoHeapIndexStart + 5;//10
    object.mNullTexSrvIndex1 = object.mNullCubeSrvIndex + 1;//11
    object.mNullTexSrvIndex2 = object.mNullTexSrvIndex1 + 1;//12


    auto nullSrv = object.GetCpuSrv(object.mNullCubeSrvIndex);
    object.mNullSrv = object.GetGpuSrv(object.mNullCubeSrvIndex);

    device->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);
    nullSrv.Offset(1, object.m_deviceResources->GetCbvSrvUavDescriptorSize());

    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    device->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);

    nullSrv.Offset(1, object.m_deviceResources->GetCbvSrvUavDescriptorSize());
    device->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);


    object.mShadowMap->BuildDescriptors(
        object.GetCpuSrv(object.mShadowMapHeapIndex),
        object.GetGpuSrv(object.mShadowMapHeapIndex),
        object.GetDsv(1));

    object.mSsao->BuildDescriptors(
        object.m_deviceResources->GetDepthStencil(),
        object.GetCpuSrv(object.mSsaoHeapIndexStart),
        object.GetGpuSrv(object.mSsaoHeapIndexStart),
        object.GetRtv(object.m_deviceResources->GetBackBufferCount()),//!!
        object.m_deviceResources->GetCbvSrvUavDescriptorSize(),
        object.m_deviceResources->GetRtvDescriptorSize());

}

void Viewport::BuildPSOs()
{   
    auto device = m_deviceResources->GetD3DDevice();
	
    m_pipelineState = { m_rootSignature.GetPrepareSsaoRootSignature(), VertexStructs::VertexPositionNormalTextureTangentU::InputLayout };

    // PSO for debug layer.

    LisaApp::PSOConfig::EffectInitial einit;
    einit.pso.SampleDesc = { 1, 0 };
    m_pipelineState.CreatePipelineState(einit.pso, device, m_shader.ShadowDebug(), Debug);
    
    // PSO for SSAO.

    LisaApp::PSOConfig::EffectSSAO ssao;
    m_pipelineStateSSAO = { m_rootSignature.GetSsaoRootSignature(), VertexStructs::VertexPositionNormalTextureTangentU::InputLayout };
    m_pipelineStateSSAO.CreatePipelineState(ssao.pso, device, m_shader.SSAO(), SSAO);

    // PSO for SSAO blur.
    m_pipelineStateSSAO.CreatePipelineState(ssao.pso, device, m_shader.SSAOBlur(), SSAOBlur);
}

void Viewport::BuildMaterials(this Viewport& object)
{
    using namespace LisaApp::Global;

    // Background.

    object.m_materials->Create(L"sky", MATERIAL::BASE);

    // Primitives

    object.m_materials->Create(L"shape", MATERIAL::BASE);
    object.m_materials->Create(L"shapeInst", MATERIAL::BASE);
    object.m_materials->Create(L"mesh", MATERIAL::BASE);
    object.m_materials->Create(L"unSelectedMesh", MATERIAL::BASE);
    object.m_materials->Create(L"face", MATERIAL::BASE);
    object.m_materials->Create(L"edge", MATERIAL::BASE);
    object.m_materials->Create(L"vertex", MATERIAL::BASE);

    // Grid.

    object.m_materials->Create(L"gridCeneter", MATERIAL::BASE);
    object.m_materials->Create(L"gridContour", MATERIAL::BASE);
    object.m_materials->Create(L"gridSubdiv", MATERIAL::BASE);
}

void Viewport::DrawSceneToShadowMap(this Viewport& object, const std::unique_ptr<LisaApp::CreatingPrimitives>& sceneObjects)
{
    auto commandList = object.m_deviceResources->GetCommandList();
    
    D3D12_VIEWPORT viewport = object.mShadowMap->Viewport();
    D3D12_RECT scissorRect = object.mShadowMap->ScissorRect();
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    // Change to DEPTH_WRITE.
    D3D12_RESOURCE_BARRIER barrier = { CD3DX12_RESOURCE_BARRIER::Transition(
        object.mShadowMap->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE) };
    commandList->ResourceBarrier(1, &barrier);

    // Clear the back buffer and depth buffer.
    commandList->ClearDepthStencilView(object.mShadowMap->Dsv(),
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // Specify the buffers we are going to render to.
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuDsv = object.mShadowMap->Dsv();
    commandList->OMSetRenderTargets(0, nullptr, false, &mhCpuDsv);
    
    // Bind the pass constant buffer for the shadow map pass.
    UINT passCBByteSize = HelperUtilities::CalcConstantBufferByteSize(sizeof(Constants::Scene));
    auto passCB = object.m_PassCB->Resource();
    D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + 1 * static_cast<unsigned long long>(passCBByteSize);
    commandList->SetGraphicsRootConstantBufferView(3, passCBAddress);

    sceneObjects->DrawShadow(commandList);

    // Change back to GENERIC_READ so we can read the texture in a shader.
    barrier = { CD3DX12_RESOURCE_BARRIER::Transition(
        object.mShadowMap->Resource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ) };
    commandList->ResourceBarrier(1, &barrier);
}
 
void Viewport::DrawNormalsAndDepth(this Viewport& object, const std::unique_ptr<LisaApp::CreatingPrimitives>& sceneObjects)
{
    auto commandList = object.m_deviceResources->GetCommandList();
    
    D3D12_VIEWPORT viewport = object.m_deviceResources->GetScreenViewport();
    D3D12_RECT scissorRect = object.m_deviceResources->GetScissorRect();
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

	auto normalMap = object.mSsao->NormalMap();
	auto normalMapRtv = object.mSsao->NormalMapRtv();
	
    // Change to RENDER_TARGET.
    D3D12_RESOURCE_BARRIER barrier = { CD3DX12_RESOURCE_BARRIER::Transition(
        normalMap, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET) };
    commandList->ResourceBarrier(1, &barrier);

	// Clear the screen normal map and depth buffer.
	float clearValue[] = {0.0f, 0.0f, 1.0f, 0.0f};
    commandList->ClearRenderTargetView(normalMapRtv, clearValue, 0, nullptr);
    commandList->ClearDepthStencilView(
        object.m_deviceResources->GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsv = object.m_deviceResources->GetDepthStencilView();
    commandList->OMSetRenderTargets(1, &normalMapRtv, true, &dsv);
    
    // Bind the constant buffer for this pass.
    auto passCB = object.m_PassCB->Resource();
    commandList->SetGraphicsRootConstantBufferView(3, passCB->GetGPUVirtualAddress());

    sceneObjects->DrawNormals(commandList);

    // Change back to GENERIC_READ so we can read the texture in a shader.
    barrier = { CD3DX12_RESOURCE_BARRIER::Transition(
        normalMap, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ) };
    commandList->ResourceBarrier(1, &barrier);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Viewport::GetCpuSrv(this Viewport& object, INT index)
{
    auto srv = CD3DX12_CPU_DESCRIPTOR_HANDLE(object.mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    srv.Offset(index, object.m_deviceResources->GetCbvSrvUavDescriptorSize());
    return srv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Viewport::GetGpuSrv(this Viewport& object, INT index)
{
    auto srv = CD3DX12_GPU_DESCRIPTOR_HANDLE(object.mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    srv.Offset(index, object.m_deviceResources->GetCbvSrvUavDescriptorSize());
    return srv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Viewport::GetDsv(this Viewport& object, INT index)
{
    auto dsv = CD3DX12_CPU_DESCRIPTOR_HANDLE(object.m_SSAODSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    dsv.Offset(index, object.m_deviceResources->GetDsvDescriptorSize());
    return dsv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Viewport::GetRtv(this Viewport& object, INT index)
{
    auto rtv = CD3DX12_CPU_DESCRIPTOR_HANDLE(object.m_SSAORTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    rtv.Offset(index, object.m_deviceResources->GetRtvDescriptorSize());
    return rtv;
}
