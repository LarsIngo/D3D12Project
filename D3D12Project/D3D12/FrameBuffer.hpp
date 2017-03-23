#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <glm/glm.hpp>

class FrameBuffer
{
    public:
        // Constructor.
        // pDevice Pointer to D3D12 device.
        FrameBuffer(ID3D12Device* pDevice, CD3DX12_CPU_DESCRIPTOR_HANDLE* pResourceHandle);

        FrameBuffer(ID3D12Device* pDevice, ID3D12Resource* pResouce);

        // Destructor.
        ~FrameBuffer();

        // Clear textures.
        void Clear(const ID3D12CommandList& commandList, float r = 0.f, float g = 0.f, float b = 0.f, float a = 0.f, float depth = 1.f);

		// Copy other frame buffer.
		void Copy(const ID3D12CommandList& commandList, FrameBuffer* fb);

        // Transition image layout.
        // commandbuffer Command buffer to make transition
        // newLayout Layout to transition to.
        void TransitionImageLayout(const ID3D12CommandList& commandList/*, VkImageLayout newLayout*/);

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
        ID3D12Device* mpDevice;
        ID3D12Resource* mResource;
        bool mMyResource;
};
