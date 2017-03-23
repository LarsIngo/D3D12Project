#pragma once

#include <d3d12.h>

class StorageBuffer
{
    public:
        // Constructor.
        // pDevice Pointer to D3D11 device.
        // pDeviceContext Pointer to D3D11 device context.
        // totalSize Total size in bytes.
        // stride Stride of each element in bytes.
        // bindFlags Bind flags. DEFAULT [D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS]
        StorageBuffer(/*ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, unsigned int totalSize, unsigned int stride, UINT bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS*/);

        // Destructor.
        ~StorageBuffer();

        // Copy other storage buffer.
        void Copy(StorageBuffer* storageBuffer);

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
        void Write(void* data, unsigned int byteSize, unsigned int offset);

        // Buffer.
        //ID3D11Buffer* mBuff;
        //ID3D11ShaderResourceView* mSRV;
        //ID3D11UnorderedAccessView* mUAV;

    private:
        //ID3D11Device* mpDevice;
        //ID3D11DeviceContext* mpDeviceContext;

        // Staging buffer.
        //ID3D11Buffer* mStagingBuff;

        // Storage buffer stride of each element in bytes.
        unsigned int mStride;

        // Storage buffer size in bytes.
        unsigned int mSize;
};
