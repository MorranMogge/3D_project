#include "ShadowMappingClass.h"

bool ShadowMappingClass::setUpShaders(ID3D11Device* device, std::string& vShaderByteCode)
{
   
    std::string shaderData;
    std::ifstream reader;
    reader.open("../x64/Debug/ShadowVertexShader.cso", std::ios::binary | std::ios::ate);
    if (!reader.is_open())
    {
        std::cout << "Could not open pixel shader file!" << std::endl;
        return false;
    }

    reader.seekg(0, std::ios::end);
    shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
    reader.seekg(0, std::ios::beg);

    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &shadowVertex)))
    {
        std::cerr << "Failed to create pixel shader!" << std::endl;
        return false;
    }

    vShaderByteCode = shaderData;
    shaderData.clear();
    reader.close();

    return true;
}

bool ShadowMappingClass::setUpDepthStencilAndSRV(ID3D11Device* device)
{
    ID3D11Texture2D* tempTexture;
    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = SHADOWMAPRESOLUTION;
    textureDesc.Height = SHADOWMAPRESOLUTION;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = LIGHTAMOUNT;
    textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, &tempTexture);
    if (FAILED(hr)) return false;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    depthStencilViewDesc.Flags = 0;
    depthStencilViewDesc.Texture2DArray.ArraySize = 1;

    for (int i = 0; i < LIGHTAMOUNT; i++)
    {
        depthStencilViewDesc.Texture2DArray.FirstArraySlice = (UINT)i;
        hr = device->CreateDepthStencilView(tempTexture, &depthStencilViewDesc, &dsView[i]);
        if (FAILED(hr)) return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2DArray.ArraySize = LIGHTAMOUNT;
    hr = device->CreateShaderResourceView(tempTexture, &srvDesc, &srv);
    tempTexture->Release();

	return true;
}

bool ShadowMappingClass::setUpInputLayout(ID3D11Device* device, const std::string& vShaderByteCode)
{
    //We only need position since the first pass only saves depth
    D3D11_INPUT_ELEMENT_DESC inputDesc[1] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HRESULT hr = device->CreateInputLayout(inputDesc, std::size(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

    return !FAILED(hr);
}

bool ShadowMappingClass::setUpSamplerState(ID3D11Device* device)
{
    D3D11_SAMPLER_DESC samplerDesc = { };
    samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 0.0f;
    samplerDesc.BorderColor[1] = 0.0f;
    samplerDesc.BorderColor[2] = 0.0f;
    samplerDesc.BorderColor[3] = 0.0f;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&samplerDesc, &shadowSampler);
    return !FAILED(hr);
}

bool ShadowMappingClass::setUpLightBuffers(ID3D11Device* device)
{

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(DirLight);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = &directionalLight;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    ID3D11Buffer* tempBuffer;

    HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &lightBuffers[0]);
    if (FAILED(hr))
    {
        return false;
    }

    for (int i = 0; i < LIGHTAMOUNT-1; i++)
    {
        bufferDesc.ByteWidth = sizeof(SpotLight);
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        data.pSysMem = &spotLights[i];
        data.SysMemPitch = 0;
        data.SysMemSlicePitch = 0;


        hr = device->CreateBuffer(&bufferDesc, &data, &lightBuffers[i+1]);
        if (FAILED(hr))
        {
            return false;
        }
    }
    return true;
}

void ShadowMappingClass::setLightPosAndRot()
{
    cameras[0].SetPosition(0, 50, 0);
    cameras[0].SetRotation(XM_PI / 2, 0, 0, immediateContext);
    if (LIGHTAMOUNT < 2) return;
    cameras[1].SetPosition(0, 25, -50);
    cameras[1].SetRotation(XM_PI / 4, 0, 0, immediateContext);
    if (LIGHTAMOUNT < 3) return;
    cameras[2].SetPosition(0, 50, 0);
    cameras[2].SetRotation(XM_PI / 4, XM_PI / 4, 0, immediateContext);
    if (LIGHTAMOUNT < 4) return;
    cameras[3].SetPosition(0, 50, 0);
    cameras[3].SetRotation(XM_PI / 4, -XM_PI / 4, 0, immediateContext);
}

ShadowMappingClass::ShadowMappingClass()
{
    directionalLight.direction = DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f);
    directionalLight.colour = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

    spotLights[0].position = DirectX::XMFLOAT3(-10.f, 10.f, 10.f);
    spotLights[0].direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
    spotLights[0].colour = DirectX::XMFLOAT3(1.0f, 0.0f, 0.5f);
    spotLights[0].cone = 10.f;
    spotLights[0].reach = 50.f;
    spotLights[0].attenuation = DirectX::XMFLOAT3(0.2f, 0.4f, 0.6f);

    spotLights[1].position = DirectX::XMFLOAT3(-10.f, 25.f, 10.f);
    spotLights[1].direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
    spotLights[1].colour = DirectX::XMFLOAT3(0.0f, 1.0f, 0.5f);
    spotLights[1].cone = 10.f;
    spotLights[1].reach = 50.f;
    spotLights[1].attenuation = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

    spotLights[2].position = DirectX::XMFLOAT3(0.f, 10.f, -25.f);
    spotLights[2].direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
    spotLights[2].colour = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
    spotLights[2].cone = 10.f;
    spotLights[2].reach = 50.f;
    spotLights[2].attenuation = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
}

ShadowMappingClass::~ShadowMappingClass()
{
    for (int i = 0; i < LIGHTAMOUNT; i++) dsView[i]->Release();
	srv->Release();
    shadowSampler->Release();
    shadowVertex->Release();
    inputLayout->Release();
    //camBuffer->Release();
}

bool ShadowMappingClass::initiateShadowMapping(ID3D11DeviceContext* immediateContext, ID3D11Device* device)
{
    std::string vShaderByteCode;
	this->immediateContext = immediateContext;
	if (this->immediateContext == nullptr)	                return false;
	if (!this->setUpDepthStencilAndSRV(device))	            return false;
    if (!this->setUpShaders(device, vShaderByteCode))       return false;
    if (!this->setUpInputLayout(device, vShaderByteCode))   return false;
    if (!this->setUpSamplerState(device))                   return false;
    if (!this->setUpLightBuffers(device))                   return false;
	for (int i = 0; i < LIGHTAMOUNT; i++) { if (!cameras[i].CreateCBuffer(immediateContext, device)) return false; }
    this->setLightPosAndRot();
    return true;
}

void ShadowMappingClass::firstPass(std::vector<SceneObject> objects)
{
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediateContext->IASetInputLayout(inputLayout);
    immediateContext->VSSetShader(shadowVertex, nullptr, 0);
    cameras[0].sendView(immediateContext);

    for (int s = 0; s < LIGHTAMOUNT; s++)
    {
        immediateContext->ClearDepthStencilView(dsView[s], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
        immediateContext->OMSetRenderTargets(0, nullptr, dsView[s]);
        cameras[s].sendView(immediateContext);
        for (int i = 0; i < objects.size(); i++)
        {
            objects[i].draw(false);
        }

    }

    immediateContext->VSSetShader(nullptr, nullptr, 0);
}

void ShadowMappingClass::firstPass(std::vector<SceneObject*> objects)
{
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediateContext->IASetInputLayout(inputLayout);
    immediateContext->VSSetShader(shadowVertex, nullptr, 0);

    for (int s = 0; s < LIGHTAMOUNT; s++)
    {
        immediateContext->ClearDepthStencilView(dsView[s], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
        immediateContext->OMSetRenderTargets(0, nullptr, dsView[s]);
        cameras[s].sendView(immediateContext);
        for (int i = 0; i < objects.size(); i++)
        {
            objects[i]->draw(true);
        }

    }
   
    immediateContext->VSSetShader(nullptr, nullptr, 0);
}

void ShadowMappingClass::secondPass(int index)
{
    for (int i = 0; i < LIGHTAMOUNT; i++)
    {
        cameras[i].sendView(immediateContext, index+i);
    } 

    immediateContext->PSSetSamplers(1, 1, &shadowSampler);
    immediateContext->PSSetShaderResources(3, 1, &srv);
}

void ShadowMappingClass::preDispatch(int index)
{
    immediateContext->CSSetConstantBuffers(index, LIGHTAMOUNT, lightBuffers);
}

void ShadowMappingClass::clearSecondPass()
{
    ID3D11ShaderResourceView* nullSrv = nullptr;
    ID3D11SamplerState* nullSampler[2]{ nullptr };
    immediateContext->PSSetSamplers(0, 2, nullSampler);
    immediateContext->PSSetShaderResources(3, 1, &nullSrv);
}
