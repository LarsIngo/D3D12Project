#include "ParticleRenderSystem.hpp"
#include "../Scene/Scene.hpp"
#include "../Camera/Camera.hpp"
#include "../D3D12/FrameBuffer.hpp"
#include "../D3D12/StorageBuffer.hpp"
#include "../D3D12/DeviceHeapMemory.hpp"
#include "../Tools/D3D12Tools.hpp"

#include <d3dcompiler.h>

ParticleRenderSystem::ParticleRenderSystem(ID3D12Device* pDevice, DXGI_FORMAT format, unsigned int width, unsigned int height)
{
    mpDevice = pDevice;
    //mDeviceHeapMemory = new DeviceHeapMemory(mpDevice, 0, 1);

    mFormat = format;
    mWidth = width;
    mHeight = height;

    {
        // create a descriptor range (descriptor table) and fill it out
        // this is a range of descriptors inside a descriptor heap
        //D3D12_DESCRIPTOR_RANGE descriptorTableRanges[1]; // only one range right now
        //descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of constant buffer views (descriptors)
        //descriptorTableRanges[0].NumDescriptors = 1; // we only have one constant buffer, so the range is only 1
        //descriptorTableRanges[0].BaseShaderRegister = 0; // start index of the shader registers in the range
        //descriptorTableRanges[0].RegisterSpace = 0; // space 0. can usually be zero
        //descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables

        //// create a descriptor table
        //D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
        //descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges); // we only have one range
        //descriptorTable.pDescriptorRanges = &descriptorTableRanges[0]; // the pointer to the beginning of our ranges array

        // create a root parameter and fill it out
        D3D12_ROOT_PARAMETER rootParameters[1]; // only one parameter right now
        //rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // this is a descriptor table
        //rootParameters[0].DescriptorTable = descriptorTable; // this is our descriptor table for this root parameter
        //rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // our pixel shader will be the only shader accessing this parameter for now
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParameters[0].Descriptor.ShaderRegister = 0;
        rootParameters[0].Descriptor.RegisterSpace = 0;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
        //rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        //rootParameters[2].Descriptor.ShaderRegister = 2;
        //rootParameters[2].Descriptor.RegisterSpace = 0;
        //rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;

        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(_countof(rootParameters), // we have 1 root parameter
            rootParameters, // a pointer to the beginning of our root parameters array
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

    D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
    {
        
        ID3DBlob* vertexShader;
        ID3DBlob* errorBuff;
        HRESULT hr = D3DCompileFromFile(L"../resources/shaders/Particles_Render_VS.hlsl",
            nullptr,
            nullptr,
            "main",
            "vs_5_0",
            0,
            0,
            &vertexShader,
            &errorBuff);
        if (FAILED(hr))
        {
            OutputDebugStringA((char*)errorBuff->GetBufferPointer());
            assert(0 && "Vertex shader compile error");
        }
        vertexShaderBytecode.BytecodeLength = vertexShader->GetBufferSize();
        vertexShaderBytecode.pShaderBytecode = vertexShader->GetBufferPointer();
    }

    D3D12_SHADER_BYTECODE geometryShaderBytecode = {};
    {

        ID3DBlob* geometryShader;
        ID3DBlob* errorBuff;
        HRESULT hr = D3DCompileFromFile(L"../resources/shaders/Particles_Render_GS.hlsl",
            nullptr,
            nullptr,
            "main",
            "gs_5_0",
            0, //D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
            0,
            &geometryShader,
            &errorBuff);
        if (FAILED(hr))
        {
            OutputDebugStringA((char*)errorBuff->GetBufferPointer());
            assert(0 && "Geomery shader compile error");
        }
        geometryShaderBytecode.BytecodeLength = geometryShader->GetBufferSize();
        geometryShaderBytecode.pShaderBytecode = geometryShader->GetBufferPointer();
    }

    D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
    {
        ID3DBlob* pixelShader;
        ID3DBlob* errorBuff;
        HRESULT hr = D3DCompileFromFile(L"../resources/shaders/Particles_Render_PS.hlsl",
            nullptr,
            nullptr,
            "main",
            "ps_5_0",
            0,
            0,
            &pixelShader,
            &errorBuff);
        if (FAILED(hr))
        {
            OutputDebugStringA((char*)errorBuff->GetBufferPointer());
            assert(0 && "Pixel shader compile error");
        }
        pixelShaderBytecode.BytecodeLength = pixelShader->GetBufferSize();
        pixelShaderBytecode.pShaderBytecode = pixelShader->GetBufferPointer();
    }

    DXGI_SAMPLE_DESC sampleDesc;
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

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

    mMetaBuffer = new StorageBuffer(mpDevice, sizeof(MetaData), sizeof(MetaData));
}

ParticleRenderSystem::~ParticleRenderSystem()
{
    //delete mDeviceHeapMemory;
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

    // set constant buffer descriptor heap
    ID3D12DescriptorHeap* pDescriptorHeap = mMetaBuffer->mDeviceHeapMemory->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    ID3D12DescriptorHeap* ppDescriptorHeaps[] = { pDescriptorHeap };
    pCommandList->SetDescriptorHeaps(_countof(ppDescriptorHeaps), ppDescriptorHeaps);

    // set the root descriptor table 0 to the constant buffer descriptor heap
    //pCommandList->SetGraphicsRootDescriptorTable(0, pDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

    pCommandList->SetGraphicsRootShaderResourceView(0, mMetaBuffer->mBuff->GetGPUVirtualAddress()); // TODO

    pCommandList->SetPipelineState(mPipeline);
    pCommandList->RSSetViewports(1, &mViewport);
    pCommandList->RSSetScissorRects(1, &mScissorRect);
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
    pCommandList->OMSetRenderTargets(1, &fb->mRTV, FALSE, NULL);
    pCommandList->DrawInstanced(1, 1, 0, 0);
}
