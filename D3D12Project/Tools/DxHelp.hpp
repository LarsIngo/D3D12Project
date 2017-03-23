#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <vector>

#include "DxAssert.hpp"

namespace DxHelp 
{
    // Creates buffer.
    // device D3D11 device.
    // usage Buffer useage flag.
    // bindFlags Buffer bind flags.
    // cpuAccessFlags Buffer CPU access flags.
    // buffer The buffer.
    // initData Pointer to initial data.
    // numOfElements Maximum number of elements (size of buffer equals sizeOf(T) * numOfElements).
    template <typename T>
    void CreateBuffer(ID3D11Device* device, D3D11_USAGE usage, UINT bindFlags, UINT cpuAccessFlags, ID3D11Buffer** buffer, T* initData = nullptr, unsigned int numOfElements = 0);

    // Creates source stuctured buffer (CPU write, GPU read).
    // device D3D11 device.
    // numOfElements Maximum number of elements (size of buffer equals sizeOf(T) * numOfElements).
    // buffer Structured buffer.
    template <typename T>
    void CreateCPUwriteGPUreadStructuredBuffer(ID3D11Device* device, unsigned int numOfElements, ID3D11ShaderResourceView** buffer);

    // Creates target stuctured buffer (CPU read, GPU write).
    // device D3D11 device.
    // numOfElements Maximum number of elements (size of buffer equals sizeOf(T) * numOfElements).
    // buffer Structured buffer.
    template <typename T>
    void CreateCPUreadGPUwriteStructuredBuffer(ID3D11Device* device, unsigned int numOfElements, ID3D11UnorderedAccessView** buffer);

    // Write stuctured data to the GPU.
    // deviceContext D3D11 device context.
    // data Pointer to array of data.
    // numOfElements Number of elements in array.
    // buffer Structured buffer.
    template <typename T>
    void WriteStructuredBuffer(ID3D11DeviceContext* deviceContext, T* data, unsigned int numOfElements, ID3D11ShaderResourceView* buffer);

    // Reads structured data from the GPU.
    // deviceContext D3D11 device context.
    // buffer Structured buffer.
    // Returns pointer to data.
    template <typename T>
    T* ReadStructuredBuffer(ID3D11DeviceContext* deviceContext, ID3D11UnorderedAccessView* buffer);

    // Write buffer data to the GPU.
    // deviceContext D3D11 device context.
    // data Pointer to array of data.
    // numOfElements Number of elements in array.
    // buffer Buffer to write.
    template <typename T>
    void WriteBuffer(ID3D11DeviceContext* deviceContext, T* data, unsigned int numOfElements, ID3D11Buffer* buffer);

    // Create vertex shader.
    // device D3D11 device.
    // shaderPath Path to shader.
    // shader Created shader.
    // inputDesc Input layout description.
    // inputLayout Input layout.
    void CreateVS(ID3D11Device* device, const char* shaderPath, const char* entry, ID3D11VertexShader** shader);

    // Create geometry shader.
    // device D3D11 device.
    // shaderPath Path to shader.
    // shader Created shader.
    void CreateGS(ID3D11Device* device, const char* shaderPath, const char* entry, ID3D11GeometryShader** shader);

    // Create pixel shader.
    // device D3D11 device.
    // shaderPath Path to shader.
    // shader Created shader.
    void CreatePS(ID3D11Device* device, const char* shaderPath, const char* entry, ID3D11PixelShader** shader);

    // Create compute shader.
    // device D3D11 device.
    // shaderPath Path to shader.
    // shader Created shader.
    void CreateCS(ID3D11Device* device, const char* shaderPath, const char* entry, ID3D11ComputeShader** shader);

    // Copy texture.
    // deviceContext D3D11 device context.
    // dst Texture to copy to.
    // src Texture to read from.
    // texWidth Texture width in pixels.
    // texHeight Texture height in pixels.
    // texMipLevels Texture mip levels.
    void CopyTexture(ID3D11DeviceContext* deviceContext, ID3D11Texture2D* dst, ID3D11Texture2D* src, unsigned int texWidth, unsigned int texHeight, unsigned int texMipLevels);
}

template <typename T>
inline void DxHelp::CreateBuffer(ID3D11Device* device, D3D11_USAGE usage, UINT bindFlags, UINT cpuAccessFlags, ID3D11Buffer** buffer, T* initData, unsigned int numOfElements)
{
    D3D11_BUFFER_DESC buffDesc;
    ZeroMemory(&buffDesc, sizeof(D3D11_BUFFER_DESC));
    buffDesc.ByteWidth = sizeof(T) * numOfElements;
    buffDesc.Usage = usage;
    buffDesc.BindFlags = bindFlags;
    buffDesc.CPUAccessFlags = cpuAccessFlags;
    buffDesc.MiscFlags = 0;
    buffDesc.StructureByteStride = sizeof(T);

    if (numOfElements != 0 && initData != nullptr)
    {
        D3D11_SUBRESOURCE_DATA subRes;
        ZeroMemory(&subRes, sizeof(D3D11_SUBRESOURCE_DATA));
        subRes.pSysMem = initData;
        subRes.SysMemPitch = 0;
        subRes.SysMemPitch = 0;
        DxAssert(device->CreateBuffer(&buffDesc, &subRes, buffer), S_OK);
    }
    else
        DxAssert(device->CreateBuffer(&buffDesc, NULL, buffer), S_OK);
}

template <typename T>
inline void DxHelp::CreateCPUwriteGPUreadStructuredBuffer(ID3D11Device* device, unsigned int numOfElements, ID3D11ShaderResourceView** buffer)
{
    ID3D11Buffer* localBuffer;
    D3D11_BUFFER_DESC bDesc;
    ZeroMemory(&bDesc, sizeof(D3D11_BUFFER_DESC));
    bDesc.ByteWidth = sizeof(T) * numOfElements;
    bDesc.Usage = D3D11_USAGE_DYNAMIC;
    bDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bDesc.StructureByteStride = sizeof(T);
    DxAssert(device->CreateBuffer(&bDesc, NULL, &localBuffer), S_OK);

    D3D11_SHADER_RESOURCE_VIEW_DESC srcDesc;
    ZeroMemory(&srcDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    srcDesc.Format = DXGI_FORMAT_UNKNOWN;
    srcDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srcDesc.Buffer.FirstElement = 0;
    srcDesc.Buffer.ElementOffset = 0;
    srcDesc.Buffer.NumElements = numOfElements;
    DxAssert(device->CreateShaderResourceView(localBuffer, &srcDesc, buffer), S_OK);

    localBuffer->Release();
}

template <typename T>
inline void DxHelp::CreateCPUreadGPUwriteStructuredBuffer(ID3D11Device* device, unsigned int numOfElements, ID3D11UnorderedAccessView** buffer)
{
    ID3D11Buffer* uavBuffer;
    D3D11_BUFFER_DESC bDesc;
    ZeroMemory(&bDesc, sizeof(D3D11_BUFFER_DESC));
    bDesc.ByteWidth = sizeof(T) * numOfElements;
    bDesc.Usage = D3D11_USAGE_DEFAULT;
    bDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    bDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bDesc.StructureByteStride = sizeof(T);
    DxAssert(device->CreateBuffer(&bDesc, NULL, &uavBuffer), S_OK);

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = numOfElements;
    uavDesc.Buffer.Flags = 0;
    DxAssert(device->CreateUnorderedAccessView(uavBuffer, &uavDesc, buffer), S_OK);

    uavBuffer->Release();
}

template <typename T>
inline void DxHelp::WriteStructuredBuffer(ID3D11DeviceContext* deviceContext, T* data, unsigned int numOfElements, ID3D11ShaderResourceView* buffer)
{
    ID3D11Resource* res;
    buffer->GetResource(&res);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    DxAssert(deviceContext->Map(res, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource), S_OK);
    memcpy(mappedResource.pData, data, sizeof(T) * numOfElements);
    deviceContext->Unmap(res, 0);
    res->Release();
}

template <typename T>
inline T* DxHelp::ReadStructuredBuffer(ID3D11DeviceContext* deviceContext, ID3D11UnorderedAccessView* buffer)
{
    ID3D11Resource* res;
    buffer->GetResource(&res);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    DxAssert(deviceContext->Map(res, 0, D3D11_MAP_READ, 0, &mappedResource), S_OK);
    deviceContext->Unmap(res, 0);
    res->Release();

    return (T*)(mappedResource.pData);
}

template <typename T>
inline void DxHelp::WriteBuffer(ID3D11DeviceContext* deviceContext, T* data, unsigned int numOfElements, ID3D11Buffer* buffer)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    DxAssert(deviceContext->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource), S_OK);
    memcpy(mappedResource.pData, data, sizeof(T) * numOfElements);
    deviceContext->Unmap(buffer, 0);
}

inline void DxHelp::CreateVS(ID3D11Device* device, const char* shaderPath, const char* entry, ID3D11VertexShader** shader)
{
    std::string s(shaderPath);
    std::wstring path(s.begin(), s.end());
    ID3DBlob* compiledShader = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(
        path.c_str(),
        nullptr,
        nullptr,
		entry,
        "vs_5_0",
        0,
        0,
        &compiledShader,
        &errorBlob
    );
    if (FAILED(hr)) {
        std::string errorMsg = (char*)errorBlob->GetBufferPointer();
        OutputDebugStringA(errorMsg.c_str());
        errorBlob->Release();
    }

    DxAssert(device->CreateVertexShader(
        compiledShader->GetBufferPointer(),
        compiledShader->GetBufferSize(),
        NULL,
        shader
    ), S_OK);

    compiledShader->Release();
}

inline void DxHelp::CreateGS(ID3D11Device* device, const char* shaderPath, const char* entry, ID3D11GeometryShader** shader)
{
    std::string s(shaderPath);
    std::wstring path(s.begin(), s.end());
    ID3DBlob* compiledShader = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(
        path.c_str(),
        nullptr,
        nullptr,
		entry,
        "gs_5_0",
        0,
        0,
        &compiledShader,
        &errorBlob
    );
    if (FAILED(hr)) {
        std::string errorMsg = (char*)errorBlob->GetBufferPointer();
        OutputDebugStringA(errorMsg.c_str());
        errorBlob->Release();
    }

    DxAssert(device->CreateGeometryShader(
        compiledShader->GetBufferPointer(),
        compiledShader->GetBufferSize(),
        NULL,
        shader
    ), S_OK);
    compiledShader->Release();
}


inline void DxHelp::CreatePS(ID3D11Device* device, const char* shaderPath, const char* entry, ID3D11PixelShader** shader)
{
    std::string s(shaderPath);
    std::wstring path(s.begin(), s.end());
    ID3DBlob* compiledShader = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(
        path.c_str(),
        nullptr,
        nullptr,
		entry,
        "ps_5_0",
        0,
        0,
        &compiledShader,
        &errorBlob
    );
    if (FAILED(hr)) {
        std::string errorMsg = (char*)errorBlob->GetBufferPointer();
        OutputDebugStringA(errorMsg.c_str());
        errorBlob->Release();
    }

    DxAssert(device->CreatePixelShader(
        compiledShader->GetBufferPointer(),
        compiledShader->GetBufferSize(),
        NULL,
        shader
    ), S_OK);
    compiledShader->Release();
}

inline void DxHelp::CreateCS(ID3D11Device* device, const char* shaderPath, const char* entry, ID3D11ComputeShader** shader)
{
    std::string s(shaderPath);
    std::wstring path(s.begin(), s.end());
    ID3DBlob* compiledShader = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(
        path.c_str(),
        nullptr,
        nullptr,
        entry,
        "cs_5_0",
        0,
        0,
        &compiledShader,
        &errorBlob
    );
    if (FAILED(hr)) {
        std::string errorMsg = (char*)errorBlob->GetBufferPointer();
        OutputDebugStringA(errorMsg.c_str());
        errorBlob->Release();
    }

    DxAssert(device->CreateComputeShader(
        compiledShader->GetBufferPointer(),
        compiledShader->GetBufferSize(),
        NULL,
        shader
    ), S_OK);
    compiledShader->Release();
}

inline void DxHelp::CopyTexture(ID3D11DeviceContext* deviceContext, ID3D11Texture2D* dst, ID3D11Texture2D* src, unsigned int texWidth, unsigned int texHeight, unsigned int texMipLevels)
{
    assert(dst != src);
    D3D11_BOX sourceRegion;

    for (unsigned int mipLevel = 0; mipLevel < texMipLevels; ++mipLevel)
    {
        sourceRegion.left = 0;
        sourceRegion.right = (texWidth >> mipLevel);
        sourceRegion.top = 0;
        sourceRegion.bottom = (texHeight >> mipLevel);
        sourceRegion.front = 0;
        sourceRegion.back = 1;
        assert(sourceRegion.bottom != 0 || sourceRegion.right != 0);

        deviceContext->CopySubresourceRegion(dst, D3D11CalcSubresource(mipLevel, 0, texMipLevels), 0, 0, 0, src, mipLevel, &sourceRegion);
    }
}
