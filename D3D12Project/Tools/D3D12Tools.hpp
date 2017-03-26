#pragma once

#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <string>
#include <vector>

#ifdef NDEBUG
#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#define ASSERT(x, y) if (x != y) { MessageBox(NULL, _T("ASSERT"), _T("ERROR"), MB_OK); }
#else
#define ASSERT(x, y) x
#endif // WIN32
#else
#include <assert.h>
#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#define ASSERT(x, y) if (x != y) { MessageBox(NULL, _T("ASSERT"), _T("ERROR"), MB_OK); assert(x == y); }
#else
#define DxAssert(x, y) assert(x == y)
#endif // WIN32
#endif // NDEBUG

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = NULL; } }

namespace D3D12Tools
{
    // Comlile shader.
    inline void CompileShader(const char* shaderPath, const char* entry, const char* version, D3D12_SHADER_BYTECODE& shader)
    {
        std::string s(shaderPath);
        std::wstring path(s.begin(), s.end());
        ID3DBlob* shaderBlob;
        ID3DBlob* errorBuff;
        HRESULT hr = D3DCompileFromFile(
            path.c_str(),
            nullptr,
            nullptr,
            entry,
            version,
            0,
            0,
            &shaderBlob,
            &errorBuff);
        if (FAILED(hr))
        {
            OutputDebugStringA((char*)errorBuff->GetBufferPointer());
            assert(0 && "Vertex shader compile error");
        }
        shader.BytecodeLength = shaderBlob->GetBufferSize();
        shader.pShaderBytecode = shaderBlob->GetBufferPointer();
    }

    // TransitionState
    inline void TransitionState(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState)
    {
        pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pResource, oldState, newState));
    }

    // Create command allocator.
    inline ID3D12CommandAllocator* CreateCommandAllocator(ID3D12Device* pDevice)
    {
        ID3D12CommandAllocator* commandAllocator;
        ASSERT(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)), S_OK);
        return commandAllocator;
    }

    // Create graphics command list.
    inline ID3D12GraphicsCommandList* CreateGraphicsCommandList(ID3D12Device* pDevice, ID3D12CommandAllocator* pCommandAllocator)
    {
        ID3D12GraphicsCommandList* commandList;
        ASSERT(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, NULL, IID_PPV_ARGS(&commandList)), S_OK);
        return commandList;
    }

    // Create graphics command list.
    inline ID3D12GraphicsCommandList* CreateGraphicsCommandList(ID3D12Device* pDevice, ID3D12CommandAllocator* pCommandAllocator)
    {
        ID3D12GraphicsCommandList* commandList;
        ASSERT(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, NULL, IID_PPV_ARGS(&commandList)), S_OK);
        return commandList;
    }

    // Create fence.
    inline ID3D12Fence* CreateFence(ID3D12Device* pDevice)
    {
        ID3D12Fence* fence;
        ASSERT(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), S_OK);
        return fence;
    }

    // Close command list.
    inline void CloseCommandList(ID3D12GraphicsCommandList* pCommandList)
    {
        ASSERT(pCommandList->Close(), S_OK);
    }

    // Execute command list.
    inline void ExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, ID3D12CommandList* commandList)
    {
        ID3D12CommandList* ppCommandLists[] = { commandList  };
        pCommandQueue->ExecuteCommandLists(1, ppCommandLists);
    }

    // Wait for fence.
    inline void WaitFence(ID3D12Fence* fence, UINT64 waitValue, HANDLE& syncEvent)
    {
        if (fence->GetCompletedValue() < waitValue)
        {
            ASSERT(fence->SetEventOnCompletion(waitValue, syncEvent), S_OK);
            WaitForSingleObject(syncEvent, INFINITE);
        }
    }

    // Reset command list and command allocator.
    inline void ResetGraphicsCommandList(ID3D12CommandAllocator* pCommandAllocator, ID3D12GraphicsCommandList* pGraphicsCommandList)
    {
        ASSERT(pCommandAllocator->Reset(), S_OK);
        ASSERT(pGraphicsCommandList->Reset(pCommandAllocator, nullptr), S_OK);
    }

}
