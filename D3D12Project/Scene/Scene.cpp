#include "Scene.hpp"
#include "../D3D12/StorageSwapBuffer.hpp"

Scene::Scene(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory, unsigned int maxParticleCount)
{
    mParticleCount = 0;
    mMaxParticleCount = maxParticleCount;

    mParticleBuffer = new StorageSwapBuffer(pDevice, pDeviceHeapMemory, sizeof(Particle) * mMaxParticleCount, sizeof(Particle));
}

Scene::~Scene()
{
    delete mParticleBuffer;
}

void Scene::AddParticles(ID3D12GraphicsCommandList* pCommandList, std::vector<Particle>& particleList)
{
    unsigned int offset = mParticleCount * sizeof(Particle);
    unsigned int particleCount = (unsigned int)particleList.size();
    unsigned int bytes = particleCount * sizeof(Particle);

    mParticleBuffer->GetInputBuffer()->Write(pCommandList, particleList.data(), bytes, offset);

    mParticleCount += particleCount;
}
