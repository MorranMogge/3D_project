#include "TesselatorClass.h"

bool TesselatorClass::setUpCameraBuffer(ID3D11Device* device)
{
	return false;
}

bool TesselatorClass::setUpShaders(ID3D11Device* device)
{
	std::string shaderData;
	std::ifstream reader;
	reader.open("../x64/Debug/HullShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cout << "Could not open hull shader file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateHullShader(shaderData.c_str(), shaderData.length(), nullptr, &hullShader)))
	{
		std::cerr << "Failed to create hull shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();

	reader.open("../x64/Debug/DomainShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cout << "Could not open domain shader file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateDomainShader(shaderData.c_str(), shaderData.length(), nullptr, &domainShader)))
	{
		std::cerr << "Failed to create domain shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();

	return true;	//If both shaders were setup correctly
}

bool TesselatorClass::setUpRasterizerState(ID3D11Device* device)
{
	D3D11_RASTERIZER_DESC desc;
	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = true;
	desc.ScissorEnable = false;
	desc.MultisampleEnable = false;
	desc.AntialiasedLineEnable = false;

	D3D11_RASTERIZER_DESC wireframedesc;
	wireframedesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	wireframedesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	wireframedesc.FrontCounterClockwise = false;
	wireframedesc.DepthBias = 0;
	wireframedesc.DepthBiasClamp = 0.0f;
	wireframedesc.SlopeScaledDepthBias = 0.0f;
	wireframedesc.DepthClipEnable = true;
	wireframedesc.ScissorEnable = false;
	wireframedesc.MultisampleEnable = false;
	wireframedesc.AntialiasedLineEnable = false;

	HRESULT hr = device->CreateRasterizerState(&desc, &rasterizerState);

	if (FAILED(hr)) return false;

	hr = device->CreateRasterizerState(&wireframedesc, &wireFrameRasterizerState);

	return !FAILED(hr);
}

TesselatorClass::TesselatorClass()
	:cameraBuffer(nullptr), cameraPtr(nullptr), currentRasterizer(nullptr), domainShader(nullptr),
	hullShader(nullptr), immediateContext(nullptr), rasterizerState(nullptr), wireFrameRasterizerState(nullptr)
{
}

TesselatorClass::~TesselatorClass()
{
	rasterizerState->Release();
	hullShader->Release();
	domainShader->Release();
	wireFrameRasterizerState->Release();
	//cameraBuffer->Release();
}

bool TesselatorClass::setUpTesselator(ID3D11DeviceContext* immediateContext, ID3D11Device* device, Camera* camera)
{
	if ((this->immediateContext = immediateContext) == nullptr) return false;
	if ((this->cameraPtr = camera) == nullptr)	return false;
	if (!this->setUpShaders(device))			return false;
	if (!this->setUpRasterizerState(device))	return false;
	if (!this->setUpCameraBuffer(device))		return false;
	return true;
}

void TesselatorClass::setRasterizerState()
{
	immediateContext->RSSetState(currentRasterizer);
	immediateContext->HSSetShader(hullShader, nullptr, 0);
	immediateContext->DSSetShader(domainShader, nullptr, 0);
}

void TesselatorClass::setWireFrame(float wireframe)
{
	if (wireframe) currentRasterizer = wireFrameRasterizerState;
	else currentRasterizer = rasterizerState;
}
