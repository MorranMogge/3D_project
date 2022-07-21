#include "DeferredRenderer.h"
#include <string>
#include <fstream>
#include <iostream>
bool DeferredRenderer::bindSrvAndRtv(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));

	subResource.pSysMem = {};
	subResource.SysMemPitch = 0;
	HRESULT hr;

	for (int i = 0; i < G_BUFFER_SIZE; i++)
	{
		hr = device->CreateTexture2D(&textureDesc, NULL, &texture[i]);
		if (FAILED(hr)) return false;
		hr = device->CreateRenderTargetView(texture[i], NULL, &rtv[i]);
		if (FAILED(hr)) return false;
		hr = device->CreateShaderResourceView(texture[i], nullptr, &srv[i]);
		if (FAILED(hr)) return false;
	}

	return true;
}

bool DeferredRenderer::setUpUav(ID3D11Device* device, IDXGISwapChain* swapChain)
{
	ID3D11Texture2D* backBuffer = nullptr;
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
	{
		std::cout << "Failed to get back buffer!" << std::endl;
		return false;
	}

	HRESULT hr = device->CreateUnorderedAccessView(backBuffer, NULL, &uaView);
	backBuffer->Release();
	return !FAILED(hr);
}

bool DeferredRenderer::setUpShaders(ID3D11Device* device, std::string& vShaderByteCode)
{
	std::string shaderData;
	std::ifstream reader;
	reader.open("../x64/Debug/VertexShader.cso", std::ios::binary | std::ios::ate);
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
		std::cerr << "Failed to create vertex shader!" << std::endl;
		return false;
	}

	vShaderByteCode = shaderData;
	shaderData.clear();
	reader.close();


	reader.open("../x64/Debug/tempPixel.cso", std::ios::binary | std::ios::ate);
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

	shaderData.clear();
	reader.close();

	reader.open("../x64/Debug/ComputeShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cout << "Could not open Compute shader file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateComputeShader(shaderData.c_str(), shaderData.length(), nullptr, &cShader)))
	{
		std::cerr << "Failed to create Compute shader!" << std::endl;
		return false;
	}
	shaderData.clear();
	reader.close();

	return true;
}

DeferredRenderer::DeferredRenderer(int width, int height)
	:width(width), height(height)
{
	vShader = nullptr;
	pShader = nullptr;
	cShader = nullptr;
	inputLayout = nullptr;
	immediateContext = nullptr;
	for (int i = 0; i < G_BUFFER_SIZE; i++)
	{
		rtv[i] = nullptr;
		srv[i] = nullptr;
		texture[i] = nullptr;
	}
	uaView = nullptr;
	imGuiStruct = nullptr;
	dsView = nullptr;
	camPtr = nullptr;
}

DeferredRenderer::~DeferredRenderer()
{
	for (int i = 0; i < G_BUFFER_SIZE; i++)
	{
		texture[i]->Release();
		srv[i]->Release();
		rtv[i]->Release();
	}
	uaView->Release();
	inputLayout->Release();

	vShader->Release();
	pShader->Release();
	cShader->Release();
}

bool DeferredRenderer::setComputeShader(ID3D11ComputeShader* cShader)
{
	return ((this->cShader=cShader)!=nullptr);
}

bool DeferredRenderer::setUpInputLayout(ID3D11Device* device, const std::string& vShaderByteCode)
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

bool DeferredRenderer::initiateDeferredRenderer(ID3D11DeviceContext* immediateContext, ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11DepthStencilView* dsView, Camera* camera, ImGuiValues* imGuiStruct)
{
	std::string vShaderByteCode;
	this->imGuiStruct = imGuiStruct;
	this->immediateContext = immediateContext;
	this->camPtr = camera;
	this->dsView = dsView;
	if (this->imGuiStruct == nullptr || this->immediateContext == nullptr || this->camPtr == nullptr || this->dsView == nullptr)	return false;
	if (!this->setUpShaders(device, vShaderByteCode))		return false;
	if (!this->setUpInputLayout(device, vShaderByteCode))	return false;
	if (!this->setUpUav(device, swapChain))					return false;
	if (!this->bindSrvAndRtv(device))		return false;
	return true;
}

void DeferredRenderer::firstPass()
{
	float clearColour[4]{ 0.0, 0.0, 0.0, 0.0 };
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	for (int i = 0; i < G_BUFFER_SIZE; i++)
	{
		immediateContext->ClearRenderTargetView(rtv[i], clearColour);
	}
	immediateContext->OMSetRenderTargets(G_BUFFER_SIZE, rtv, dsView);

	//Layout and other stuff
	immediateContext->IASetInputLayout(inputLayout);
	//immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->PSSetShader(pShader, nullptr, 0);
}

void DeferredRenderer::updateBuffers(CamData camData, ImGuiValues imGuiStuff)
{
}

void DeferredRenderer::ComputeShaderPass()
{
}

void DeferredRenderer::secondPass()
{
	immediateContext->VSSetShader(nullptr, nullptr, 0);
	immediateContext->HSSetShader(nullptr, nullptr, 0);	//Since we use tesselation for LOD
	immediateContext->DSSetShader(nullptr, nullptr, 0); //-||-
	immediateContext->PSSetShader(nullptr, nullptr, 0);

	ID3D11RenderTargetView* nullRtv[5]{ nullptr };// = nullptr;
	immediateContext->OMSetRenderTargets(5, nullRtv, nullptr);
	immediateContext->CSSetShader(cShader, nullptr, 0);
	immediateContext->CSSetUnorderedAccessViews(0, 1, &uaView, nullptr);
	immediateContext->CSSetShaderResources(0, 5, srv);
	//immediateContext->CSSetConstantBuffers(0, 1, &camBuffer);
	//immediateContext->CSSetConstantBuffers(1, 1, &imGuiBuffer);
	immediateContext->Dispatch(48, 32, 1);
}
