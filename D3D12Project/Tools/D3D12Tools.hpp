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
    void CompileShader(const char* shaderPath, const char* entry, const char* version, D3D12_SHADER_BYTECODE& shader)
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
    void TransitionState(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState)
    {
        pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pResource, oldState, newState));
    }

    // Create graphics command List;
    ID3D12GraphicsCommandList* CreateGraphicsCommandList(ID3D12Device* pDevice, ID3D12CommandAllocator* pCommandAllocator)
    {
        ID3D12GraphicsCommandList* commandList;
        ASSERT(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, NULL, IID_PPV_ARGS(&commandList)), S_OK);
        return commandList;
    }

    // Execute Command Lists
    void ExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, std::vector<ID3D12CommandList*>& commandLists)
    {
        pCommandQueue->ExecuteCommandLists(commandLists.size(), commandLists.data());
    }

    // Wait for fence;
    void WaitFence(ID3D12Fence* fence, UINT64 waitValue, HANDLE& syncEvent)
    {
        if (fence->GetCompletedValue() < waitValue)
        {
            ASSERT(fence->SetEventOnCompletion(waitValue, syncEvent), S_OK);
            WaitForSingleObject(syncEvent, INFINITE);
        }
    }

    // Reset command list and command allocator.
    void ResetGraphicsCommandList(ID3D12CommandAllocator* pCommandAllocator, ID3D12GraphicsCommandList* pGraphicsCommandList)
    {
        ASSERT(pCommandAllocator->Reset(), S_OK);
        ASSERT(pGraphicsCommandList->Reset(pCommandAllocator, nullptr), S_OK);
    }

}
