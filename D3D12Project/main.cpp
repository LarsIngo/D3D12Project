#include <crtdbg.h>
#include <iostream>

#include "D3D12/D3D12Renderer.hpp"
#include "Tools/CPUTimer.hpp"
#include "Tools/D3D12Timer.hpp"
#include "Tools/D3D12Tools.hpp"
#include "Tools/Profiler.hpp"
#include "D3D12/FrameBuffer.hpp"
#include "Particle/ParticleUpdateSystem.hpp"
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

    ID3D12Device* pDevice = renderer.mDevice;
    DeviceHeapMemory* pDeviceHeapMemory = renderer.mDeviceHeapMemory;

    ID3D12CommandQueue* computeCommandQueue = D3D12Tools::CreateCommandQueue(pDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE);
    ID3D12CommandAllocator* computeCommandAllocator = D3D12Tools::CreateCommandAllocator(pDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE);
    ID3D12GraphicsCommandList* computeCommandList = D3D12Tools::CreateCommandList(pDevice, computeCommandAllocator, D3D12_COMMAND_LIST_TYPE_COMPUTE);
    D3D12Tools::CloseCommandList(computeCommandList);
    ID3D12Fence* computeCompleteFence = D3D12Tools::CreateFence(pDevice);

    ID3D12CommandQueue* pGraphicsCommandQueue = renderer.mGraphicsCommandQueue;
    ID3D12CommandAllocator* graphicsCommandAllocator = D3D12Tools::CreateCommandAllocator(pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
    ID3D12GraphicsCommandList* graphicsCommandList = D3D12Tools::CreateCommandList(pDevice, graphicsCommandAllocator, D3D12_COMMAND_LIST_TYPE_DIRECT);
    D3D12Tools::CloseCommandList(graphicsCommandList);
    ID3D12Fence* graphicsCompleteFence = D3D12Tools::CreateFence(pDevice);

    //ID3D12CommandQueue* queryCommandQueue = D3D12Tools::CreateCommandQueue(pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
    //ID3D12CommandAllocator* queryCommandAllocator = D3D12Tools::CreateCommandAllocator(pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
    //ID3D12GraphicsCommandList* queryCommandList = D3D12Tools::CreateCommandList(pDevice, queryCommandAllocator, D3D12_COMMAND_LIST_TYPE_DIRECT);
    //D3D12Tools::CloseCommandList(queryCommandList);
    ID3D12Fence* queryComputeCompleteFence = D3D12Tools::CreateFence(pDevice);
    ID3D12Fence* queryGraphicsCompleteFence = D3D12Tools::CreateFence(pDevice);

    ParticleUpdateSystem particleUpdateSystem(pDevice, pDeviceHeapMemory);
    ParticleRenderSystem particleRenderSystem(pDevice, pDeviceHeapMemory, renderer.mBackBufferFormat, width, height);
    
    InputManager inputManager(renderer.mGLFWwindow);

    FrameBuffer frameBuffer(pDevice, pDeviceHeapMemory, width, height, renderer.mBackBufferFormat);
    Camera camera(60.f, &frameBuffer);
    camera.mPosition.z = -5.f;

    int lenX = 256;
    int lenY = 256;
    Scene scene(pDevice, pDeviceHeapMemory, lenX * lenY);
    {
        ID3D12CommandAllocator* uploadCommandAllocator = D3D12Tools::CreateCommandAllocator(pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
        ID3D12GraphicsCommandList* uploadCommandList = D3D12Tools::CreateCommandList(pDevice, uploadCommandAllocator, D3D12_COMMAND_LIST_TYPE_DIRECT);
        ID3D12Fence* uploadFence = D3D12Tools::CreateFence(pDevice);

        std::vector<Particle> particleList;
        Particle particle;
        float spacing = 1.f;
        float speed = 0.1f;
        particle.scale = glm::vec4(spacing / 2.f, spacing / 2.f, 0.f, 0.f);
        for (int y = 0; y < lenY; ++y)
        {
            for (int x = 0; x < lenX; ++x)
            {
                particle.position = glm::vec4(x * spacing, y * spacing, 0.f, 0.f);
                particle.velocity = -glm::normalize(particle.position + glm::vec4(speed, speed, 0.f, 0.f));
                particle.color = glm::vec4((float)y / lenY, 0.7f, 1.f - (float)x / lenX, 1.f);
                particleList.push_back(particle);
            }
        }
        scene.AddParticles(uploadCommandList, particleList);

        D3D12Tools::CloseCommandList(uploadCommandList);
        D3D12Tools::ExecuteCommandLists(pGraphicsCommandQueue, uploadCommandList);
        pGraphicsCommandQueue->Signal(uploadFence, 1);
        D3D12Tools::WaitFence(uploadFence, 1);

        uploadCommandAllocator->Release();
        uploadFence->Release();
        uploadCommandList->Release();
    }
    // --- INIT --- //

    // +++ MAIN LOOP +++ //
    {
        float dt = 0.f;
        float totalTime = 0.f;
        unsigned int frameCount = 0;
        D3D12Timer gpuComputeTimer(pDevice);
        D3D12Timer gpuGraphicsTimer(pDevice);
        Profiler profiler(1600, 200);
        while (renderer.Running())
        {
            //glm::clamp(dt, 1.f / 6000.f, 1.f / 60.f);
            bool cpuProfile = inputManager.KeyPressed(GLFW_KEY_F1);
            bool gpuProfile = inputManager.KeyPressed(GLFW_KEY_F2);
            {
                CPUTIMER(dt);
                // +++ UPDATE +++ //
                D3D12Tools::WaitFence(computeCompleteFence, renderer.mFrameID);
                D3D12Tools::ResetCommandList(computeCommandAllocator, computeCommandList);
                if (gpuProfile) gpuComputeTimer.Start(computeCommandList);

                camera.Update(20.f, 2.f, dt, &inputManager);
                particleUpdateSystem.Update(computeCommandList, &scene, dt);

                if (gpuProfile) gpuComputeTimer.Stop(computeCommandList);
                D3D12Tools::CloseCommandList(computeCommandList);
                D3D12Tools::ExecuteCommandLists(computeCommandQueue, computeCommandList);
                computeCommandQueue->Signal(computeCompleteFence, renderer.mFrameID + 1);
#ifdef SYNC_COMPUTE_GRAPHICS
                D3D12Tools::WaitFence(computeCompleteFence, renderer.mFrameID, renderer.mSyncEvent);
#endif
                // --- UPDATE --- //

                // +++ RENDER +++ //
                D3D12Tools::WaitFence(graphicsCompleteFence, renderer.mFrameID);
                D3D12Tools::ResetCommandList(graphicsCommandAllocator, graphicsCommandList);
                if (gpuProfile) gpuGraphicsTimer.Start(graphicsCommandList);

                FrameBuffer* backBuffer = renderer.SwapBackBuffer();

                backBuffer->TransitionState(graphicsCommandList, D3D12_RESOURCE_STATE_COPY_DEST);
                camera.mpFrameBuffer->Clear(graphicsCommandList, 0.2f, 0.2f, 0.2f, 0.f);
                particleRenderSystem.Render(graphicsCommandList, &scene, &camera);
                backBuffer->Copy(graphicsCommandList, camera.mpFrameBuffer);
                backBuffer->TransitionState(graphicsCommandList, D3D12_RESOURCE_STATE_PRESENT);

                if (gpuProfile) gpuGraphicsTimer.Stop(graphicsCommandList);
                D3D12Tools::CloseCommandList(graphicsCommandList);
                D3D12Tools::ExecuteCommandLists(pGraphicsCommandQueue, graphicsCommandList);
                pGraphicsCommandQueue->Signal(graphicsCompleteFence, renderer.mFrameID + 1);
#ifdef SYNC_COMPUTE_GRAPHICS
                D3D12Tools::WaitFence(graphicsCompleteFence, renderer.mFrameID, renderer.mSyncEvent);
#endif
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
            if (gpuProfile)
            {
                // Wait complete.
                D3D12Tools::WaitFence(computeCompleteFence, renderer.mFrameID);
                D3D12Tools::WaitFence(graphicsCompleteFence, renderer.mFrameID);

                // Reslove query data.
                D3D12Tools::ResetCommandList(computeCommandAllocator, computeCommandList);
                gpuComputeTimer.ResolveQuery(computeCommandList);
                D3D12Tools::CloseCommandList(computeCommandList);
                D3D12Tools::ExecuteCommandLists(computeCommandQueue, computeCommandList);
                computeCommandQueue->Signal(queryGraphicsCompleteFence, renderer.mFrameID + 1);
                D3D12Tools::WaitFence(queryGraphicsCompleteFence, renderer.mFrameID + 1);
                gpuComputeTimer.CalculateTime();

                D3D12Tools::ResetCommandList(graphicsCommandAllocator, graphicsCommandList);
                gpuGraphicsTimer.ResolveQuery(graphicsCommandList);
                D3D12Tools::CloseCommandList(graphicsCommandList);
                D3D12Tools::ExecuteCommandLists(pGraphicsCommandQueue, graphicsCommandList);
                pGraphicsCommandQueue->Signal(queryComputeCompleteFence, renderer.mFrameID + 1);
                D3D12Tools::WaitFence(queryComputeCompleteFence, renderer.mFrameID + 1);
                gpuGraphicsTimer.CalculateTime();

                // Get timestamps.
                float computeTime = 1.f / 1000000.f * gpuComputeTimer.GetDeltaTime();
                float graphicsTime = 1.f / 1000000.f * gpuGraphicsTimer.GetDeltaTime();
                std::cout << "GPU(Total) : " << computeTime + graphicsTime << " ms | GPU(Compute): " << computeTime << " ms | GPU(Graphics) : " << graphicsTime << " ms" << std::endl;
                profiler.Rectangle(gpuComputeTimer.GetBeginTime(), 1, gpuComputeTimer.GetDeltaTime(), 1, 0.f, 0.f, 1.f);
                profiler.Rectangle(gpuGraphicsTimer.GetBeginTime(), 0, gpuGraphicsTimer.GetDeltaTime(), 1, 0.f, 1.f, 0.f);
            }
            if (inputManager.KeyPressed(GLFW_KEY_F3))
            {
                std::cout << "CPU(Average delta time) : " << totalTime / frameCount * 1000.f << " ms" << std::endl;
            }
            // --- PROFILING --- //
        }
    }
    // --- MAIN LOOP --- //

    // +++ SHUTDOWN +++ //
    D3D12Tools::WaitFence(computeCompleteFence, renderer.mFrameID);
    D3D12Tools::WaitFence(graphicsCompleteFence, renderer.mFrameID);

    queryGraphicsCompleteFence->Release();
    queryComputeCompleteFence->Release();

    computeCompleteFence->Release();
    computeCommandList->Release();
    computeCommandAllocator->Release();
    computeCommandQueue->Release();

    graphicsCompleteFence->Release();
    graphicsCommandList->Release();
    graphicsCommandAllocator->Release();
    // --- SHUTDOWN --- //

    return 0;
}
