#include "FrameBuffer.hpp"
#include "DeviceHeapMemory.hpp"
#include "../Tools/D3D12Tools.hpp"

FrameBuffer::FrameBuffer(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory, unsigned int width, unsigned int height, DXGI_FORMAT format, ID3D12Resource* pInitResouce)
{
    mpDevice = pDevice;
    mpDeviceHeapMemory = pDeviceHeapMemory;

    mWidth = width;
    mHeight = height;
    mFormat = format;

    if (pInitResouce == nullptr)
    {
        D3D12_RESOURCE_DESC resouceDesc;
        ZeroMemory(&resouceDesc, sizeof(resouceDesc));
        resouceDesc.MipLevels = 1;
        resouceDesc.Format = mFormat;
        resouceDesc.Width = mWidth;
        resouceDesc.Height = mHeight;
        resouceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        resouceDesc.DepthOrArraySize = 1;
        resouceDesc.SampleDesc.Count = 1;
        resouceDesc.SampleDesc.Quality = 0;
        resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resouceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resouceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = mFormat;
        clearValue.Color[0] = 0.2f;
        clearValue.Color[1] = 0.2f;
        clearValue.Color[2] = 0.2f;
        clearValue.Color[3] = 0.0f;
        ASSERT(mpDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &resouceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            &clearValue,
            IID_PPV_ARGS(&mResource)), S_OK);
    }
    else
    {
        mResource = pInitResouce;
    }

    mResource->SetName(L"Frame buffer");
    mRTV = mpDeviceHeapMemory->GenerateRTV(nullptr, mResource);
}

FrameBuffer::~FrameBuffer()
{
    SAFE_RELEASE(mResource);
}

void FrameBuffer::Clear(ID3D12GraphicsCommandList* pCommandList, float r, float g, float b, float a, float depth)
{
    TransitionState(pCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    float clrColor[4] = { r, g, b, a };
    pCommandList->ClearRenderTargetView(mRTV, clrColor, 0, nullptr);
}

void FrameBuffer::Copy(ID3D12GraphicsCommandList* pCommandList, FrameBuffer* fb)
{
    assert(fb != this);
	assert(mWidth == fb->mWidth && mHeight == fb->mHeight);

    fb->TransitionState(pCommandList, D3D12_RESOURCE_STATE_COPY_SOURCE);

    TransitionState(pCommandList, D3D12_RESOURCE_STATE_COPY_DEST);

    pCommandList->CopyResource(mResource, fb->mResource);
}

void FrameBuffer::TransitionState(ID3D12GraphicsCommandList* pCommandList, D3D12_RESOURCE_STATES newState)
{
    if (mState == newState) return;

    D3D12Tools::TransitionState(pCommandList, mResource, mState, newState);
    mState = newState;
}
