#pragma once

#ifdef _DEBUG
#define BUILD_ENABLE_D3D11_DEBUG
#endif

#include <GLFW/glfw3.h>
#include <d3d12.h>
#include <dxgi1_5.h>
#include <d3dcompiler.h>

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

        // Present.
        // frameBuffer Frame buffer to present to window.
        void Present(FrameBuffer* frameBuffer);

        // D3D12 device.
        ID3D12Device* mDevice;

        // GLFW window.
        GLFWwindow* mGLFWwindow;
    private:
        void InitialiseGLFW();
        void DeInitialiseGLFW();

        void InitialiseD3D12();
        void DeInitialiseD3D12();

        unsigned int mWinWidth;
        unsigned int mWinHeight;
        bool mClose;

        //D3D12
        //IDXGISwapChain* mSwapChain;
        FrameBuffer* mWinFrameBuffer;
        //ID3D11SamplerState* mSamplerState;
        //ID3D11RasterizerState* mRasterizerState;
};
