#include <crtdbg.h>
#include <iostream>

#include "D3D12/D3D12Renderer.hpp"
#include "Tools/CPUTimer.hpp"
#include "Tools/D3D12Timer.hpp"
#include "D3D12/FrameBuffer.hpp"
#include "Particle/ParticleSystem.hpp"
#include "Scene/Scene.hpp"
#include "Camera/Camera.hpp"
#include "Managers/InputManager.hpp"

//#define SYNC_COMPUTE_GRAPHICS

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // +++ INIT +++ //
    unsigned int width = 1920 / 2;
    unsigned int height = 1080 / 2;
    D3D12Renderer renderer(width, height);

    ID3D12Device* device = renderer.mDevice;
    DeviceHeapMemory* deviceHeapMemory = renderer.mDeviceHeapMemory;

    ParticleSystem particleSystem;//(device, deviceContext);
    
    InputManager inputManager(renderer.mGLFWwindow);

    FrameBuffer frameBuffer(device, deviceHeapMemory, width, height, renderer.mBackBufferFormat);
    Camera camera(60.f, &frameBuffer);
    camera.mPosition.z = -5.f;

    int lenX = 1024;
    int lenY = 1024;
    Scene scene; // (device, deviceContext, lenX * lenY);
    {
        std::vector<Particle> particleList;
        Particle particle;
        float spaceing = 1.f;
        float speed = 0.1f;
        particle.scale = glm::vec4(spaceing / 2.f, spaceing / 2.f, 0.f, 0.f);
        for (int y = 0; y < lenY; ++y)
        {
            for (int x = 0; x < lenX; ++x)
            {
                particle.position = glm::vec4(x * spaceing, y * spaceing, 0.f, 0.f);
                particle.velocity = -glm::normalize(particle.position + glm::vec4(speed, speed, 0.f, 0.f));
                particle.color = glm::vec4((float)y / lenY, 0.7f, 1.f - (float)x / lenX, 1.f);
                particleList.push_back(particle);
            }
        }
        scene.AddParticles(particleList);
    }
    // --- INIT --- //

    // +++ MAIN LOOP +++ //
    {
        float dt = 0.f;
        float totalTime = 0.f;
        unsigned int frameCount = 0;
        while (renderer.Running())
        {
            //glm::clamp(dt, 1.f / 6000.f, 1.f / 60.f);
            bool cpuProfile = inputManager.KeyPressed(GLFW_KEY_F1);
            bool gpuProfile = inputManager.KeyPressed(GLFW_KEY_F2);
            {
                CPUTIMER(dt);
                // +++ UPDATE +++ //
                camera.Update(20.f, 2.f, dt, &inputManager);
                //particleSystem.Update(&scene, dt);
                // --- UPDATE --- //

                // +++ RENDER +++ //
                FrameBuffer* backBuffer = renderer.SwapBackBuffer();


                //backBuffer->TransitionImageLayout(graphicsCommandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
                //camera.mpFrameBuffer->Clear(0.2f, 0.2f, 0.2f);
                //particleSystem.Render(&scene, &camera);
                //backBuffer->Copy(camera.mpFrameBuffer);
                //backBuffer->TransitionImageLayout(graphicsCommandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
                // --- RENDER --- //

                // +++ PRESENET +++ //
                renderer.PresentBackBuffer();
                // --- PRESENET --- //
            }
            // +++ PROFILING +++ //
            ++frameCount;
            totalTime += dt;
            if (cpuProfile)
            {
                std::cout << "CPU(Delta time): " << 1000.f * dt << " ms | FPS: " << 1.f / dt << std::endl;
            }
            // --- PROFILING --- //
        }
    }
    // --- MAIN LOOP --- //

    // +++ SHUTDOWN +++ //
    // --- SHUTDOWN --- //

    return 0;
}
