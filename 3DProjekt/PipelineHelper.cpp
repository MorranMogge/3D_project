#include "PipelineHelper.h"
#include <string>
#include <fstream>
#include <iostream>
#include <DirectXMath.h>
#include <vector>
#include "memoryLeakChecker.h"

using namespace DirectX;

bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vShaderByteCode)
{
    std::string shaderData;
    std::ifstream reader;
    reader.open("../Debug/VertexShader.cso", std::ios::binary | std::ios::ate);
    if (!reader.is_open())
    {
        std::cout << "Could not open vertex shader file!" << std::endl;
        return false;
    }

    reader.seekg(0, std::ios::end);
    shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
    reader.seekg(0, std::ios::beg);

    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader)))
    {
        std::cerr<< "Failed to create vertex shader!" << std::endl;
        return false;
    }

    vShaderByteCode = shaderData;
    shaderData.clear();
    reader.close();

    reader.open("../Debug/PixelShader.cso", std::ios::binary | std::ios::ate);
    if (!reader.is_open())
    {
        std::cout << "Could not open pixel shader file!" << std::endl;
        return false;
    }

    reader.seekg(0, std::ios::end);
    shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
    reader.seekg(0, std::ios::beg);

    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
    {
        std::cerr << "Failed to create pixel shader!" << std::endl;
        return false;
    }

    return true;
}

bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode)
{
    D3D11_INPUT_ELEMENT_DESC inputDesc[3] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HRESULT hr = device->CreateInputLayout(inputDesc, 3, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

    return !FAILED(hr);
}

//bool CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer)
//{
//    SimpleVertex quad[6] =
//    {
//        //First triangle
//        { {-0.5f, 0.5f, 0.0f}, {0, 0, -1}, {0, 0} },
//        { {0.5f, -0.5f, 0.0f}, {0, 0, -1}, {1, 1} },
//        { {-0.5f, -0.5f, 0.0f}, {0, 0, -1}, {0, 1} },
//
//        //Second triangle
//        { {-0.5f, 0.5f, 0.0f}, {0, 0, -1}, {0, 0} },
//        { {0.5f, 0.5f, 0.0f}, {0, 0, -1}, {1, 0} },
//        { {0.5f, -0.5f, 0.0f}, {0, 0, -1}, {1, 1} }
//    };
//
//    D3D11_BUFFER_DESC bufferDesc;
//    bufferDesc.ByteWidth            = sizeof(quad);
//    bufferDesc.Usage                = D3D11_USAGE_IMMUTABLE;
//    bufferDesc.BindFlags            = D3D11_BIND_VERTEX_BUFFER;
//    bufferDesc.CPUAccessFlags       = 0;
//    bufferDesc.MiscFlags            = 0;
//    bufferDesc.StructureByteStride  = 0;
//
//    D3D11_SUBRESOURCE_DATA data;
//    data.pSysMem            = quad;
//    data.SysMemPitch        = 0;
//    data.SysMemSlicePitch   = 0;
//
//    HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
//
//    return !FAILED(hr);
//}

bool CreateSamplerState(ID3D11Device* device, ID3D11SamplerState*& samplerState)
{
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&samplerDesc, &samplerState);
    return !FAILED(hr);
}

bool SetupPipeline(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, 
                    ID3D11InputLayout*& inputLayout, ID3D11SamplerState*& samplerState)
{
    std::string vShaderByteCode;
    if (!LoadShaders(device, vShader, pShader, vShaderByteCode))
    {
        std::cerr << "Error loading shaders!" << std::endl;
        return false;
    }

    if (!CreateInputLayout(device, inputLayout, vShaderByteCode))
    {
        std::cerr << "Error creating input layout!" << std::endl;
        return false;
    }

    /*if (!CreateVertexBuffer(device, vertexBuffer))
    {
        std::cerr << "Error creating vertex buffer view!" << std::endl;
        return false;
    }*/

    /*if (!Create2DTexture(device, texture, srv))
    {
        std::cerr << "Error creating texture or srv!" << std::endl;
        return false;
    }*/

    if (!CreateSamplerState(device, samplerState))
    {
        std::cerr << "Error creating sampler state!" << std::endl;
        return false;
    }

    return true;
}
