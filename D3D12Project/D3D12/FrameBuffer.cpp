#include "FrameBuffer.hpp"
#include "DeviceHeapMemory.hpp"
#include "../Tools/D3D12Tools.hpp"

FrameBuffer::FrameBuffer(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory, unsigned int width, unsigned int height, DXGI_FORMAT format, ID3D12Resource* pInitResouce)
{
    mpDevice = pDevice;
    mpDeviceHeapMemory = pDeviceHeapMemory;
    mWidth = width;
    mHeight = height;

    if (pInitResouce == nullptr)
    {
        // http://stackoverflow.com/questions/35568302/what-is-the-d3d12-equivalent-of-d3d11-createtexture2d
        D3D12_RESOURCE_DESC resouceDesc;
        ZeroMemory(&resouceDesc, sizeof(resouceDesc));
        resouceDesc.MipLevels = 1;
        resouceDesc.Format = format;
        resouceDesc.Width = width;
        resouceDesc.Height = height;
        resouceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        resouceDesc.DepthOrArraySize = 1;
        resouceDesc.SampleDesc.Count = 1;
        resouceDesc.SampleDesc.Quality = 0;
        resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resouceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resouceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        D3D12Tools::CreateResource(mpDevice, resouceDesc, &mResource);
    }
    else
    {
        mResource = pInitResouce;
    }

    mRTV = mpDeviceHeapMemory->GenerateRTV(mResource);
}

FrameBuffer::~FrameBuffer()
{
    SAFE_RELEASE(mResource);

    //if (mColTex != nullptr) mColTex->Release();
    //if (mColSRV != nullptr) mColSRV->Release();
    //if (mColRTV != nullptr) mColRTV->Release();
    //if (mColUAV != nullptr) mColUAV->Release();
}

void FrameBuffer::Clear(ID3D12GraphicsCommandList& commandList, float r, float g, float b, float a, float depth)
{
    //float clrColor[4] = { r, g, b, a };
    //float clrWorld[4] = { 0.f, 0.f, 0.f, 0.f };
    //float clrNorm[4] = { 0.f, 0.f, 0.f, 0.f };
    //float clrDepth[4] = { 0.f, 0.f, 0.f, 0.f };
    //if (mColRTV != nullptr) mpDeviceContext->ClearRenderTargetView(mColRTV, clrColor);
}

void FrameBuffer::Copy(ID3D12GraphicsCommandList& commandList, FrameBuffer* fb)
{
 //   assert(fb != this);
	//assert(mWidth == fb->mWidth && mHeight == fb->mHeight);
 //   assert(mMipLevels == fb->mMipLevels);

 //   DxHelp::CopyTexture(mpDeviceContext, mColTex, fb->mColTex, mWidth, mHeight, mMipLevels);
}

void FrameBuffer::TransitionState(ID3D12GraphicsCommandList& commandList, D3D12_RESOURCE_STATES newState)
{
    D3D12Tools::TransitionState(commandList, mResource, mState, newState);
    mState = newState;
}

//void FrameBuffer::GenerateRTV(ID3D12DescriptorHeap* heapRTV)
//{
//    mRTV = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapRTV->GetCPUDescriptorHandleForHeapStart());
//    //mRTV.Offset(1, mpDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
//    CD3DX12_CPU_DESCRIPTOR_HANDLE a = mRTV;
//    mpDevice->CreateRenderTargetView(mResource, nullptr, a);
//    mRTV.Offset(1, mpDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
//    CD3DX12_CPU_DESCRIPTOR_HANDLE b = mRTV;
//    //CD3DX12_CPU_DESCRIPTOR_HANDLE c = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapRTV->GetCPUDescriptorHandleForHeapStart());
//}