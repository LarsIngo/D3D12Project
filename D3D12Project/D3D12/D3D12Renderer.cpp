#include "D3D12Renderer.hpp"
#include "DeviceHeapMemory.hpp"
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
    mFrameID = 0;

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

    UINT64 fenceCompletedValue = mPresentCompleteFence->GetCompletedValue();
    if (fenceCompletedValue < mFrameID)
    {
        ASSERT(mPresentCompleteFence->SetEventOnCompletion(mFrameID, mSyncEvent), S_OK);

        WaitForSingleObject(mSyncEvent, INFINITE);
    }

    assert(mActiveSwapchainBufferIndex <= mSwapChainFrameBufferList.size());

    return mSwapChainFrameBufferList[mActiveSwapchainBufferIndex];
}


void D3D12Renderer::PresentBackBuffer()
{
    mCommandQueue->Signal(mPresentCompleteFence, mFrameID + 1);

    mSwapChain->Present(0, 0);

    ++mFrameID;
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
            adapter->Release();
            ++adapterIndex;
            continue;
        }
        HRESULT hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
        if (SUCCEEDED(hr))
        {
            adapterFound = true;
            break;
        }
        ++adapterIndex;
    }
    ASSERT(adapterFound, true);

    ASSERT(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice)), S_OK);
    adapter->Release();

    
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
    commandQueueDesc.Priority = 0;
    commandQueueDesc.NodeMask = 0;
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ASSERT(mDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&mCommandQueue)), S_OK);

    mDeviceHeapMemory = new DeviceHeapMemory(mDevice);
    
    mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_MODE_DESC backBufferDesc;
    backBufferDesc.Width = mWinWidth;
    backBufferDesc.Height = mWinHeight;
    backBufferDesc.RefreshRate.Numerator = 0;
    backBufferDesc.RefreshRate.Denominator = 0;
    backBufferDesc.Format = mBackBufferFormat;
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
        mSwapChainFrameBufferList[i] = new FrameBuffer(mDevice, mDeviceHeapMemory, mWinWidth, mWinHeight, mBackBufferFormat, resource);
    }
        
    ASSERT(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mGraphicsCommandAllocator)), S_OK);

    
    ASSERT(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mGraphicsCompleteFence)), S_OK);
    ASSERT(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mPresentCompleteFence)), S_OK);

    mSyncEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    assert(mSyncEvent != nullptr);

    dxgiFactory->Release();
}

void D3D12Renderer::DeInitialiseD3D12()
{
    CloseHandle(mSyncEvent);
    SAFE_RELEASE(mPresentCompleteFence);
    SAFE_RELEASE(mGraphicsCompleteFence);
    SAFE_RELEASE(mGraphicsCommandAllocator);
    for (std::size_t i = 0; i < mSwapChainFrameBufferList.size(); ++i)
        delete mSwapChainFrameBufferList[i];
    SAFE_RELEASE(mSwapChain);
    delete mDeviceHeapMemory;
    SAFE_RELEASE(mCommandQueue);
    SAFE_RELEASE(mDevice);
}
