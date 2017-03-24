#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <glm/glm.hpp>

class FrameBuffer
{
    public:
        // Constructor.
        // pDevice Pointer to D3D12 device.
        FrameBuffer(ID3D12Device* pDevice, unsigned int width, unsigned int height, DXGI_FORMAT format);

        FrameBuffer(ID3D12Device* pDevice, ID3D12Resource* pResouce);

        // Destructor.
        ~FrameBuffer();

        // Clear textures.
        void Clear(ID3D12GraphicsCommandList& commandList, float r = 0.f, float g = 0.f, float b = 0.f, float a = 0.f, float depth = 1.f);

		// Copy other frame buffer.
		void Copy(ID3D12GraphicsCommandList& commandList, FrameBuffer* fb);

        // Transition state.
        // commandList Command buffer to make transition
        // newState State to transition to.
        void TransitionState(ID3D12GraphicsCommandList& commandList, D3D12_RESOURCE_STATES newState);

        // Generate render target view.
        void GenerateRTV(ID3D12DescriptorHeap* heapRTV);

        // Frame buffer width in pixels.
        unsigned int mWidth;
        // Frame buffer height in pixels.
        unsigned int mHeight;
		// Format.
		DXGI_FORMAT mFormat;
        // State.
        D3D12_RESOURCE_STATES mState;

        // Color.
        ID3D12Resource* mResource;
        CD3DX12_CPU_DESCRIPTOR_HANDLE mRTV;
        //ID3D11ShaderResourceView* mColSRV;
        //ID3D11RenderTargetView* mColRTV;
        //ID3D11UnorderedAccessView* mColUAV;

        // Depth stencil.
        //ID3D11Texture2D* mDepthStencilTex;
        //ID3D11DepthStencilView* mDepthStencilDSV;

    private:
        ID3D12Device* mpDevice;
};
