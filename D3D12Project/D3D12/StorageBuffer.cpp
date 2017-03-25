#include "StorageBuffer.hpp"
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
        resouceDesc.Height = 1;
        resouceDesc.DepthOrArraySize = 1;
        resouceDesc.MipLevels = 1;
        resouceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resouceDesc.SampleDesc.Count = 1;
        resouceDesc.SampleDesc.Quality = 0;
        resouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resouceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        D3D12Tools::CreateResource(mpDevice, resouceDesc, D3D12_HEAP_TYPE_DEFAULT, &mBuff);
    }

    {   // SRV.
        //D3D11_SHADER_RESOURCE_VIEW_DESC srcDesc;
        //ZeroMemory(&srcDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        //srcDesc.Format = DXGI_FORMAT_UNKNOWN;
        //srcDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        //srcDesc.Buffer.FirstElement = 0;
        //srcDesc.Buffer.ElementOffset = 0;
        //srcDesc.Buffer.NumElements = mSize / mStride;
        //DxAssert(mpDevice->CreateShaderResourceView(mBuff, &srcDesc, &mSRV), S_OK);
    }

    {   // UAV.
        //D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        //uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        //uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        //uavDesc.Buffer.FirstElement = 0;
        //uavDesc.Buffer.NumElements = mSize / mStride;
        //uavDesc.Buffer.Flags = 0;
        //DxAssert(mpDevice->CreateUnorderedAccessView(mBuff, &uavDesc, &mUAV), S_OK);
    }

    {   // Staging Buffer.
        D3D12_RESOURCE_DESC resouceDesc;
        ZeroMemory(&resouceDesc, sizeof(resouceDesc));
        resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resouceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        resouceDesc.Height = 1;
        resouceDesc.DepthOrArraySize = 1;
        resouceDesc.MipLevels = 1;
        resouceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resouceDesc.SampleDesc.Count = 1;
        resouceDesc.SampleDesc.Quality = 0;
        resouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resouceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        D3D12Tools::CreateResource(mpDevice, resouceDesc, D3D12_HEAP_TYPE_UPLOAD, &mBuff);
    }
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

    //D3D11_MAPPED_SUBRESOURCE mappedResource;
    //ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    //DxAssert(mpDeviceContext->Map(mStagingBuff, 0, D3D11_MAP_WRITE, 0, &mappedResource), S_OK);
    //memcpy(mappedResource.pData, reinterpret_cast<unsigned char*>(data) + offset, byteSize);
    //mpDeviceContext->Unmap(mStagingBuff, 0);

    pCommandList->CopyResource(mBuff, mStagingBuff);
}
