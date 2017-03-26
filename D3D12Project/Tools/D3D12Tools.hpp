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
    void CompileShader(const char* shaderPath, const char* entry, const char* version, D3D12_SHADER_BYTECODE& shader);

    // TransitionState
    void TransitionState(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState);


    inline void D3D12Tools::CompileShader(const char* shaderPath, const char* entry, const char* version, D3D12_SHADER_BYTECODE& shader)
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

    inline void D3D12Tools::TransitionState(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState)
    {
        pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pResource, oldState, newState));
    }
}
