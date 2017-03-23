#pragma once

#include <vector>
#include "../Particle/Particle.hpp"
#include <d3d12.h>

class StorageSwapBuffer;
class ParticleSystem;

class Scene
{
    friend ParticleSystem;

    public:
        // Constructor.
        // pDevice Pointer to D3D11 device.
        // pDeviceContext Pointer to D3D11 device context.
        // maxParticleCount Max number of particles in scene.
        Scene(/*ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, unsigned int maxParticleCount*/);

        // Destructor.
        ~Scene();

        // Adds partilces to scene.
        // particleList Vector of particles to add.
        void AddParticles(std::vector<Particle>& particleList);

    private:
        unsigned int mMaxParticleCount;
        unsigned int mParticleCount;
        StorageSwapBuffer* mParticleBuffer;

        //ID3D11Device* mpDevice;
        //ID3D11DeviceContext* mpDeviceContext;
};
