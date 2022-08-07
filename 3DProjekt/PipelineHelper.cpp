#include "PipelineHelper.h"
#include <string>
#include <fstream>
#include <iostream>
#include <DirectXMath.h>
#include <vector>
#include "memoryLeakChecker.h"

using namespace DirectX;

bool CreateSamplerState(ID3D11Device* device, ID3D11SamplerState*& samplerState)
{
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0.0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&samplerDesc, &samplerState);
    return !FAILED(hr);
}

bool createCamBuffer(ID3D11Device* device, ID3D11Buffer*& camBuffer, struct CamData& camData)
{
	camData.cameraPosition = XMFLOAT3(0.0f, 0.0f, -20.0f);
	camData.tesselationConst = 20.0f;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(CamData);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &camData;
	data.SysMemPitch = data.SysMemPitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, &camBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool createImGuiBuffer(ID3D11Device* device, ID3D11Buffer*& imGuiBuffer, struct ImGuiValues& imGuiStuff)
{
	imGuiStuff.imposition = false;
	imGuiStuff.imnormal = false;
	imGuiStuff.imcolour = false;
	imGuiStuff.imwireframe = false;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(ImGuiValues);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &imGuiStuff;
	data.SysMemPitch = data.SysMemPitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, &imGuiBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool SetupPipeline(ID3D11Device* device, ID3D11SamplerState*& samplerState, ID3D11Buffer* &camBuffer, CamData& camData, ID3D11Buffer* &imGuiBuffer, ImGuiValues& imGuiStuff)
{
    if (!CreateSamplerState(device, samplerState))
    {
        std::cerr << "Error creating sampler state!" << std::endl;
        return false;
    }

	if (!createCamBuffer(device, camBuffer, camData))
	{
		std::cerr << "Error creating cam buffer!" << std::endl;
		return false;
	}

	if (!createImGuiBuffer(device, imGuiBuffer, imGuiStuff))
	{
		std::cerr << "Error creating imGui buffer" << std::endl;
		return false;
	}

    return true;
}

