#include "FrameBuffer.hpp"

FrameBuffer::FrameBuffer(/*ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, unsigned int width, unsigned int height, UINT bindFlags, UINT miscFlags, ID3D11Texture2D* initTexture*/)
{
    //mpDevice = pDevice;
    //mpDeviceContext = pDeviceContext;
    //mWidth = width;
    //mHeight = height;

    //mColTex = nullptr;
    //mColSRV = nullptr;
    //mColRTV = nullptr;
    //mColUAV = nullptr;

    ////mDepthStencilTex = nullptr;
    ////mDepthStencilDSV = nullptr;

    //D3D11_TEXTURE2D_DESC texDesc;
    //ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
    //texDesc.Width = mWidth;
    //texDesc.Height = mHeight;
    //texDesc.MipLevels = 1;
    //texDesc.ArraySize = 1;
    //texDesc.SampleDesc.Count = 1;
    //texDesc.SampleDesc.Quality = 0;
    //texDesc.Usage = D3D11_USAGE_DEFAULT;
    //texDesc.CPUAccessFlags = 0;
    //texDesc.MiscFlags = miscFlags;

    //// Color.
    //texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //texDesc.BindFlags = bindFlags;
    //if (initTexture == nullptr) { DxAssert(mpDevice->CreateTexture2D(&texDesc, NULL, &mColTex), S_OK); } else mColTex = initTexture;
    //if (texDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE) DxAssert(mpDevice->CreateShaderResourceView(mColTex, NULL, &mColSRV), S_OK);
    //if (texDesc.BindFlags & D3D11_BIND_RENDER_TARGET) DxAssert(mpDevice->CreateRenderTargetView(mColTex, NULL, &mColRTV), S_OK);
    //if (texDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) DxAssert(mpDevice->CreateUnorderedAccessView(mColTex, NULL, &mColUAV), S_OK);
    //// Generate mip levels.
    //if (miscFlags == D3D11_RESOURCE_MISC_GENERATE_MIPS)
    //    mpDeviceContext->GenerateMips(mColSRV);
    //// Get desc info.
    //D3D11_TEXTURE2D_DESC desc;
    //mColTex->GetDesc(&desc);
    //mMipLevels = desc.MipLevels;
}

FrameBuffer::~FrameBuffer()
{
    //if (mColTex != nullptr) mColTex->Release();
    //if (mColSRV != nullptr) mColSRV->Release();
    //if (mColRTV != nullptr) mColRTV->Release();
    //if (mColUAV != nullptr) mColUAV->Release();
}

void FrameBuffer::Clear(float r, float g, float b, float a, float depth)
{
    //float clrColor[4] = { r, g, b, a };
    //float clrWorld[4] = { 0.f, 0.f, 0.f, 0.f };
    //float clrNorm[4] = { 0.f, 0.f, 0.f, 0.f };
    //float clrDepth[4] = { 0.f, 0.f, 0.f, 0.f };
    //if (mColRTV != nullptr) mpDeviceContext->ClearRenderTargetView(mColRTV, clrColor);
}

void FrameBuffer::Copy(FrameBuffer* fb)
{
 //   assert(fb != this);
	//assert(mWidth == fb->mWidth && mHeight == fb->mHeight);
 //   assert(mMipLevels == fb->mMipLevels);

 //   DxHelp::CopyTexture(mpDeviceContext, mColTex, fb->mColTex, mWidth, mHeight, mMipLevels);
}
