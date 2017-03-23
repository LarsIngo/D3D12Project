#pragma once

#ifdef _DEBUG
#define BUILD_ENABLE_D3D12_DEBUG
#endif

#include <GLFW/glfw3.h>
#include <d3d12.h>
#include <dxgi1_5.h>
#include <d3dcompiler.h>
#include <d3dx12.h>

#include <vector>
class FrameBuffer;

class D3D12Renderer
{
    public:
        // Constructor.
        // winWidth Window width in pixels.
        // winHeight Window height in pixels.
        D3D12Renderer(unsigned int winWidth = 640, unsigned int winHeight = 640);

        // Destructor.
        ~D3D12Renderer();

        // Whether window is running of not.
        bool Running() const;

        // Close window.
        void Close();

        // Swap back buffer.
        // Returns next active frame buffer to present to window.
        FrameBuffer* SwapBackBuffer();

        // Present active back buffer.
        void PresentBackBuffer();

        // GLFW window.
        GLFWwindow* mGLFWwindow;

        // D3D12 device.
        ID3D12Device* mDevice;
        ID3D12CommandQueue* mCommandQueue;
        ID3D12DescriptorHeap* mDescriptorHeap;
        CD3DX12_CPU_DESCRIPTOR_HANDLE* mResouceHandle;

    private:
        void InitialiseGLFW();
        void DeInitialiseGLFW();

        void InitialiseD3D12();
        void DeInitialiseD3D12();

        //D3D12
        IDXGISwapChain4* mSwapChain;
        UINT mActiveSwapchainBufferIndex;
        std::vector<FrameBuffer*> mSwapChainFrameBufferList;

        ID3D12CommandAllocator* mGraphicsCommandAllocator;
        ID3D12GraphicsCommandList* mGraphicsCommandList;
        ID3D12Fence* mGraphicsCompleteFence;
        HANDLE mFenceEvent;

        //ID3D11SamplerState* mSamplerState;
        //ID3D11RasterizerState* mRasterizerState;


        unsigned int mWinWidth;
        unsigned int mWinHeight;
        bool mClose;
};
