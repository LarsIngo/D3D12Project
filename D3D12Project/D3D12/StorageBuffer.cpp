#include "StorageBuffer.hpp"
#include "../Tools/D3D12Tools.hpp"

StorageBuffer::StorageBuffer(/*ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, unsigned int totalSize, unsigned int stride, UINT bindFlags*/)
{
    //mpDevice = pDevice;
    //mpDeviceContext = pDeviceContext;
    //mSize = totalSize;
    //mStride = stride;

    //mBuff = nullptr;
    //mSRV = nullptr;
    //mUAV = nullptr;

    //
    //{   // Buffer.
    //    D3D11_BUFFER_DESC bufferDesc;
    //    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    //    bufferDesc.ByteWidth = mSize;
    //    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    //    bufferDesc.BindFlags = bindFlags;
    //    bufferDesc.CPUAccessFlags = 0;
    //    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    //    bufferDesc.StructureByteStride = mStride;
    //    DxAssert(mpDevice->CreateBuffer(&bufferDesc, NULL, &mBuff), S_OK);
    //}

    //if (bindFlags & D3D11_BIND_SHADER_RESOURCE)
    //{   // SRV.
    //    D3D11_SHADER_RESOURCE_VIEW_DESC srcDesc;
    //    ZeroMemory(&srcDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    //    srcDesc.Format = DXGI_FORMAT_UNKNOWN;
    //    srcDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    //    srcDesc.Buffer.FirstElement = 0;
    //    srcDesc.Buffer.ElementOffset = 0;
    //    srcDesc.Buffer.NumElements = mSize / mStride;
    //    DxAssert(mpDevice->CreateShaderResourceView(mBuff, &srcDesc, &mSRV), S_OK);
    //}

    //if (bindFlags & D3D11_BIND_UNORDERED_ACCESS)
    //{   // UAV.
    //    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    //    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    //    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    //    uavDesc.Buffer.FirstElement = 0;
    //    uavDesc.Buffer.NumElements = mSize / mStride;
    //    uavDesc.Buffer.Flags = 0;
    //    DxAssert(mpDevice->CreateUnorderedAccessView(mBuff, &uavDesc, &mUAV), S_OK);
    //}

    //{   // Staging Buffer.
    //    D3D11_BUFFER_DESC bufferDesc;
    //    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    //    bufferDesc.ByteWidth = mSize;
    //    bufferDesc.Usage = D3D11_USAGE_STAGING;
    //    bufferDesc.BindFlags = 0;
    //    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    //    bufferDesc.MiscFlags = 0;
    //    bufferDesc.StructureByteStride = mStride;
    //    DxAssert(mpDevice->CreateBuffer(&bufferDesc, NULL, &mStagingBuff), S_OK);
    //}
}

StorageBuffer::~StorageBuffer()
{
    //mBuff->Release();
    //if (mSRV != nullptr) mSRV->Release();
    //if (mUAV != nullptr) mUAV->Release();
    //mStagingBuff->Release();
}

void StorageBuffer::Copy(StorageBuffer* storageBuffer)
{
    //assert(storageBuffer != this);
    //assert(mSize == storageBuffer->GetSize());

    //mpDeviceContext->CopyResource(mBuff, storageBuffer->mBuff);
}

unsigned int StorageBuffer::GetSize()
{
    return mSize;
}

unsigned int StorageBuffer::GetStride()
{
    return mStride;
}

void StorageBuffer::Write(void* data, unsigned int byteSize, unsigned int offset)
{
   /* assert(offset + byteSize <= mSize);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    DxAssert(mpDeviceContext->Map(mStagingBuff, 0, D3D11_MAP_WRITE, 0, &mappedResource), S_OK);
    memcpy(mappedResource.pData, reinterpret_cast<unsigned char*>(data) + offset, byteSize);
    mpDeviceContext->Unmap(mStagingBuff, 0);

    mpDeviceContext->CopyResource(mBuff, mStagingBuff);*/
}
