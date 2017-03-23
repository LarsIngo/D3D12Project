#include "ParticleSystem.hpp"
#include "../D3D12/D3D12Renderer.hpp"
#include "../D3D12/FrameBuffer.hpp"
#include "../D3D12/StorageSwapBuffer.hpp"
#include "../Tools/D3D12Tools.hpp"
#include "../Scene/Scene.hpp"
#include "../Camera/Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

ParticleSystem::ParticleSystem(/*ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext*/)
{
    //mpDevice = pDevice;
    //mpDeviceContext = pDeviceContext;

    //// Create meta buffers.
    //DxHelp::CreateCPUwriteGPUreadStructuredBuffer<UpdateMetaData>(mpDevice, 1, &mUpdateMetaDataBuffer);
    //DxHelp::CreateCPUwriteGPUreadStructuredBuffer<RenderMetaData>(mpDevice, 1, &mRenderMetaDataBuffer);

    //// Create render pipeline.
    //{
    //    DxHelp::CreateVS(mpDevice, "resources/shaders/Particles_Render_VS.hlsl", "main", &mVertexShader);
    //    DxHelp::CreateGS(mpDevice, "resources/shaders/Particles_Render_GS.hlsl", "main", &mGeometryShader);
    //    DxHelp::CreatePS(mpDevice, "resources/shaders/Particles_Render_PS.hlsl", "main", &mPixelShader);

    //    {   // Create blend state.
    //        D3D11_BLEND_DESC blendDesc;
    //        ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
    //        blendDesc.AlphaToCoverageEnable = false;
    //        blendDesc.IndependentBlendEnable = true;

    //        blendDesc.RenderTarget[0].BlendEnable = true;
    //        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    //        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    //        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    //        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    //        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    //        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    //        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    //        DxAssert(mpDevice->CreateBlendState(&blendDesc, &mBlendState), S_OK);
    //    }
    //}

    //// Create update pipeline.
    //DxHelp::CreateCS(mpDevice, "resources/shaders/Particles_Update_CS.hlsl", "main", &mComputeShader);
}

ParticleSystem::~ParticleSystem()
{
    /*mUpdateMetaDataBuffer->Release();
    mRenderMetaDataBuffer->Release();

    mComputeShader->Release();

    mVertexShader->Release();
    mGeometryShader->Release();
    mPixelShader->Release();
    mBlendState->Release();*/
}

void ParticleSystem::Update(Scene* scene, float dt)
{
    /*mpDeviceContext->CSSetShader(mComputeShader, NULL, NULL);
    mpDeviceContext->CSSetShaderResources(0, 1, &scene->mParticleBuffer->GetInputBuffer()->mSRV);
    {
        mUpdateMetaData.dt = dt;
        mUpdateMetaData.particleCount = scene->mParticleCount;
        DxHelp::WriteStructuredBuffer<UpdateMetaData>(mpDeviceContext, &mUpdateMetaData, 1, mUpdateMetaDataBuffer);
        mpDeviceContext->CSSetShaderResources(1, 1, &mUpdateMetaDataBuffer);
    }
    mpDeviceContext->CSSetUnorderedAccessViews(0, 1, &scene->mParticleBuffer->GetOutputBuffer()->mUAV, NULL);

    mpDeviceContext->Dispatch(scene->mParticleCount / 256 + 1,1,1);

    mpDeviceContext->CSSetShader(NULL, NULL, NULL);
    void* p[1] = { NULL };
    mpDeviceContext->CSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)p);
    mpDeviceContext->CSSetShaderResources(1, 1, (ID3D11ShaderResourceView**)p);
    mpDeviceContext->CSSetUnorderedAccessViews(0, 1, (ID3D11UnorderedAccessView**)p, NULL);*/
}

void ParticleSystem::Render(Scene* scene, Camera* camera)
{
    /*mpDeviceContext->VSSetShader(mVertexShader, NULL, NULL);
    mpDeviceContext->GSSetShader(mGeometryShader, NULL, NULL);
    mpDeviceContext->PSSetShader(mPixelShader, NULL, NULL);
    mpDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    float blendFactor[] = { 0.f, 0.f, 0.f, 0.f };
    UINT sampleMask = 0xffffffff;
    mpDeviceContext->OMSetBlendState(mBlendState, blendFactor, sampleMask);
    mpDeviceContext->OMSetRenderTargets(1, &camera->mpFrameBuffer->mColRTV, NULL);
    mpDeviceContext->VSSetShaderResources(0, 1, &scene->mParticleBuffer->GetOutputBuffer()->mSRV);
    {
        mRenderMetaData.vpMatrix = glm::transpose(camera->mProjectionMatrix * camera->mViewMatrix);
        mRenderMetaData.lensPosition = camera->mPosition;
        mRenderMetaData.lensUpDirection = camera->mUpDirection;
        DxHelp::WriteStructuredBuffer<RenderMetaData>(mpDeviceContext, &mRenderMetaData, 1, mRenderMetaDataBuffer);
        mpDeviceContext->GSSetShaderResources(0, 1, &mRenderMetaDataBuffer);
    }

    mpDeviceContext->Draw(scene->mParticleCount, 0);

    mpDeviceContext->VSSetShader(NULL, NULL, NULL);
    mpDeviceContext->GSSetShader(NULL, NULL, NULL);
    mpDeviceContext->PSSetShader(NULL, NULL, NULL);
    mpDeviceContext->OMSetBlendState(NULL, blendFactor, sampleMask);
    void* p[1] = { NULL };
    mpDeviceContext->OMSetRenderTargets(1, (ID3D11RenderTargetView**)p, NULL);
    mpDeviceContext->VSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)p);
    mpDeviceContext->GSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)p);

    scene->mParticleBuffer->Swap();*/
}
