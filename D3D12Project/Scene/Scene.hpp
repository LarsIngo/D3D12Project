#pragma once

#include <vector>
#include "../Particle/Particle.hpp"
#include <d3d12.h>

class StorageSwapBuffer;
class ParticleRenderSystem;
class ParticleUpdateSystem;
class DeviceHeapMemory;

class Scene
{
    friend ParticleRenderSystem;
    friend ParticleUpdateSystem;

    public:
        // Constructor.
        Scene(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory, unsigned int maxParticleCount);

        // Destructor.
        ~Scene();

        // Adds partilces to scene.
        // particleList Vector of particles to add.
        void AddParticles(ID3D12GraphicsCommandList* pCommandList, std::vector<Particle>& particleList);

    private:
        unsigned int mMaxParticleCount;
        unsigned int mParticleCount;
        StorageSwapBuffer* mParticleBuffer;
};
