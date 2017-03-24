#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <map>

class DeviceHeapMemory
{
    public:
        // Constructor.
        DeviceHeapMemory(ID3D12Device* pDevice);

        // Destructor.
        ~DeviceHeapMemory();

        CD3DX12_CPU_DESCRIPTOR_HANDLE GenerateRTV(ID3D12Resource* pResource);

    private:
        struct HeapMemory{
            ID3D12DescriptorHeap* mDescriptorHeap;
            CD3DX12_CPU_DESCRIPTOR_HANDLE mCPUDescriptorHandle;
            std::size_t mIndex;
            std::size_t mMaxCount;
        };
        std::map<D3D12_DESCRIPTOR_HEAP_TYPE, HeapMemory> mHeapMemoryMap;
    
        ID3D12Device* mpDevice;
};
