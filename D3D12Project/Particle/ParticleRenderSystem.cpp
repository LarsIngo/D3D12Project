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
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
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

    D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
    {
        ID3DBlob* pixelShader;
        ID3DBlob* errorBuff;
        HRESULT hr = D3DCompileFromFile(L"../resources/shaders/Particles_Render_PS.hlsl",
            nullptr,
            nullptr,
            "main",
            "ps_5_0",
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
            0,
            &pixelShader,
            &errorBuff);
        if (FAILED(hr))
        {
            OutputDebugStringA((char*)errorBuff->GetBufferPointer());
            assert(0 && "Pixel shader compile error");
        }
    }

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};


    DXGI_SAMPLE_DESC sampleDesc = {};
    sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = inputLayoutDesc;
    psoDesc.pRootSignature = mRootSignature;
    psoDesc.VS = vertexShaderBytecode;
    psoDesc.PS = pixelShaderBytecode;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.RTVFormats[0] = mFormat;
    psoDesc.SampleDesc = sampleDesc;
    psoDesc.SampleMask = 0xffffffff;
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.NumRenderTargets = 1;

    ASSERT(mpDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipeline)), S_OK);

    mViewport.TopLeftX = 0;
    mViewport.TopLeftY = 0;
    mViewport.Width = mWidth;
    mViewport.Height = mHeight;
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;

    // Fill out a scissor rect
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
    //pCommandList->SetPipelineState(mPipeline);
    pCommandList->SetGraphicsRootSignature(mRootSignature);
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    pCommandList->RSSetViewports(1, &mViewport);
    pCommandList->RSSetScissorRects(1, &mScissorRect);
    pCommandList->OMSetRenderTargets(1, &fb->mRTV, FALSE, NULL);
    pCommandList->DrawInstanced(3, 1, 0, 0);
}

