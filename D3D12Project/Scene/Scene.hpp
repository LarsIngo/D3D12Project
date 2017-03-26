#pragma once

#include <vector>
#include "../Particle/Particle.hpp"
#include <d3d12.h>

class StorageSwapBuffer;
class ParticleSystem;
class DeviceHeapMemory;

class Scene
{
    friend ParticleSystem;

    public:
        // Constructor.
        Scene(ID3D12Device* pDevice, unsigned int maxParticleCount);

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
