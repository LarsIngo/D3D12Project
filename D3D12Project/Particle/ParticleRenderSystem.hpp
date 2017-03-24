#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <glm/glm.hpp>

class Scene;
class StorageBuffer;
class FrameBuffer;
class Camera;

class ParticleRenderSystem
{
    public:
        // Constructor.
        ParticleRenderSystem(ID3D12Device* pDevice, DXGI_FORMAT format, unsigned int width, unsigned int height);

        // Destructor.
        ~ParticleRenderSystem();

        // Render particles.
        // pCommandList Command list to update.
        // scene Scene to update.
        // camera Camera to render from.
        void Render(ID3D12GraphicsCommandList* pCommandList, Scene* scene, Camera* camera);

        ID3D12PipelineState* mPipeline;

    private:
        ID3D12Device* mpDevice;

        DXGI_FORMAT mFormat;
        unsigned int mWidth;
        unsigned int mHeight;

        //VkShaderModule mComputeShaderModule;

        //VkDescriptorPool mPipelineDescriptorPool;
        //VkDescriptorSet mPipelineDescriptorSet;
        //VkDescriptorSetLayout mPipelineDescriptorSetLayout;
        //VkPipelineLayout mPipelineLayout;
        //VkPipeline mPipeline;

        ID3D12RootSignature* mRootSignature;

        D3D12_VIEWPORT mViewport;
        D3D12_RECT mScissorRect;

        struct MetaData
        {
            glm::mat4 vpMatrix;
            glm::vec4 lensPosition;
            glm::vec4 lensUpDirection;
        } mMetaData;
        //VkBuffer mMetaDataBuffer;
        //VkDeviceMemory mMetaDataBufferMemory;
};
