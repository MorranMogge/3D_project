#include "ShadowMappingClass.h"

bool ShadowMappingClass::setUpShaders(ID3D11Device* device)
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
    textureDesc.ArraySize = 1;
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
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    depthStencilViewDesc.Flags = 0;

    hr = device->CreateDepthStencilView(tempTexture, &depthStencilViewDesc, &dsView);

    if (FAILED(hr)) return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(tempTexture, &srvDesc, &srv);

	return true;
}

ShadowMappingClass::ShadowMappingClass()
{
}

ShadowMappingClass::~ShadowMappingClass()
{
	dsView->Release();
	srv->Release();
}

bool ShadowMappingClass::initiateShadowMapping(ID3D11DeviceContext* immediateContext, ID3D11Device* device)
{
	this->immediateContext = immediateContext;
	if (this->immediateContext == nullptr)	    return false;
	if (!this->setUpDepthStencilAndSRV(device))	return false;
    if (!this->setUpShaders(device))            return false;
	for (int i = 0; i < LIGHTAMOUNT; i++) { if (!cameras[i].CreateCBuffer(immediateContext, device)) return false; }
    cameras[0].SetPosition(0, 50, 0);
    cameras[0].SetRotation(XM_PI / 2, 0, 0, immediateContext);
	return true;
}

void ShadowMappingClass::firstPass(std::vector<SceneObject> objects)
{
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->OMSetRenderTargets(0, nullptr, dsView);
    cameras[0].sendView(immediateContext);
	//immediateContext->PSSetSamplers(0, 1, &shadowSampler);
	immediateContext->VSSetShader(shadowVertex, nullptr, 0);
    immediateContext->PSSetShader(nullptr, nullptr, 0);
    
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i].draw(false);
    }

	//Texture2D depthTexture
}
