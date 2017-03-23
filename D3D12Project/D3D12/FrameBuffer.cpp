#include "FrameBuffer.hpp"
#include "../Tools/D3D12Tools.hpp"

FrameBuffer::FrameBuffer(ID3D12Device* pDevice, CD3DX12_CPU_DESCRIPTOR_HANDLE* pResourceHandle)
{
    mpDevice = pDevice;
    mMyResource = true;

    // http://stackoverflow.com/questions/35568302/what-is-the-d3d12-equivalent-of-d3d11-createtexture2d
    assert(pResourceHandle != nullptr);
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.Texture2D.PlaneSlice = 0;
    mpDevice->CreateRenderTargetView(mResource, nullptr, *pResourceHandle);
    pResourceHandle->Offset(1, mpDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
   
    D3D12_RESOURCE_DESC resourceDesc = mResource->GetDesc();
    mWidth = resourceDesc.Width;
    mHeight = resourceDesc.Height;
}

FrameBuffer::FrameBuffer(ID3D12Device* pDevice, ID3D12Resource* pResouce)
{
    mpDevice = pDevice;
    mResource = pResouce;
    mMyResource = false;

    D3D12_RESOURCE_DESC resourceDesc = mResource->GetDesc();
    mWidth = resourceDesc.Width;
    mHeight = resourceDesc.Height;
}

FrameBuffer::~FrameBuffer()
{
    if(mMyResource) SAFE_RELEASE(mResource);

    //if (mColTex != nullptr) mColTex->Release();
    //if (mColSRV != nullptr) mColSRV->Release();
    //if (mColRTV != nullptr) mColRTV->Release();
    //if (mColUAV != nullptr) mColUAV->Release();
}

void FrameBuffer::Clear(const ID3D12CommandList& commandList, float r, float g, float b, float a, float depth)
{
    //float clrColor[4] = { r, g, b, a };
    //float clrWorld[4] = { 0.f, 0.f, 0.f, 0.f };
    //float clrNorm[4] = { 0.f, 0.f, 0.f, 0.f };
    //float clrDepth[4] = { 0.f, 0.f, 0.f, 0.f };
    //if (mColRTV != nullptr) mpDeviceContext->ClearRenderTargetView(mColRTV, clrColor);
}

void FrameBuffer::Copy(const ID3D12CommandList& commandList, FrameBuffer* fb)
{
 //   assert(fb != this);
	//assert(mWidth == fb->mWidth && mHeight == fb->mHeight);
 //   assert(mMipLevels == fb->mMipLevels);

 //   DxHelp::CopyTexture(mpDeviceContext, mColTex, fb->mColTex, mWidth, mHeight, mMipLevels);
}

void FrameBuffer::TransitionImageLayout(const ID3D12CommandList& commandList/*, VkImageLayout newLayout*/)
{

}
