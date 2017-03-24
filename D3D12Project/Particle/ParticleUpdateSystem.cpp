//#include "ParticleUpdateSystem.hpp"
//#include "VkRenderer.hpp"
//#include "Scene.hpp"
//#include "FrameBuffer.hpp"
//#include "StorageSwapBuffer.hpp"
//#include "Camera.hpp"
//#include <glm/gtc/matrix_transform.hpp>
//#include "vkTools.hpp"
//
//ParticleUpdateSystem::ParticleUpdateSystem(VkDevice device, VkPhysicalDevice physicalDevice)
//{
//    mDevice = device;
//    mPhysicalDevice = physicalDevice;
//
//    // Create meta buffers.
//    uint32_t minOffsetAligment;
//    vkTools::CreateBuffer(mDevice, mPhysicalDevice, sizeof(MetaData),
//        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//        mMetaDataBuffer, mMetaDataBufferMemory, minOffsetAligment
//        );
//    assert(sizeof(MetaData) % minOffsetAligment == 0);
//
//    // Create compute pipeline.
//    {
//        vkTools::CreateShaderModule(mDevice, "resources/shaders/Particles_Update_CS.spv", mComputeShaderModule);
//
//        VkDescriptorSetLayoutBinding particleInBufferSetLayoutBinding;
//        particleInBufferSetLayoutBinding.descriptorCount = 1;
//        particleInBufferSetLayoutBinding.pImmutableSamplers = nullptr;
//        particleInBufferSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
//        particleInBufferSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//        particleInBufferSetLayoutBinding.binding = 0;
//        VkDescriptorSetLayoutBinding particleOutBufferSetLayoutBinding;
//        particleOutBufferSetLayoutBinding.descriptorCount = 1;
//        particleOutBufferSetLayoutBinding.pImmutableSamplers = nullptr;
//        particleOutBufferSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
//        particleOutBufferSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//        particleOutBufferSetLayoutBinding.binding = 1;
//        VkDescriptorSetLayoutBinding metaBufferSetLayoutBinding;
//        metaBufferSetLayoutBinding.descriptorCount = 1;
//        metaBufferSetLayoutBinding.pImmutableSamplers = nullptr;
//        metaBufferSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
//        metaBufferSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//        metaBufferSetLayoutBinding.binding = 2;
//        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindingList{ particleInBufferSetLayoutBinding, particleOutBufferSetLayoutBinding, metaBufferSetLayoutBinding };
//
//        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
//        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//        descriptorSetLayoutCreateInfo.pNext = 0;
//        descriptorSetLayoutCreateInfo.flags = 0;
//        descriptorSetLayoutCreateInfo.bindingCount = descriptorSetLayoutBindingList.size();
//        descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindingList.data();
//        vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mPipelineDescriptorSetLayout);
//
//        std::vector<VkDescriptorSetLayout> descriptorSetLayoutList{ mPipelineDescriptorSetLayout };
//        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
//        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//        pipelineLayoutCreateInfo.pNext = NULL;
//        pipelineLayoutCreateInfo.flags = 0;
//        pipelineLayoutCreateInfo.setLayoutCount = descriptorSetLayoutList.size();
//        pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayoutList.data();
//        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
//        pipelineLayoutCreateInfo.pPushConstantRanges = NULL;
//        vkTools::VkErrorCheck(vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout));
//
//        VkDescriptorPoolSize descriptorPoolSize;
//        descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//        descriptorPoolSize.descriptorCount = descriptorSetLayoutBindingList.size();
//        std::vector<VkDescriptorPoolSize> descriptorPoolSizeList{ descriptorPoolSize };
//
//        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
//        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//        descriptorPoolCreateInfo.pNext = NULL;
//        descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
//        descriptorPoolCreateInfo.maxSets = 1;
//        descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizeList.data();
//        descriptorPoolCreateInfo.poolSizeCount = descriptorPoolSizeList.size();
//        vkTools::VkErrorCheck(vkCreateDescriptorPool(mDevice, &descriptorPoolCreateInfo, nullptr, &mPipelineDescriptorPool));
//
//        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
//        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//        descriptorSetAllocateInfo.pNext = NULL;
//        descriptorSetAllocateInfo.descriptorPool = mPipelineDescriptorPool;
//        descriptorSetAllocateInfo.pSetLayouts = &mPipelineDescriptorSetLayout;
//        descriptorSetAllocateInfo.descriptorSetCount = 1;
//        vkTools::VkErrorCheck(vkAllocateDescriptorSets(mDevice, &descriptorSetAllocateInfo, &mPipelineDescriptorSet));
//
//        VkComputePipelineCreateInfo computePipelineCreateInfo;
//        computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
//        computePipelineCreateInfo.pNext = NULL;
//        computePipelineCreateInfo.flags = 0;
//        computePipelineCreateInfo.stage = vkTools::CreatePipelineShaderStageCreateInfo(mDevice, mComputeShaderModule, VK_SHADER_STAGE_COMPUTE_BIT, "main");;
//        computePipelineCreateInfo.layout = mPipelineLayout;
//        computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
//        computePipelineCreateInfo.basePipelineIndex = NULL;
//        vkTools::VkErrorCheck(vkCreateComputePipelines(mDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &mPipeline));
//    }
//}
//
//ParticleUpdateSystem::~ParticleUpdateSystem()
//{
//    vkFreeMemory(mDevice, mMetaDataBufferMemory, nullptr);
//    vkDestroyBuffer(mDevice, mMetaDataBuffer, nullptr);
//
//    vkDestroyShaderModule(mDevice, mComputeShaderModule, nullptr);
//
//    vkDestroyPipeline(mDevice, mPipeline, nullptr);
//    vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
//    vkDestroyDescriptorSetLayout(mDevice, mPipelineDescriptorSetLayout, nullptr);
//    vkFreeDescriptorSets(mDevice, mPipelineDescriptorPool, 1, &mPipelineDescriptorSet);
//    vkDestroyDescriptorPool(mDevice, mPipelineDescriptorPool, nullptr);
//}
//
//void ParticleUpdateSystem::Update(VkCommandBuffer commandBuffer, Scene* scene, float dt)
//{
//    mMetaData.dt = dt;
//    mMetaData.particleCount = scene->mParticleCount;
//    vkTools::WriteBuffer(commandBuffer, mDevice, mMetaDataBufferMemory, &mMetaData, sizeof(MetaData), 0);
//
//    {   // vkUpdateDescriptorSets.
//        VkDescriptorBufferInfo particleInBufferInputDescriptorBufferInfo;
//        VkWriteDescriptorSet particleInBufferInputWriteDescriptorSet;
//        particleInBufferInputDescriptorBufferInfo.buffer = scene->mParticleBuffer->GetInputBuffer()->mBuffer;
//        particleInBufferInputDescriptorBufferInfo.offset = 0;
//        particleInBufferInputDescriptorBufferInfo.range = scene->mParticleBuffer->GetInputBuffer()->GetSize();
//        particleInBufferInputWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        particleInBufferInputWriteDescriptorSet.pNext = NULL;
//        particleInBufferInputWriteDescriptorSet.dstSet = mPipelineDescriptorSet;
//        particleInBufferInputWriteDescriptorSet.dstArrayElement = 0;
//        particleInBufferInputWriteDescriptorSet.descriptorCount = 1;
//        particleInBufferInputWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//        particleInBufferInputWriteDescriptorSet.pImageInfo = NULL;
//        particleInBufferInputWriteDescriptorSet.dstBinding = 0;
//        particleInBufferInputWriteDescriptorSet.pBufferInfo = &particleInBufferInputDescriptorBufferInfo;
//
//        VkDescriptorBufferInfo particleOutBufferInputDescriptorBufferInfo;
//        VkWriteDescriptorSet particleOutBufferInputWriteDescriptorSet;
//        particleOutBufferInputDescriptorBufferInfo.buffer = scene->mParticleBuffer->GetOutputBuffer()->mBuffer;
//        particleOutBufferInputDescriptorBufferInfo.offset = 0;
//        particleOutBufferInputDescriptorBufferInfo.range = scene->mParticleBuffer->GetOutputBuffer()->GetSize();
//        particleOutBufferInputWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        particleOutBufferInputWriteDescriptorSet.pNext = NULL;
//        particleOutBufferInputWriteDescriptorSet.dstSet = mPipelineDescriptorSet;
//        particleOutBufferInputWriteDescriptorSet.dstArrayElement = 0;
//        particleOutBufferInputWriteDescriptorSet.descriptorCount = 1;
//        particleOutBufferInputWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//        particleOutBufferInputWriteDescriptorSet.pImageInfo = NULL;
//        particleOutBufferInputWriteDescriptorSet.dstBinding = 1;
//        particleOutBufferInputWriteDescriptorSet.pBufferInfo = &particleOutBufferInputDescriptorBufferInfo;
//
//        VkDescriptorBufferInfo metaBufferInputDescriptorBufferInfo;
//        VkWriteDescriptorSet metaBufferInputWriteDescriptorSet;
//        metaBufferInputDescriptorBufferInfo.buffer = mMetaDataBuffer;
//        metaBufferInputDescriptorBufferInfo.offset = 0;
//        metaBufferInputDescriptorBufferInfo.range = sizeof(MetaData);
//        metaBufferInputWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        metaBufferInputWriteDescriptorSet.pNext = NULL;
//        metaBufferInputWriteDescriptorSet.dstSet = mPipelineDescriptorSet;
//        metaBufferInputWriteDescriptorSet.dstArrayElement = 0;
//        metaBufferInputWriteDescriptorSet.descriptorCount = 1;
//        metaBufferInputWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//        metaBufferInputWriteDescriptorSet.pImageInfo = NULL;
//        metaBufferInputWriteDescriptorSet.dstBinding = 2;
//        metaBufferInputWriteDescriptorSet.pBufferInfo = &metaBufferInputDescriptorBufferInfo;
//
//        std::vector<VkWriteDescriptorSet> writeDescriptorSetList{ particleInBufferInputWriteDescriptorSet, particleOutBufferInputWriteDescriptorSet, metaBufferInputWriteDescriptorSet };
//        vkUpdateDescriptorSets(mDevice, writeDescriptorSetList.size(), writeDescriptorSetList.data(), 0, NULL);
//    }
//    
//    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mPipeline);
//    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mPipelineLayout, 0, 1, &mPipelineDescriptorSet, 0, NULL);
//    vkCmdDispatch(commandBuffer, scene->mParticleCount / 256 + 1, 1, 1);
//}
