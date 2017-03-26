#include "StorageBuffer.hpp"
#include "DeviceHeapMemory.hpp"
#include "../Tools/D3D12Tools.hpp"

StorageBuffer::StorageBuffer(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory, unsigned int totalSize, unsigned int stride)
{
    mpDevice = pDevice;
    mpDeviceHeapMemory = pDeviceHeapMemory;
    mSize = totalSize;
    mStride = stride;
    
    {   // Buffer.
        D3D12_RESOURCE_DESC resouceDesc;
        ZeroMemory(&resouceDesc, sizeof(resouceDesc));
        resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resouceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        resouceDesc.Width = mSize;
        resouceDesc.Height = 1;
        resouceDesc.DepthOrArraySize = 1;
        resouceDesc.MipLevels = 1;
        resouceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resouceDesc.SampleDesc.Count = 1;
        resouceDesc.SampleDesc.Quality = 0;
        resouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resouceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        ASSERT(mpDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &resouceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&mBuff)), S_OK);
    }
    mBuff->SetName(L"Storage buffer");

    {   // SRV.
        D3D12_SHADER_RESOURCE_VIEW_DESC srcDesc;
        ZeroMemory(&srcDesc, sizeof(srcDesc));
        srcDesc.Format = DXGI_FORMAT_UNKNOWN;
        srcDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srcDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srcDesc.Buffer.FirstElement = 0;
        srcDesc.Buffer.NumElements = mSize / mStride;
        srcDesc.Buffer.StructureByteStride = mStride;
        srcDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        mSRV = mpDeviceHeapMemory->GenerateSRV(&srcDesc, mBuff);
    }

    //{   // UAV.
    //    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    //    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    //    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    //    uavDesc.Buffer.FirstElement = 0;
    //    uavDesc.Buffer.NumElements = mSize / mStride;
    //    uavDesc.Buffer.StructureByteStride = mStride;
    //    uavDesc.Buffer.CounterOffsetInBytes = 0;
    //    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
    //    mUAV = mpDeviceHeapMemory->GenerateUAV(&uavDesc, mBuff);
    //}

    {   // Staging Buffer.
        D3D12_RESOURCE_DESC resouceDesc;
        ZeroMemory(&resouceDesc, sizeof(resouceDesc));
        resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resouceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        resouceDesc.Width = mSize;
        resouceDesc.Height = 1;
        resouceDesc.DepthOrArraySize = 1;
        resouceDesc.MipLevels = 1;
        resouceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resouceDesc.SampleDesc.Count = 1;
        resouceDesc.SampleDesc.Quality = 0;
        resouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resouceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        ASSERT(mpDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &resouceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mStagingBuff)), S_OK);
    }
    mStagingBuff->SetName(L"Storage staging buffer");
}

StorageBuffer::~StorageBuffer()
{
    SAFE_RELEASE(mBuff);
    SAFE_RELEASE(mStagingBuff);
}

void StorageBuffer::Copy(ID3D12GraphicsCommandList* pCommandList, StorageBuffer* storageBuffer)
{
    assert(storageBuffer != this);
    assert(mSize == storageBuffer->GetSize());

    pCommandList->CopyResource(mBuff, storageBuffer->mBuff);
}

unsigned int StorageBuffer::GetSize()
{
    return mSize;
}

unsigned int StorageBuffer::GetStride()
{
    return mStride;
}

void StorageBuffer::Write(ID3D12GraphicsCommandList* pCommandList, void* data, unsigned int byteSize, unsigned int offset)
{
    assert(offset + byteSize <= mSize);

    void* mappedResource;
    CD3DX12_RANGE readRange(0, 0);
    CD3DX12_RANGE writeRange(offset, offset + byteSize);
    ASSERT(mStagingBuff->Map(0, &readRange, &mappedResource), S_OK);
    memcpy(reinterpret_cast<unsigned char*>(mappedResource) + offset, data, byteSize);
    mStagingBuff->Unmap(0, &writeRange);

    pCommandList->CopyResource(mBuff, mStagingBuff);
}
