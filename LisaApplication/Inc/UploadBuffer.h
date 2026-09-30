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

#pragma once

#include "pch.h"
#include "HelperUtilities.h"

template<typename T>
class UploadBuffer
{
public:
    UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) : 
        m_isConstantBuffer(isConstantBuffer)
    {
        m_elementByteSize = sizeof(T);

        // Constant buffer elements need to be multiples of 256 bytes.
        // This is because the hardware can only view constant data 
        // at m*256 byte offsets and of n*256 byte lengths. 
        // typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
        // UINT64 OffsetInBytes; // multiple of 256
        // UINT   SizeInBytes;   // multiple of 256
        // } D3D12_CONSTANT_BUFFER_VIEW_DESC;
        if(isConstantBuffer)
            m_elementByteSize = HelperUtilities::CalcConstantBufferByteSize(sizeof(T));

        const CD3DX12_HEAP_PROPERTIES heapUpload{ D3D12_HEAP_TYPE_UPLOAD };
        const CD3DX12_RESOURCE_DESC resDesc{ resDesc.Buffer(m_elementByteSize * elementCount) };
        DX::ThrowIfFailed(device->CreateCommittedResource(
            &heapUpload,
            D3D12_HEAP_FLAG_NONE,
            &resDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_uploadBuffer)));

        DX::ThrowIfFailed(m_uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedData)));

        // We do not need to unmap until we are done with the resource.  However, we must not write to
        // the resource while it is in use by the GPU (so we must use synchronization techniques).
    }

    UploadBuffer(const UploadBuffer& rhs) = delete;
    UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
    ~UploadBuffer()
    {
        if(m_uploadBuffer != nullptr)
            m_uploadBuffer->Unmap(0, nullptr);

        m_mappedData = nullptr;
    }

    ID3D12Resource* Resource()const
    {
        return m_uploadBuffer.Get();
    }

    void CopyData(std::int32_t elementIndex, const T& data)
    {
        memcpy(&m_mappedData[elementIndex* m_elementByteSize], &data, sizeof(T));
    }

    T* GetData(std::int32_t elementIndex)
    {
        return reinterpret_cast<T*>(&m_mappedData[elementIndex * m_elementByteSize]);
    }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer;
    BYTE* m_mappedData = nullptr;

    UINT m_elementByteSize = 0;
    bool m_isConstantBuffer = false;
};