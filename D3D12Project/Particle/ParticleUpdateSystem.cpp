#include "ParticleUpdateSystem.hpp"
#include "../Scene/Scene.hpp"
#include "../D3D12/StorageBuffer.hpp"
#include "../D3D12/StorageSwapBuffer.hpp"
#include "../D3D12/DeviceHeapMemory.hpp"
#include "../Tools/D3D12Tools.hpp"

ParticleUpdateSystem::ParticleUpdateSystem(ID3D12Device* pDevice, DeviceHeapMemory* pDeviceHeapMemory)
{
    mpDevice = pDevice;
    mpDeviceHeapMemory = pDeviceHeapMemory;

    {
        D3D12_ROOT_PARAMETER rootParameters[3];
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParameters[0].Descriptor.ShaderRegister = 0;
        rootParameters[0].Descriptor.RegisterSpace = 0;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParameters[1].Descriptor.ShaderRegister = 1;
        rootParameters[1].Descriptor.RegisterSpace = 0;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
        rootParameters[2].Descriptor.ShaderRegister = 0;
        rootParameters[2].Descriptor.RegisterSpace = 0;
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

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

    D3D12_SHADER_BYTECODE computeShaderBytecode;
    D3D12Tools::CompileShader("resources/shaders/Particles_Update_CS.hlsl", "main", "cs_5_0", computeShaderBytecode);

    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = mRootSignature;
    psoDesc.CS = computeShaderBytecode;
    psoDesc.NodeMask = 0;
    psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    ASSERT(mpDevice->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&mPipeline)), S_OK);

    mMetaBuffer = new StorageBuffer(mpDevice, mpDeviceHeapMemory, sizeof(MetaData), sizeof(MetaData));
}

ParticleUpdateSystem::~ParticleUpdateSystem()
{
    delete mMetaBuffer;
    SAFE_RELEASE(mPipeline);
    SAFE_RELEASE(mRootSignature);
}

void ParticleUpdateSystem::Update(ID3D12GraphicsCommandList* pCommandList, Scene* scene, float dt)
{
    mMetaData.dt = dt;
    mMetaData.particleCount = scene->mParticleCount;
    mMetaBuffer->Write(pCommandList, &mMetaData, sizeof(MetaData), 0);

    pCommandList->SetComputeRootSignature(mRootSignature);

    ID3D12DescriptorHeap* ppDescriptorHeaps[] = { mpDeviceHeapMemory->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) };
    pCommandList->SetDescriptorHeaps(_countof(ppDescriptorHeaps), ppDescriptorHeaps);

    pCommandList->SetPipelineState(mPipeline);
    assert(scene->mParticleBuffer->GetInputBuffer()->mState == D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    pCommandList->SetComputeRootShaderResourceView(0, scene->mParticleBuffer->GetInputBuffer()->mBuff->GetGPUVirtualAddress());
    pCommandList->SetComputeRootShaderResourceView(1, mMetaBuffer->mBuff->GetGPUVirtualAddress());
    assert(scene->mParticleBuffer->GetOutputBuffer()->mState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    pCommandList->SetComputeRootUnorderedAccessView(2, scene->mParticleBuffer->GetOutputBuffer()->mBuff->GetGPUVirtualAddress());

    pCommandList->Dispatch(static_cast<unsigned int>(ceil(scene->mParticleCount / 128.f)), 1, 1);

    {   // CREATE DEPENDENCY TO MEASURE EXECUTION TIME OF DISPATCH CALL //https://www.gamedev.net/topic/646113-compute-shader-execution-time/
        pCommandList->CopyBufferRegion(scene->mParticleBuffer->GetOutputBuffer()->mBuff, 0, scene->mParticleBuffer->GetInputBuffer()->mBuff, 0, sizeof(Particle));
    }
}
