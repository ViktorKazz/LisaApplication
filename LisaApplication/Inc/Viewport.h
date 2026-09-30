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

#ifndef VIEWPORT_CLASS_H
#define VIEWPORT_CLASS_H

#include "MathHelper.h"
#include "UploadBuffer.h"
#include "Camera.h"

#include "ShadowMap.h"
#include "Ssao.h"
#include <windowsx.h>

#include "DeviceResources.h"
#include "AnimationHelper.h"

#include "RootSignature.h"
#include "PipelineState.h"
#include "UploadShaders.h"

#include "VertexStructs.h"

#include "Materials.h"
#include "Textures.h"
#include "CustomTextures.h"
#include "PolygonPrimitives.h"
#include "HelperUtilities.h"
#include "GameTimer.h"

#include "AppColors.h"
#include "Pivot.h"
#include "Grid.h"
#include "Background.h"
#include "Picking.h"

#include "CreatingPrimitives.h"

#include "Elements.h"

class Viewport : public UI::WndInitialize, public UI::WindowTransformation, public DX::IDeviceNotify
{
public:
    using WndInitialize::WndInitialize;

    Viewport(const Viewport& rhs) = delete;
    Viewport& operator=(const Viewport& rhs) = delete;

    ~Viewport();

    // Accessors.

    //auto GetStyle()                   const noexcept { return m_style; };
    auto GetTransform()               const noexcept { return m_transform; };

    bool InitializationResources(HWND hwnd, int width, int height);

    void CreateDeviceDependentResources(this Viewport& object);
    void CreateWindowSizeDependentResources(this Viewport& object);

    void OnMouseDown(LPARAM lParam, INT width, INT height, bool hasShift = false);
    void OnMouseUp();
    void OnMouseMove(WPARAM btnState, LPARAM lParam, INT width, INT height);
    bool OnKeyDown(HWND hwnd, WPARAM wParam);
    void OnKeyboardInput();
    //void OnPivot(LONG width, LONG height);

    void Update();
    void UpdateSceneObjects(this Viewport& object);
    void UpdateObjectCBs(this Viewport& object);
    void UpdateMaterialBuffer(this Viewport& object);
    void UpdateShadowTransform(this Viewport& object);
    void UpdateMainPassCB(this Viewport& object);
    void UpdateShadowPassCB(this Viewport& object);
    void UpdateSsaoCB(this Viewport& object);

    void BuildDescriptorHeaps(this Viewport& object);
    void BuildPSOs();
    void BuildMaterials(this Viewport& object);

    void Draw(this Viewport& object);
    void MSAA(this Viewport& object);

    // Helper method to clear the back buffers.
    void ClearViews(this Viewport& object, bool clearRTVandDSV = true);

    void CreateSceneObjects(const LisaApp::PrimitivesData& primitivesData, UINT primitives);
    void DeleteSceneObjects();

    // A function that must be located before those functions that need to be executed after the command list is closed.
    void CommandListClose();

    void DrawSceneToShadowMap(this Viewport& object, const std::unique_ptr<LisaApp::CreatingPrimitives>& sceneObjects);
    void DrawNormalsAndDepth(this Viewport& object, const std::unique_ptr<LisaApp::CreatingPrimitives>& sceneObjects);

    float AspectRatio(this Viewport& object);

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    //LRESULT CALLBACK MessageHandled(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override { return 0; };

    std::unique_ptr<Viewport> Perspective(
        HINSTANCE hInstance,
        const RECT& rect,
        const std::variant<std::wstring, std::pair<std::wstring, std::wstring>>& varName,
        UI::ui_type type,
        UI::ui_modes style,
        UI::ui_transform transform,
        HWND parentHwnd
    );

    void OnWindowSizeChanged(this Viewport& object, INT width, INT height);
    // The main window message handler.
    static LRESULT CALLBACK ViewportWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuSrv(this Viewport& object, INT index);
    CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuSrv(this Viewport& object, INT index);
    CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv(this Viewport& object, INT index);
    CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(this Viewport& object, INT index);

private:
    // Device resources.
    std::unique_ptr<DX::DeviceResources>            m_deviceResources{ nullptr };

    UI::ui_modes m_style{};
    UI::ui_transform m_transform{};

    // Viewport dimensions.
    INT m_width{};
    INT m_height{};

    HWND m_parentHwnd{};

    INT m_parentWindowHeight{};
    INT m_parentWindowWidth{};

    std::unique_ptr<LisaApp::Grid> m_grid = std::make_unique<LisaApp::Grid>();
    std::unique_ptr<LisaApp::Background> m_background = std::make_unique<LisaApp::Background>();
    std::unique_ptr<LisaApp::Materials> m_materials = std::make_unique<LisaApp::Materials>();
    std::unique_ptr<LisaApp::CreatingPrimitives> m_creatingPrimitives = std::make_unique<LisaApp::CreatingPrimitives>();

    std::unordered_map<std::wstring, std::unique_ptr<Textures>> m_textures;

    // We cannot update a cbuffer until the GPU is done processing the commands
    // that reference it.  So each frame needs their own cbuffers.
    std::unique_ptr<UploadBuffer<Constants::Scene>> m_PassCB{ nullptr };
    std::unique_ptr<UploadBuffer<Constants::Ssao>> m_SsaoCB{ nullptr };

    DirectX::BoundingFrustum mCamFrustum;

    RootSignature m_rootSignature;

    // PSOs

    enum Mode : std::uint32_t
    {
        Debug = 0,
        SSAO,
        SSAOBlur
    };

    PipelineState m_pipelineState;
    PipelineState m_pipelineStateSSAO;
    UploadShaders m_shader;




    DXGI_FORMAT m_backBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM };
    DXGI_FORMAT m_depthBufferFormat{ DXGI_FORMAT_D24_UNORM_S8_UINT };

    UINT m_targetSampleCount{};
    

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

    UINT mSkyTexHeapIndex{ 0 };
    UINT mShadowMapHeapIndex{ 0 };
    UINT mSsaoHeapIndexStart{ 0 };
    UINT mSsaoAmbientMapIndex{ 0 };

    UINT mNullCubeSrvIndex{ 0 };
    UINT mNullTexSrvIndex1{ 0 };
    UINT mNullTexSrvIndex2{ 0 };

    CD3DX12_GPU_DESCRIPTOR_HANDLE mNullSrv;

    Constants::Scene mMainPassCB;  // index 0 of pass cbuffer.
    Constants::Scene mShadowPassCB;// index 1 of pass cbuffer.

    // Camera

    Camera m_camera;


    std::unique_ptr<ShadowMap> mShadowMap;


    DirectX::BoundingSphere mSceneBounds;
    
    POINT mLastMousePos{};

    // Light

    float mLightNearZ{ 0.0f };
    float mLightFarZ{ 0.0f };
    DirectX::XMFLOAT3 mLightPosW{};
    DirectX::XMFLOAT4X4 mLightView = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mLightProj = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mShadowTransform = MathHelper::Identity4x4();

    float mLightRotationAngle{ 0.0f };
    
    DirectX::XMFLOAT3 mBaseLightDirections{ DirectX::XMFLOAT3(0.57735f, -0.57735f, 0.57735f) };
    DirectX::XMFLOAT3 mRotatedLightDirections{};

    // SSAO

    std::unique_ptr<Ssao> mSsao;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>    m_SSAORTVDescriptorHeap{ nullptr };
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>    m_SSAODSVDescriptorHeap{ nullptr };

    // MSAA resources.
    Microsoft::WRL::ComPtr<ID3D12Resource>          m_msaaRenderTarget{ nullptr };
    Microsoft::WRL::ComPtr<ID3D12Resource>          m_msaaDepthStencil{ nullptr };

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>    m_msaaRTVDescriptorHeap{ nullptr };
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>    m_msaaDSVDescriptorHeap{ nullptr };

    UINT                                            m_sampleCount{};
    UINT                                            m_sampleQuality{};
    bool                                            m_msaa{ true };

    bool      mAppPaused = false;  // is the application paused?
    bool      mMinimized = false;  // is the application minimized?
    bool      mMaximized = false;  // is the application maximized?
    bool      mResizing = false;   // are the resize bars being dragged?
    bool      mFullscreenState = false;// fullscreen enabled


    // Used to keep track of the “delta-time” and game time (§4.4).

    GameTimer mTimer;

    //INT m_moveItem{};

    DirectX::XMFLOAT3 m_translation{};
    DirectX::XMVECTORF32 m_meshColor{ AppColors::HDR::LemonYellow };

    // Folders/paths for resources after unpacking archives.
    std::filesystem::path m_newShadersPath;

    // To create a pivot.
    std::unique_ptr<LisaApp::Pivot> m_pivot = std::make_unique<LisaApp::Pivot>();
};

#endif // !VIEWPORT_CLASS_H
