#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <glm/glm.hpp>

class Scene;
class StorageBuffer;
class FrameBuffer;

class ParticleUpdateSystem
{
    public:
        // Constructor.
        ParticleUpdateSystem(ID3D12Device* pDevice);

        // Destructor.
        ~ParticleUpdateSystem();

        // Update particles.
        // pCommandList Command list to update.
        // scene Scene to update.
        // dt Delta time.
        void Update(ID3D12CommandList* pCommandList, Scene* scene, float dt);

    private:
        ID3D12Device* mpDevice;

        //VkShaderModule mComputeShaderModule;

        //VkDescriptorPool mPipelineDescriptorPool;
        //VkDescriptorSet mPipelineDescriptorSet;
        //VkDescriptorSetLayout mPipelineDescriptorSetLayout;
        //VkPipelineLayout mPipelineLayout;
        //VkPipeline mPipeline;

        ID3D12PipelineState* mPipeline;
        
        struct MetaData
        {
            float dt;
            unsigned int particleCount;
            float pad[6];
        } mMetaData;
        //VkBuffer mMetaDataBuffer;
        //VkDeviceMemory mMetaDataBufferMemory;
};
