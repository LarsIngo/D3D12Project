#pragma once

#include <d3d12.h>
#include <d3dx12.h>

class DeviceHeapMemory;

class StorageBuffer
{
    public:
        // Constructor.
        // pDevice Pointer to D3D11 device.
        StorageBuffer(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory, unsigned int totalSize, unsigned int stride);

        // Destructor.
        ~StorageBuffer();

        // Copy other storage buffer.
        void Copy(ID3D12GraphicsCommandList* pCommandList, StorageBuffer* storageBuffer);

        // Get size of storage buffer.
        // Returns size in bytes.
        unsigned int GetSize();

        // Get stride of storage buffer.
        // Returns stride of each element in bytes.
        unsigned int GetStride();
        
        // Write to storage buffer.
        // data Data to write.
        // byteSize Size of data in bytes.
        // off Offset to write data in bytes.
        void Write(ID3D12GraphicsCommandList* pCommandList, void* data, unsigned int byteSize, unsigned int offset);

        // Transition state.
        // commandList Command buffer to make transition
        // newState State to transition to.
        void TransitionState(ID3D12GraphicsCommandList* pCommandList, D3D12_RESOURCE_STATES newState);

        // Buffer.
        D3D12_RESOURCE_STATES mState;
        ID3D12Resource* mBuff;
        CD3DX12_CPU_DESCRIPTOR_HANDLE mSRV;
        CD3DX12_CPU_DESCRIPTOR_HANDLE mUAV;

    private:
        ID3D12Device* mpDevice;
        DeviceHeapMemory* mpDeviceHeapMemory;

        // Staging buffer.
        ID3D12Resource* mStagingBuff;

        // Storage buffer stride of each element in bytes.
        unsigned int mStride;

        // Storage buffer size in bytes.
        unsigned int mSize;
};
