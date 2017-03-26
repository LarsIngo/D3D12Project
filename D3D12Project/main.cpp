#include <crtdbg.h>
#include <iostream>

#include "D3D12/D3D12Renderer.hpp"
#include "Tools/CPUTimer.hpp"
#include "Tools/D3D12Timer.hpp"
#include "Tools/D3D12Tools.hpp"
#include "D3D12/FrameBuffer.hpp"
#include "Particle/ParticleRenderSystem.hpp"
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

    ID3D12GraphicsCommandList* graphicsCommandList;
    ASSERT(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, renderer.mGraphicsCommandAllocator, NULL, IID_PPV_ARGS(&graphicsCommandList)), S_OK);
    ASSERT(graphicsCommandList->Close(), S_OK);

    ParticleRenderSystem particleRenderSystem(device, renderer.mBackBufferFormat, width, height);
    
    InputManager inputManager(renderer.mGLFWwindow);

    FrameBuffer frameBuffer(device, width, height, renderer.mBackBufferFormat);
    Camera camera(60.f, &frameBuffer);
    camera.mPosition.z = -5.f;

    int lenX = 2;
    int lenY = 2;
    Scene scene(device, lenX * lenY);
    {
        ID3D12GraphicsCommandList* uploadCommandList;
        ASSERT(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, renderer.mGraphicsCommandAllocator, NULL, IID_PPV_ARGS(&uploadCommandList)), S_OK);

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
        scene.AddParticles(uploadCommandList, particleList);

        ASSERT(uploadCommandList->Close(), S_OK);
        ID3D12CommandList* ppCommandLists[] = { uploadCommandList };
        renderer.mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
        renderer.mCommandQueue->Signal(renderer.mUploadCompleteFence, 1);
        if (renderer.mUploadCompleteFence->GetCompletedValue() < 1)
        {
            ASSERT(renderer.mUploadCompleteFence->SetEventOnCompletion(1, renderer.mSyncEvent), S_OK);
            WaitForSingleObject(renderer.mSyncEvent, INFINITE);
        }
        ASSERT(renderer.mUploadCommandAllocator->Reset(), S_OK);
        ASSERT(uploadCommandList->Reset(renderer.mUploadCommandAllocator, nullptr), S_OK);
        uploadCommandList->Release();
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
                if (renderer.mGraphicsCompleteFence->GetCompletedValue() < renderer.mFrameID)
                {
                    ASSERT(renderer.mGraphicsCompleteFence->SetEventOnCompletion(renderer.mFrameID, renderer.mSyncEvent), S_OK);
                    WaitForSingleObject(renderer.mSyncEvent, INFINITE);
                }
                ASSERT(renderer.mGraphicsCommandAllocator->Reset(), S_OK);
                ASSERT(graphicsCommandList->Reset(renderer.mGraphicsCommandAllocator, nullptr), S_OK);
                
                FrameBuffer* backBuffer = renderer.SwapBackBuffer();

                backBuffer->TransitionState(graphicsCommandList, D3D12_RESOURCE_STATE_COPY_DEST);
                camera.mpFrameBuffer->Clear(graphicsCommandList, 0.2f, 0.2f, 0.2f, 0.f);
                particleRenderSystem.Render(graphicsCommandList, &scene, &camera);
                backBuffer->Copy(graphicsCommandList, camera.mpFrameBuffer);
                backBuffer->TransitionState(graphicsCommandList, D3D12_RESOURCE_STATE_PRESENT);

                ASSERT(graphicsCommandList->Close(), S_OK);
                ID3D12CommandList* ppGraphicsCommandLists[] = { graphicsCommandList };
                renderer.mCommandQueue->ExecuteCommandLists(_countof(ppGraphicsCommandLists), ppGraphicsCommandLists);
                renderer.mCommandQueue->Signal(renderer.mGraphicsCompleteFence, renderer.mFrameID + 1);
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
    graphicsCommandList->Release();
    // --- SHUTDOWN --- //

    return 0;
}
