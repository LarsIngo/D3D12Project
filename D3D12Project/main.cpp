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

#define SKIP_TIME_NANO 5000000000

#define PROFILE_FRAME_COUNT 1000

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

    ID3D12CommandQueue* graphicsCommandQueue = D3D12Tools::CreateCommandQueue(pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
    ID3D12CommandAllocator* graphicsCommandAllocator = D3D12Tools::CreateCommandAllocator(pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
    ID3D12GraphicsCommandList* graphicsCommandList = D3D12Tools::CreateCommandList(pDevice, graphicsCommandAllocator, D3D12_COMMAND_LIST_TYPE_DIRECT);
    D3D12Tools::CloseCommandList(graphicsCommandList);
    ID3D12Fence* graphicsCompleteFence = D3D12Tools::CreateFence(pDevice);

    ID3D12Fence* queryComputeCompleteFence = D3D12Tools::CreateFence(pDevice);
    ID3D12Fence* queryGraphicsCompleteFence = D3D12Tools::CreateFence(pDevice);

    ParticleUpdateSystem particleUpdateSystem(pDevice, pDeviceHeapMemory);
    ParticleRenderSystem particleRenderSystem(pDevice, pDeviceHeapMemory, renderer.mBackBufferFormat, width, height);
    
    InputManager inputManager(renderer.mGLFWwindow);

    FrameBuffer frameBuffer(pDevice, pDeviceHeapMemory, width, height, renderer.mBackBufferFormat);
    Camera camera(60.f, &frameBuffer);

    int lenX = 256;
    int lenY = 256;
    Scene scene(pDevice, pDeviceHeapMemory, lenX * lenY);
    {
        ID3D12CommandQueue* uploadCommandQueue = D3D12Tools::CreateCommandQueue(pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
        ID3D12CommandAllocator* uploadCommandAllocator = D3D12Tools::CreateCommandAllocator(pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
        ID3D12GraphicsCommandList* uploadCommandList = D3D12Tools::CreateCommandList(pDevice, uploadCommandAllocator, D3D12_COMMAND_LIST_TYPE_DIRECT);
        ID3D12Fence* uploadFence = D3D12Tools::CreateFence(pDevice);

        std::vector<Particle> particleList;
        Particle particle;
        float spacing = 1.f;
        float speed = 0.1f;
        particle.scale = glm::vec4(spacing * 0.75f, spacing * 0.75f, 0.f, 0.f);
        for (int y = 0; y < lenY; ++y)
        {
            for (int x = 0; x < lenX; ++x)
            {
                particle.position = glm::vec4(x * spacing, y * spacing, 0.f, 0.f);
                //particle.velocity = -glm::normalize(particle.position + glm::vec4(speed, speed, 0.f, 0.f));
                particle.velocity = glm::vec4(0.f, 0.f, 0.f, 0.f);
                particle.color = glm::vec4((float)y / lenY, 0.7f, 1.f - (float)x / lenX, 1.f);
                particleList.push_back(particle);
            }
        }
        scene.AddParticles(uploadCommandList, particleList);
        
        camera.mPosition.x = lenX / 2.f * spacing;
        camera.mPosition.y = lenY / 2.f * spacing;
        camera.mPosition.z = -50.f;

        D3D12Tools::CloseCommandList(uploadCommandList);
        D3D12Tools::ExecuteCommandLists(uploadCommandQueue, uploadCommandList);
        uploadCommandQueue->Signal(uploadFence, 1);
        D3D12Tools::WaitFence(uploadFence, 1);

        uploadCommandQueue->Release();
        uploadCommandAllocator->Release();
        uploadFence->Release();
        uploadCommandList->Release();
    }
    // --- INIT --- //

    // +++ MAIN LOOP +++ //
    {
        double startTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        double currentTime = 0.0;
        double totalTime = 0.0;
        double totalMeasureTime = 0.0;
        float mt = 0.f;
        float dt = 0.f;
        double profileFrames[PROFILE_FRAME_COUNT];
        for (int i = 0; i < PROFILE_FRAME_COUNT; ++i)
            profileFrames[i] = 0.0;
        double averageTime = 0.0;

        std::cout << "+++ Skip time: " << SKIP_TIME_NANO << " nanoseconds. (Wait for program to stabilize) +++" << std::endl;
        std::cout << "Hold F1 to sync compute/graphics. " << std::endl;
        std::cout << "Hold F2 to profile. " << std::endl;
        std::cout << "Hold F3 to show average frame time. " << std::endl;
        unsigned int frameCount = 0;

        D3D12Timer gpuComputeTimer(pDevice);
        float computeBaseline = 0.f;
        {
            ID3D12Fence* baselineFence = D3D12Tools::CreateFence(pDevice);

            // Reset compute command list;
            D3D12Tools::ResetCommandList(computeCommandAllocator, computeCommandList);

            // Get baseline.
            gpuComputeTimer.Start(computeCommandList);
            gpuComputeTimer.Stop(computeCommandList);

            D3D12Tools::CloseCommandList(computeCommandList);
            D3D12Tools::ExecuteCommandLists(computeCommandQueue, computeCommandList);

            computeCommandQueue->Signal(baselineFence, 1);
            D3D12Tools::WaitFence(baselineFence, 1);

            // Resolve compute query data.
            D3D12Tools::ResetCommandList(computeCommandAllocator, computeCommandList);
            gpuComputeTimer.ResolveQuery(computeCommandList);
            D3D12Tools::CloseCommandList(computeCommandList);
            D3D12Tools::ExecuteCommandLists(computeCommandQueue, computeCommandList);

            // Resolve compute query data.
            computeCommandQueue->Signal(baselineFence, 2);
            D3D12Tools::WaitFence(baselineFence, 2);
            gpuComputeTimer.CalculateTime(computeCommandQueue);
            computeBaseline = gpuComputeTimer.GetBeginTime();

            baselineFence->Release();
        }

        D3D12Timer gpuGraphicsTimer(pDevice);
        float graphicsBaseline = 0.f;
        {
            ID3D12Fence* baselineFence = D3D12Tools::CreateFence(pDevice);

            // Reset graphics command list;
            D3D12Tools::ResetCommandList(graphicsCommandAllocator, graphicsCommandList);

            // Get baseline.
            gpuGraphicsTimer.Start(graphicsCommandList);
            gpuGraphicsTimer.Stop(graphicsCommandList);

            D3D12Tools::CloseCommandList(graphicsCommandList);
            D3D12Tools::ExecuteCommandLists(graphicsCommandQueue, graphicsCommandList);

            graphicsCommandQueue->Signal(baselineFence, 1);
            D3D12Tools::WaitFence(baselineFence, 1);

            // Resolve graphics query data.
            D3D12Tools::ResetCommandList(graphicsCommandAllocator, graphicsCommandList);
            gpuGraphicsTimer.ResolveQuery(graphicsCommandList);
            D3D12Tools::CloseCommandList(graphicsCommandList);
            D3D12Tools::ExecuteCommandLists(graphicsCommandQueue, graphicsCommandList);

            // Resolve graphics query data.
            graphicsCommandQueue->Signal(baselineFence, 2);
            D3D12Tools::WaitFence(baselineFence, 2);
            gpuGraphicsTimer.CalculateTime(graphicsCommandQueue);
            graphicsBaseline = gpuGraphicsTimer.GetBeginTime();
            

            baselineFence->Release();
        }

        // Set baseline.
        if (computeBaseline > graphicsBaseline)
        {
            gpuComputeTimer.mBaseline = computeBaseline - graphicsBaseline;
        }
        else
        {
            gpuGraphicsTimer.mBaseline = graphicsBaseline - computeBaseline;
        }

        Profiler profiler(1600, 200);
        while (renderer.Running())
        {
            //glm::clamp(dt, 1.f / 6000.f, 1.f / 60.f);
            bool syncComputeGraphics = inputManager.KeyPressed(GLFW_KEY_F1);
            {
                double lastTime = currentTime;
                currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                dt = (currentTime - lastTime) / 1000000000;
                totalTime = currentTime - startTime;

                CPUTIMER(mt);
                // +++ UPDATE +++ //
                //D3D12Tools::WaitFence(computeCompleteFence, renderer.mFrameID);
                D3D12Tools::ResetCommandList(computeCommandAllocator, computeCommandList);
                if (totalTime > SKIP_TIME_NANO) gpuComputeTimer.Start(computeCommandList);

                camera.Update(20.f, 2.f, dt, &inputManager);
                particleUpdateSystem.Update(computeCommandList, &scene, dt);

                if (totalTime > SKIP_TIME_NANO) gpuComputeTimer.Stop(computeCommandList);
                D3D12Tools::CloseCommandList(computeCommandList);
                D3D12Tools::ExecuteCommandLists(computeCommandQueue, computeCommandList);
                computeCommandQueue->Signal(computeCompleteFence, renderer.mFrameID + 1);
                //SYNC_COMPUTE_GRAPHICS
                if (syncComputeGraphics) D3D12Tools::WaitFence(computeCompleteFence, renderer.mFrameID + 1);
                // --- UPDATE --- //

                // +++ RENDER +++ //
                //D3D12Tools::WaitFence(graphicsCompleteFence, renderer.mFrameID);
                D3D12Tools::ResetCommandList(graphicsCommandAllocator, graphicsCommandList);
                if (totalTime > SKIP_TIME_NANO) gpuGraphicsTimer.Start(graphicsCommandList);

                camera.mpFrameBuffer->Clear(graphicsCommandList, 0.2f, 0.2f, 0.2f, 0.f);
                particleRenderSystem.Render(graphicsCommandList, &scene, &camera);

                if (totalTime > SKIP_TIME_NANO) gpuGraphicsTimer.Stop(graphicsCommandList);
                D3D12Tools::CloseCommandList(graphicsCommandList);
                D3D12Tools::ExecuteCommandLists(graphicsCommandQueue, graphicsCommandList);
                graphicsCommandQueue->Signal(graphicsCompleteFence, renderer.mFrameID + 1);
                // --- RENDER --- //

                // Wait on CPU for compute and graphics to complete.
                D3D12Tools::WaitFence(computeCompleteFence, renderer.mFrameID + 1);
                D3D12Tools::WaitFence(graphicsCompleteFence, renderer.mFrameID + 1);
            }

            // +++ PRESENET +++ //
            // Present frame.
            renderer.Present(camera.mpFrameBuffer);
            // --- PRESENET --- //

            // +++ PROFILING +++ //
            if (totalTime > SKIP_TIME_NANO)
            {
                if (frameCount == 0)
                    std::cout << "--- Skip time over --- " << std::endl << std::endl;

                totalMeasureTime += mt;
                ++frameCount;

                if (inputManager.KeyPressed(GLFW_KEY_F2))
                {
                    // Resolve compute query data.
                    D3D12Tools::ResetCommandList(computeCommandAllocator, computeCommandList);
                    gpuComputeTimer.ResolveQuery(computeCommandList);
                    D3D12Tools::CloseCommandList(computeCommandList);
                    D3D12Tools::ExecuteCommandLists(computeCommandQueue, computeCommandList);

                    // Fetch compute query data.
                    computeCommandQueue->Signal(queryComputeCompleteFence, renderer.mFrameID + 1);
                    D3D12Tools::WaitFence(queryComputeCompleteFence, renderer.mFrameID + 1);
                    gpuComputeTimer.CalculateTime(computeCommandQueue);

                    // Resolve graphics query data.
                    D3D12Tools::ResetCommandList(graphicsCommandAllocator, graphicsCommandList);
                    gpuGraphicsTimer.ResolveQuery(graphicsCommandList);
                    D3D12Tools::CloseCommandList(graphicsCommandList);
                    D3D12Tools::ExecuteCommandLists(graphicsCommandQueue, graphicsCommandList);

                    // Resolve graphics query data.
                    graphicsCommandQueue->Signal(queryGraphicsCompleteFence, renderer.mFrameID + 1);
                    D3D12Tools::WaitFence(queryGraphicsCompleteFence, renderer.mFrameID + 1);
                    gpuGraphicsTimer.CalculateTime(graphicsCommandQueue);

                    // Get timestamps.
                    float computeTime = 1.f / 1000000.f * gpuComputeTimer.GetDeltaTime();
                    float graphicsTime = 1.f / 1000000.f * gpuGraphicsTimer.GetDeltaTime();
                    std::cout << "GPU(Total) : " << computeTime + graphicsTime << " ms | GPU(Compute): " << computeTime << " ms | GPU(Graphics) : " << graphicsTime << " ms" << std::endl;
                    profiler.Rectangle(gpuComputeTimer.GetBeginTime(), 1, gpuComputeTimer.GetDeltaTime(), 1, 0.f, 0.f, 1.f);
                    profiler.Rectangle(gpuGraphicsTimer.GetBeginTime(), 0, gpuGraphicsTimer.GetDeltaTime(), 1, 0.f, 1.f, 0.f);
                    profiler.Point(gpuGraphicsTimer.GetBeginTime(), totalMeasureTime / frameCount, syncComputeGraphics ? "'-ro'" : "'-bo'");
                }

                // CALCULATE AVERAGE FRAME TIME OF LAST NUMBER OF FRAMES
                averageTime -= profileFrames[frameCount % PROFILE_FRAME_COUNT];
                profileFrames[frameCount % PROFILE_FRAME_COUNT] = mt;
                averageTime += mt;

                if (inputManager.KeyPressed(GLFW_KEY_F3))
                {
                    std::cout << "CPU(Average delta time of last " << PROFILE_FRAME_COUNT << " frames) : " << averageTime / PROFILE_FRAME_COUNT / 1000000 << " ms : FrameCount: " << frameCount << std::endl;
                }
            }
            // --- PROFILING --- //
        }
    }
    // --- MAIN LOOP --- //

    // +++ SHUTDOWN +++ //
    queryComputeCompleteFence->Release();
    queryGraphicsCompleteFence->Release();

    computeCompleteFence->Release();
    computeCommandList->Release();
    computeCommandAllocator->Release();
    computeCommandQueue->Release();

    graphicsCommandQueue->Release();
    graphicsCompleteFence->Release();
    graphicsCommandList->Release();
    graphicsCommandAllocator->Release();
    // --- SHUTDOWN --- //

    return 0;
}
