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

        ID3D12DescriptorHeap* GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);

        CD3DX12_CPU_DESCRIPTOR_HANDLE GenerateRTV(D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc, ID3D12Resource* pResource);

        CD3DX12_CPU_DESCRIPTOR_HANDLE GenerateSRV(D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, ID3D12Resource* pResource);

        CD3DX12_CPU_DESCRIPTOR_HANDLE GenerateUAV(D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc, ID3D12Resource* pResource);

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
