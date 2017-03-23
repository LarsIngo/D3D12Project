#pragma once

#include <d3d12.h>
#include <glm/glm.hpp>

class FrameBuffer
{
    public:
        // Constructor.
        // pDevice Pointer to D3D11 device.
        // pDeviceContext Pointer to D3D11 device context.
        // width Width in pixels.
        // height Height in pixels.
        // bindFlags Bind flags. DEFAULT [D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS]
		// miscFlags Misc flags. DEFAULT [0]
        // initTexture Initialised texture. DEFAULT [nullptr]
        FrameBuffer(/*ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, unsigned int width, unsigned int height, UINT bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS, UINT miscFlags = 0, ID3D11Texture2D* initTexture = nullptr*/);

        // Destructor.
        ~FrameBuffer();

        // Clear textures.
        void Clear(float r = 0.f, float g = 0.f, float b = 0.f, float a = 0.f, float depth = 1.f);

		// Copy other frame buffer.
		void Copy(FrameBuffer* fb);

        // Frame buffer width in pixels.
        unsigned int mWidth;
        // Frame buffer height in pixels.
        unsigned int mHeight;
		// Number of mip levels.
		unsigned int mMipLevels;

        // Color.
        //ID3D11Texture2D* mColTex;
        //ID3D11ShaderResourceView* mColSRV;
        //ID3D11RenderTargetView* mColRTV;
        //ID3D11UnorderedAccessView* mColUAV;

        // Depth stencil.
        //ID3D11Texture2D* mDepthStencilTex;
        //ID3D11DepthStencilView* mDepthStencilDSV;

    private:
        //ID3D11Device* mpDevice;
        //ID3D11DeviceContext* mpDeviceContext;
};
