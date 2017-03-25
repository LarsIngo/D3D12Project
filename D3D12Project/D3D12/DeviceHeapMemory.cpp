#include "DeviceHeapMemory.hpp"
#include "../Tools/D3D12Tools.hpp"

DeviceHeapMemory::DeviceHeapMemory(ID3D12Device* pDevice)
{
    mpDevice = pDevice;
    
    {   // RTV
        HeapMemory& heapMemory = mHeapMemoryMap[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
        heapMemory.mIndex = 0;
        heapMemory.mMaxCount = 10;
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
        descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        descriptorHeapDesc.NumDescriptors = heapMemory.mMaxCount;
        descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        descriptorHeapDesc.NodeMask = 0;
        ASSERT(mpDevice->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&heapMemory.mDescriptorHeap)), S_OK);
        heapMemory.mCPUDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapMemory.mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    }

    {   // CBV_SRV_UAV
        HeapMemory& heapMemory = mHeapMemoryMap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
        heapMemory.mIndex = 0;
        heapMemory.mMaxCount = 10;
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
        descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        descriptorHeapDesc.NumDescriptors = heapMemory.mMaxCount;
        descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        descriptorHeapDesc.NodeMask = 0;
        ASSERT(mpDevice->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&heapMemory.mDescriptorHeap)), S_OK);
        heapMemory.mCPUDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapMemory.mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    }
    
}

DeviceHeapMemory::~DeviceHeapMemory()
{
    for (auto& it : mHeapMemoryMap)
        SAFE_RELEASE(it.second.mDescriptorHeap);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DeviceHeapMemory::GenerateRTV(ID3D12Resource* pResource)
{
    auto it = mHeapMemoryMap.find(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    assert(it != mHeapMemoryMap.end());
    HeapMemory& heapMemory = it->second;
    assert(heapMemory.mIndex + 1 < heapMemory.mMaxCount);
    CD3DX12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = heapMemory.mCPUDescriptorHandle;
    mpDevice->CreateRenderTargetView(pResource, nullptr, CPUDescriptorHandle);
    heapMemory.mCPUDescriptorHandle.Offset(1, mpDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
    ++heapMemory.mIndex;
    return CPUDescriptorHandle;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DeviceHeapMemory::GenerateCBV_SRV_UAV(ID3D12Resource* pResource)
{
    auto it = mHeapMemoryMap.find(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    assert(it != mHeapMemoryMap.end());
    HeapMemory& heapMemory = it->second;
    assert(heapMemory.mIndex + 1 < heapMemory.mMaxCount);
    CD3DX12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = heapMemory.mCPUDescriptorHandle;
    mpDevice->CreateRenderTargetView(pResource, nullptr, CPUDescriptorHandle);
    heapMemory.mCPUDescriptorHandle.Offset(1, mpDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    ++heapMemory.mIndex;
    return CPUDescriptorHandle;
}
