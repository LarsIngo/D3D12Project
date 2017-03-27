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
        ParticleUpdateSystem(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory);

        // Destructor.
        ~ParticleUpdateSystem();

        // Render particles.
        void Update(ID3D12GraphicsCommandList* pCommandList, Scene* scene, float dt);

        // Pipeline state object.
        ID3D12PipelineState* mPipeline;

    private:
        ID3D12Device* mpDevice;
        DeviceHeapMemory* mpDeviceHeapMemory;

        ID3D12RootSignature* mRootSignature;

        struct MetaData
        {
            float dt;
            unsigned int particleCount;
            float pad[6];
        } mMetaData;
        StorageBuffer* mMetaBuffer;
};
