#include "ParticleHandler.h"
#include <fstream>

bool ParticleHandler::setUpUAV(ID3D11Device* device)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc =

	{
	uavDesc.Format = DXGI_FORMAT_R32_FLOAT,
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER,

	uavDesc.Buffer = {
						uavDesc.Buffer.FirstElement = 0,
						uavDesc.Buffer.NumElements = particles.size() * 3,
						uavDesc.Buffer.Flags = 0
						}
	};

	HRESULT hr = device->CreateUnorderedAccessView(vBuffer, &uavDesc, &particleUAV);
	return !FAILED(hr);
}

void ParticleHandler::updateWorldMatrix(int index)
{
	if (index == 0) worldMatrix = DirectX::XMMatrixIdentity() * DirectX::XMMatrixTranslation(0,1,0);// *DirectX::XMMatrixTranslation(particles[index].x, particles[index].y, particles[index].z);
	else worldMatrix = DirectX::XMMatrixTranspose(worldMatrix); worldMatrix *= DirectX::XMMatrixTranslation(1.0f / 144.0f, 0, 0);
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
	DirectX::XMStoreFloat4x4(&wrlMtx, worldMatrix);

	D3D11_MAPPED_SUBRESOURCE mapRes;
	ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HRESULT hr = immediateContext->Map(constBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapRes);
	memcpy(mapRes.pData, &wrlMtx, sizeof(wrlMtx));
	immediateContext->Unmap(constBuffer, 0);
}

bool ParticleHandler::setUpShaders(ID3D11Device* device)
{
	std::string shaderData;
	std::ifstream reader;
	reader.open("../x64/Debug/ParticleVertex.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cout << "Could not open particle vertex shader file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pVertex)))
	{
		std::cerr << "Failed to create particle vertex shader!" << std::endl;
		return false;
	}

	vShaderByteCode = shaderData;
	shaderData.clear();
	reader.close();


	reader.open("../x64/Debug/ParticlePixelShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cout << "Could not open particle pixel shader file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pPixel)))
	{
		std::cerr << "Failed to create particle pixel shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();

	reader.open("../x64/Debug/ParticleShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cout << "Could not open particle Compute shader file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateComputeShader(shaderData.c_str(), shaderData.length(), nullptr, &pCompute)))
	{
		std::cerr << "Failed to create particle pixel shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();

	reader.open("../x64/Debug/ParticleGeometryShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cout << "Could not open particle particle Geometry shader file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateGeometryShader(shaderData.c_str(), shaderData.length(), nullptr, &pGeometry)))
	{
		std::cerr << "Failed to create particle Geometry shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();

	return true;
}

bool ParticleHandler::setUpInputLayout(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[1] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, std::size(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

	return !FAILED(hr);
}

bool ParticleHandler::setUpConstBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(wrlMtx);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, 0, &constBuffer);

	if (FAILED(hr)) return false;
	this->updateWorldMatrix(0);


	D3D11_BUFFER_DESC newBufferDesc = {};
	newBufferDesc.ByteWidth = sizeof(timer);
	newBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	newBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	newBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	newBufferDesc.MiscFlags = 0;
	newBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&bufferDesc, 0, &timerBuffer);
	return !FAILED(hr);
}

void ParticleHandler::updateTimer()
{
	D3D11_MAPPED_SUBRESOURCE mapRes;
	ZeroMemory(&mapRes, sizeof(timer));
	HRESULT hr = immediateContext->Map(timerBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapRes);
	memcpy(mapRes.pData, &timer, sizeof(timer));
	immediateContext->Unmap(timerBuffer, 0);
}

bool ParticleHandler::setUpVertexBuffer(ID3D11Device* device)
{
	//Setup description
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT3) * this->particles.size();
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	stride = sizeof(DirectX::XMFLOAT3);
	offset = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = particles.data();

	if ((FAILED(device->CreateBuffer(&bufferDesc, &data, &vBuffer)))) return false;
	return true;
}

ParticleHandler::ParticleHandler()
	:cameraPtr(nullptr), immediateContext(nullptr), constBuffer(nullptr), inputLayout(nullptr), offset(0), stride(0), pVertex(nullptr),
	pGeometry(nullptr), pPixel(nullptr), pCompute(nullptr), particleUAV(nullptr), timerBuffer(nullptr), vBuffer(nullptr), 
	worldMatrix(DirectX::XMMatrixIdentity()), wrlMtx(XMFLOAT4X4()), vShaderByteCode("")
{
	timer.time = 0;
	timer.particlesPerThread = 10;
	for (int i = 0; i < AMOUNT_OF_PARTICLES; i++) //Creates the "particles"
	{
		particles.push_back({ (float)cos(i + 1) / (float)AMOUNT_OF_PARTICLES, 5.0f, (float)sin(i + 1) / (float)AMOUNT_OF_PARTICLES });
	}
}

ParticleHandler::~ParticleHandler()
{
	inputLayout->Release();
	particleUAV->Release();

	//Buffers
	vBuffer->Release();
	constBuffer->Release();
	timerBuffer->Release();

	//Shaders
	pVertex->Release();
	pGeometry->Release();
	pPixel->Release();
	pCompute->Release();
}

bool ParticleHandler::InitiateHandler(ID3D11DeviceContext* immediateContext, ID3D11Device* device, Camera* camera)
{
	this->immediateContext = immediateContext;
	if ((this->immediateContext) == nullptr)		return false;
	if ((this->cameraPtr = camera) == nullptr)		return false;
	if (!this->setUpShaders(device))				return false;
	if (!this->setUpInputLayout(device))			return false;
	if (!this->setUpVertexBuffer(device))			return false;
	if (!this->setUpConstBuffer(device))			return false;
	if (!this->setUpUAV(device))					return false;
	return true; //If everything was setup correctly
}

void ParticleHandler::drawParticles()
{
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->VSSetShader(pVertex, nullptr, 0);
	immediateContext->GSSetShader(pGeometry, nullptr, 0);
	immediateContext->PSSetShader(pPixel, nullptr, 0);
	
	cameraPtr->sendGeometryMatrix(immediateContext);
	cameraPtr->sendVectorsGeometry(immediateContext);
	immediateContext->VSSetConstantBuffers(0, 1, &constBuffer);	

	immediateContext->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);

	immediateContext->Draw(AMOUNT_OF_PARTICLES,0);

	//Set everything back to nullptr
	immediateContext->VSSetShader(nullptr, nullptr, 0);
	immediateContext->GSSetShader(nullptr, nullptr, 0);
	immediateContext->PSSetShader(nullptr, nullptr, 0);
}

void ParticleHandler::updateParticles()
{
	//Setup
	immediateContext->CSSetShader(pCompute, nullptr, 0);
	immediateContext->CSSetUnorderedAccessViews(0, 1, &particleUAV, nullptr);
	
	timer.time += 1.0f / 144.0f;
	if (timer.time >= 10.0f)
	{
		timer.time = 0;
	}
	this->updateTimer();
	immediateContext->CSSetConstantBuffers(0, 1, &timerBuffer);

	//Dispatch
	immediateContext->Dispatch(AMOUNT_OF_PARTICLES / (ThreadsPerGroup*timer.particlesPerThread), 1, 1);
}

