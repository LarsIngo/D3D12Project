#include "ParticleRenderSystem.hpp"
#include "../Tools/D3D12Tools.hpp"
#include "../Scene/Scene.hpp"
#include "../D3D12/FrameBuffer.hpp"
#include "../D3D12/StorageBuffer.hpp"
#include "../Camera/Camera.hpp"

ParticleRenderSystem::ParticleRenderSystem(ID3D12Device* pDevice, DXGI_FORMAT format, unsigned int width, unsigned int height)
{
    mpDevice = pDevice;

    mFormat = format;
    mWidth = width;
    mHeight = height;

    {
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);

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
}

ParticleRenderSystem::~ParticleRenderSystem()
{
    SAFE_RELEASE(mPipeline);
    SAFE_RELEASE(mRootSignature);
}

void ParticleRenderSystem::Render(ID3D12GraphicsCommandList* pCommandList, Scene* scene, Camera* camera)
{
    FrameBuffer* fb = camera->mpFrameBuffer;
    fb->TransitionState(pCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    pCommandList->SetGraphicsRootSignature(mRootSignature);
    pCommandList->SetPipelineState(mPipeline);
    pCommandList->RSSetViewports(1, &mViewport);
    pCommandList->RSSetScissorRects(1, &mScissorRect);
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
    pCommandList->OMSetRenderTargets(1, &fb->mRTV, FALSE, NULL);
    pCommandList->DrawInstanced(1, 1, 0, 0);
}
