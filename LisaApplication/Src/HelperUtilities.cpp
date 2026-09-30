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

#include "HelperUtilities.h"

Microsoft::WRL::ComPtr<ID3D12Resource> HelperUtilities::CreateDefaultBuffer(
    ID3D12Device* device,
    ID3D12GraphicsCommandList* cmdList,
    const void* initData,
    UINT64 byteSize,
    Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
    Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

    // Create the actual default buffer resource.
    const CD3DX12_HEAP_PROPERTIES heapDefault{ D3D12_HEAP_TYPE_DEFAULT };
    const CD3DX12_RESOURCE_DESC resDesc{ resDesc.Buffer(byteSize) };
    DX::ThrowIfFailed(device->CreateCommittedResource(
        &heapDefault,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

    // In order to copy CPU memory data into our default buffer, we need to create
    // an intermediate upload heap. 
    const CD3DX12_HEAP_PROPERTIES heapUpload{ D3D12_HEAP_TYPE_UPLOAD };
    DX::ThrowIfFailed(device->CreateCommittedResource(
        &heapUpload,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(uploadBuffer.GetAddressOf())));


    // Describe the data we want to copy into the default buffer.
    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData = initData;
    subResourceData.RowPitch = byteSize;
    subResourceData.SlicePitch = subResourceData.RowPitch;

    // Schedule to copy the data to the default buffer resource.  
    // At a high level, the helper function UpdateSubresources
    // will copy the CPU memory into the intermediate upload heap.  
    // Then, using ID3D12CommandList::CopySubresourceRegion,
    // the intermediate upload heap data will be copied to mBuffer.
    const CD3DX12_RESOURCE_BARRIER pResource1{ 
        pResource1.Transition(
            defaultBuffer.Get(),
            D3D12_RESOURCE_STATE_COMMON, 
            D3D12_RESOURCE_STATE_COPY_DEST
        ) 
    };
    cmdList->ResourceBarrier(1, &pResource1);

    UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
    const CD3DX12_RESOURCE_BARRIER pResource2{
        pResource2.Transition(
            defaultBuffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_GENERIC_READ
        )
    };
    cmdList->ResourceBarrier(1, &pResource2);

    // Note: uploadBuffer has to be kept alive after the above function calls because
    // the command list has not been executed yet that performs the actual copy.
    // The caller can Release the uploadBuffer after it knows the copy has been executed.


    return defaultBuffer;
}
