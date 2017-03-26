#pragma once

#include "StorageBuffer.hpp"

class StorageSwapBuffer
{
    public:
        // Constructor.
        StorageSwapBuffer(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory, unsigned int totalSize, unsigned int stride);

        // Destructor.
        ~StorageSwapBuffer();

        // Get write storage buffer.
        // Returns output storage buffer.
        StorageBuffer* GetOutputBuffer();

        // Get read storage buffer.
        // Returns input storage buffer.
        StorageBuffer* GetInputBuffer();

        // Swaps read and write storage buffer. 
        void Swap();

    private:
        // Storage buffer count.
        static const unsigned int mBufferCount = 2;
        // Storeage buffers.
        StorageBuffer* mBuffers[mBufferCount];
        // Buffer index.
        unsigned int mBufferIndex = 0;
};
