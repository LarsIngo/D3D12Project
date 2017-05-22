#include "Scene.hpp"
#include "../D3D12/StorageSwapBuffer.hpp"

Scene::Scene(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory, unsigned int maxParticleCount)
{
    mParticleCount = 0;
    mMaxParticleCount = maxParticleCount;

    mParticleUpdateBuffer = new StorageSwapBuffer(pDevice, pDeviceHeapMemory, sizeof(Particle) * mMaxParticleCount, sizeof(Particle));
    mParticleRenderBuffer = new StorageBuffer(pDevice, pDeviceHeapMemory, sizeof(Particle) * mMaxParticleCount, sizeof(Particle));
}

Scene::~Scene()
{
    delete mParticleUpdateBuffer;
    delete mParticleRenderBuffer;
}

void Scene::AddParticles(ID3D12GraphicsCommandList* pCommandList, std::vector<Particle>& particleList)
{
    unsigned int offset = mParticleCount * sizeof(Particle);
    unsigned int particleCount = (unsigned int)particleList.size();
    unsigned int bytes = particleCount * sizeof(Particle);

    mParticleUpdateBuffer->GetInputBuffer()->Write(pCommandList, particleList.data(), bytes, offset);
    mParticleUpdateBuffer->GetOutputBuffer()->Write(pCommandList, particleList.data(), bytes, offset);

    mParticleUpdateBuffer->GetInputBuffer()->TransitionState(pCommandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    mParticleUpdateBuffer->GetOutputBuffer()->TransitionState(pCommandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    mParticleCount += particleCount;
}
