#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <glm/glm.hpp>

class Scene;
class StorageBuffer;
class FrameBuffer;
class Camera;
class DeviceHeapMemory;

class ParticleUpdateSystem
{
    public:
        // Constructor.
        ParticleUpdateSystem(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory, DXGI_FORMAT format, unsigned int width, unsigned int height);

        // Destructor.
        ~ParticleUpdateSystem();

        // Render particles.
        void Update(ID3D12GraphicsCommandList* pCommandList, Scene* scene, float dt);

        // Pipeline state object.
        ID3D12PipelineState* mPipeline;

    private:
        ID3D12Device* mpDevice;
        DeviceHeapMemory* mpDeviceHeapMemory;

        DXGI_FORMAT mFormat;
        unsigned int mWidth;
        unsigned int mHeight;

        ID3D12RootSignature* mRootSignature;

        D3D12_VIEWPORT mViewport;
        D3D12_RECT mScissorRect;

        struct MetaData
        {
            float dt;
            unsigned int particleCount;
            float pad[6];
        } mMetaData;
        StorageBuffer* mMetaBuffer;
};
