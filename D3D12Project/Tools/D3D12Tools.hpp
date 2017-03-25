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
    // Create Resource
    void CreateResource(ID3D12Device* pDevice, const D3D12_RESOURCE_DESC& resouceDesc, D3D12_HEAP_TYPE heapType, ID3D12Resource** ppResource);

    // TransitionState
    void TransitionState(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState);



    inline void D3D12Tools::CreateResource(ID3D12Device* pDevice, const D3D12_RESOURCE_DESC& resouceDesc, D3D12_HEAP_TYPE heapType, ID3D12Resource** ppResource)
    {
        ASSERT(pDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(heapType), 
            D3D12_HEAP_FLAG_NONE,
            &resouceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(ppResource)), S_OK);
    }

    inline void D3D12Tools::TransitionState(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState)
    {
        pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pResource, oldState, newState));
    }
}
