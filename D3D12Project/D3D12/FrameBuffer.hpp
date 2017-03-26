#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <glm/glm.hpp>

class DeviceHeapMemory;

class FrameBuffer
{
    public:
        // Constructor.
        // pDevice Pointer to D3D12 device.
        FrameBuffer(ID3D12Device* pDevice, unsigned int width, unsigned int height, DXGI_FORMAT format, ID3D12Resource* pInitResouce = nullptr);

        // Destructor.
        ~FrameBuffer();

        // Clear textures.
        void Clear(ID3D12GraphicsCommandList* pCommandList, float r = 0.f, float g = 0.f, float b = 0.f, float a = 0.f, float depth = 1.f);

		// Copy other frame buffer.
		void Copy(ID3D12GraphicsCommandList* pCommandList, FrameBuffer* fb);

        // Transition state.
        // commandList Command buffer to make transition
        // newState State to transition to.
        void TransitionState(ID3D12GraphicsCommandList* pCommandList, D3D12_RESOURCE_STATES newState);

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

    private:
        ID3D12Device* mpDevice;
        DeviceHeapMemory* mDeviceHeapMemory;
};
