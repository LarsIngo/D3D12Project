#include "D3D12Renderer.hpp"
#include "../Tools/D3D12Tools.hpp"
#include "FrameBuffer.hpp"
#include <d3dx12.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

D3D12Renderer::D3D12Renderer(unsigned int winWidth, unsigned int winHeight)
{
    mWinWidth = winWidth;
    mWinHeight = winHeight;
    mClose = false;

    // Window.
    InitialiseGLFW();

    // D3D12.
    InitialiseD3D12();
}

D3D12Renderer::~D3D12Renderer()
{
    DeInitialiseD3D12();
    DeInitialiseGLFW();
}

bool D3D12Renderer::Running() const
{
    if (mClose)
        return true;

    glfwPollEvents();

    return !glfwWindowShouldClose(mGLFWwindow);
}

void D3D12Renderer::Close()
{
    mClose = true;
}

FrameBuffer* D3D12Renderer::SwapBackBuffer()
{
    mActiveSwapchainBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
    assert(mActiveSwapchainBufferIndex <= mSwapChainFrameBufferList.size());

    return mSwapChainFrameBufferList[mActiveSwapchainBufferIndex];
}


void D3D12Renderer::PresentBackBuffer()
{
    mSwapChain->Present(0, 0);
}

void D3D12Renderer::InitialiseGLFW()
{
    /* Initialize the library */
    if (!glfwInit())
        assert(0 && "GLFWERROR: Initialize the library.");

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    /* Create window */
    mGLFWwindow = glfwCreateWindow(mWinWidth, mWinHeight, "D3D12 window", NULL, NULL);
}

void D3D12Renderer::DeInitialiseGLFW()
{
    glfwTerminate();
}

void D3D12Renderer::InitialiseD3D12()
{
    assert(mGLFWwindow != NULL);

    IDXGIFactory5* dxgiFactory;
    ASSERT(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)), S_OK);

#ifdef BUILD_ENABLE_D3D12_DEBUG
    ID3D12Debug* debugInterface;
    ASSERT(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)), S_OK);
    debugInterface->EnableDebugLayer();
    debugInterface->Release();
#endif

    IDXGIAdapter1* adapter;
    int adapterIndex = 0;
    bool adapterFound = false;
    while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            ++adapterIndex;
            continue;
        }
        HRESULT hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
        if (SUCCEEDED(hr))
        {
            adapterFound = true;
            break;
        }
        ++adapterIndex;
    }
    ASSERT(adapterFound, true);

    ASSERT(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mDevice)), S_OK);


    D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
    commandQueueDesc.Priority = 0;
    commandQueueDesc.NodeMask = 0;
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ASSERT(mDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&mCommandQueue)), S_OK);


    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
    descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    descriptorHeapDesc.NumDescriptors = 10;
    descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    descriptorHeapDesc.NodeMask = 0;
    ASSERT(mDevice->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&mDescriptorHeap)), S_OK);

    mResouceHandle = new CD3DX12_CPU_DESCRIPTOR_HANDLE(mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


    DXGI_MODE_DESC backBufferDesc;
    backBufferDesc.Width = mWinWidth;
    backBufferDesc.Height = mWinHeight;
    backBufferDesc.RefreshRate.Numerator = 0;
    backBufferDesc.RefreshRate.Denominator = 0;
    backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    backBufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    backBufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SAMPLE_DESC sampleDesc;
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    const std::size_t swapChainBufferCount = 3;
    mSwapChainFrameBufferList.resize(swapChainBufferCount);

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swapChainDesc.BufferDesc = backBufferDesc;
    swapChainDesc.SampleDesc = sampleDesc;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = swapChainBufferCount;
    swapChainDesc.OutputWindow = glfwGetWin32Window(mGLFWwindow);
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = 0;

    IDXGISwapChain* tmpSwapChain;
    ASSERT(dxgiFactory->CreateSwapChain(mCommandQueue, &swapChainDesc, &tmpSwapChain), S_OK);
    mSwapChain = static_cast<IDXGISwapChain4*>(tmpSwapChain);

    for (std::size_t i = 0; i < mSwapChainFrameBufferList.size(); ++i)
    {
        ID3D12Resource* resource;
        mSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&resource));
        mSwapChainFrameBufferList[i] = new FrameBuffer(mDevice, mResouceHandle, mWinWidth, mWinHeight, resource);
    }
        

    ASSERT(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mGraphicsCommandAllocator)), S_OK);
    ASSERT(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mGraphicsCommandAllocator, NULL, IID_PPV_ARGS(&mGraphicsCommandList)), S_OK);
    mGraphicsCommandList->Close();

    ASSERT(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mGraphicsCompleteFence)), S_OK);
    mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void D3D12Renderer::DeInitialiseD3D12()
{
    //mSamplerState->Release();
    //mRasterizerState->Release();

    SAFE_RELEASE(mDevice);
    delete mResouceHandle;
    SAFE_RELEASE(mCommandQueue);
    SAFE_RELEASE(mDescriptorHeap);
    SAFE_RELEASE(mSwapChain);
    for (std::size_t i = 0; i < mSwapChainFrameBufferList.size(); ++i) delete mSwapChainFrameBufferList[i];
    SAFE_RELEASE(mGraphicsCommandAllocator);
    SAFE_RELEASE(mGraphicsCommandList);
    SAFE_RELEASE(mGraphicsCompleteFence);
    CloseHandle(mFenceEvent);
}
