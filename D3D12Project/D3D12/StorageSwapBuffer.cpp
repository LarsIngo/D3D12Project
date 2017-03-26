#include "StorageSwapBuffer.hpp"

StorageSwapBuffer::StorageSwapBuffer(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory, unsigned int totalSize, unsigned int stride)
{
    for (unsigned int i = 0; i < mBufferCount; ++i)
        mBuffers[i] = new StorageBuffer(pDevice, pDeviceHeapMemory, totalSize, stride);
}

StorageSwapBuffer::~StorageSwapBuffer()
{
    for (unsigned int i = 0; i < mBufferCount; ++i)
        delete mBuffers[i];
}

StorageBuffer* StorageSwapBuffer::GetOutputBuffer()
{
    return mBuffers[mBufferIndex];
}

StorageBuffer* StorageSwapBuffer::GetInputBuffer()
{
    return mBuffers[(mBufferIndex + 1) % mBufferCount];
}

void StorageSwapBuffer::Swap()
{
    mBufferIndex = (mBufferIndex + 1) % mBufferCount;
}
