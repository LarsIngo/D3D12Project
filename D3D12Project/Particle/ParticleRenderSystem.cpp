#include "ParticleRenderSystem.hpp"
#include "../Scene/Scene.hpp"
#include "../Camera/Camera.hpp"
#include "../D3D12/FrameBuffer.hpp"
#include "../D3D12/StorageBuffer.hpp"
#include "../D3D12/StorageSwapBuffer.hpp"
#include "../D3D12/DeviceHeapMemory.hpp"
#include "../Tools/D3D12Tools.hpp"

#include <d3dcompiler.h>

ParticleRenderSystem::ParticleRenderSystem(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory, DXGI_FORMAT format, unsigned int width, unsigned int height)
{
    mpDevice = pDevice;
    mpDeviceHeapMemory = pDeviceHeapMemory;

    mFormat = format;
    mWidth = width;
    mHeight = height;

    {
        D3D12_ROOT_PARAMETER rootParameters[2];
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParameters[0].Descriptor.ShaderRegister = 0;
        rootParameters[0].Descriptor.RegisterSpace = 0;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParameters[1].Descriptor.ShaderRegister = 1;
        rootParameters[1].Descriptor.RegisterSpace = 0;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;

        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(_countof(rootParameters),
            rootParameters,
            0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);

        ID3DBlob* signature;
        HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
        if (FAILED(hr))
        {
            assert(0 && "D3D12SerializeRootSignature");
        }

        hr = mpDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
        if (FAILED(hr))
        {
            assert(0 && "CreateRootSignature");
        }
    }

    D3D12_SHADER_BYTECODE vertexShaderBytecode;
    D3D12Tools::CompileShader("../resources/shaders/Particles_Render_VS.hlsl", "main", "vs_5_0", vertexShaderBytecode);

    D3D12_SHADER_BYTECODE geometryShaderBytecode;
    D3D12Tools::CompileShader("../resources/shaders/Particles_Render_GS.hlsl", "main", "gs_5_0", geometryShaderBytecode);

    D3D12_SHADER_BYTECODE pixelShaderBytecode;
    D3D12Tools::CompileShader("../resources/shaders/Particles_Render_PS.hlsl", "main", "ps_5_0", pixelShaderBytecode);

    DXGI_SAMPLE_DESC sampleDesc;
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = TRUE;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
    blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = mRootSignature;
    psoDesc.VS = vertexShaderBytecode;
    psoDesc.GS = geometryShaderBytecode;
    psoDesc.PS = pixelShaderBytecode;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    psoDesc.RTVFormats[0] = mFormat;
    psoDesc.SampleDesc = sampleDesc;
    psoDesc.SampleMask = 0xffffffff;
    psoDesc.RasterizerState = rasterizerDesc;
    psoDesc.BlendState = blendDesc;
    psoDesc.NumRenderTargets = 1;

    ASSERT(mpDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipeline)), S_OK);

    mViewport.TopLeftX = 0;
    mViewport.TopLeftY = 0;
    mViewport.Width = (float)mWidth;
    mViewport.Height = (float)mHeight;
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;

    mScissorRect.left = 0;
    mScissorRect.top = 0;
    mScissorRect.right = mWidth;
    mScissorRect.bottom = mHeight;

    mMetaBuffer = new StorageBuffer(mpDevice, mpDeviceHeapMemory, sizeof(MetaData), sizeof(MetaData));
}

ParticleRenderSystem::~ParticleRenderSystem()
{
    delete mMetaBuffer;
    SAFE_RELEASE(mPipeline);
    SAFE_RELEASE(mRootSignature);
}

void ParticleRenderSystem::Render(ID3D12GraphicsCommandList* pCommandList, Scene* scene, Camera* camera)
{
    FrameBuffer* fb = camera->mpFrameBuffer;
    fb->TransitionState(pCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    mMetaData.vpMatrix = glm::transpose(camera->mProjectionMatrix * camera->mViewMatrix);
    mMetaData.lensPosition = glm::vec4(camera->mPosition, 0.f);
    mMetaData.lensUpDirection = glm::vec4(camera->mUpDirection, 0.f);
    mMetaBuffer->Write(pCommandList, &mMetaData, sizeof(MetaData), 0);

    pCommandList->SetGraphicsRootSignature(mRootSignature);

    ID3D12DescriptorHeap* ppDescriptorHeaps[] = { mpDeviceHeapMemory->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) };
    pCommandList->SetDescriptorHeaps(_countof(ppDescriptorHeaps), ppDescriptorHeaps);

    pCommandList->SetGraphicsRootShaderResourceView(0, scene->mParticleBuffer->GetOutputBuffer()->mBuff->GetGPUVirtualAddress());
    pCommandList->SetGraphicsRootShaderResourceView(1, mMetaBuffer->mBuff->GetGPUVirtualAddress());

    pCommandList->SetPipelineState(mPipeline);
    pCommandList->RSSetViewports(1, &mViewport);
    pCommandList->RSSetScissorRects(1, &mScissorRect);
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
    pCommandList->OMSetRenderTargets(1, &fb->mRTV, FALSE, NULL);
    pCommandList->DrawInstanced(scene->mParticleCount, 1, 0, 0);
}
