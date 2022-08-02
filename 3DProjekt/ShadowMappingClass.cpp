#include "ShadowMappingClass.h"

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

    hr = device->CreateShaderResourceView(tempTexture, NULL, &srv);

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
	if (this->immediateContext == nullptr)	return false;
	if (!this->setUpDepthStencilAndSRV(device))	return false;
	for (int i = 0; i < LIGHTAMOUNT; i++) { if (!cameras[i].CreateCBuffer(immediateContext, device)) return false; }
	return true;
}

void ShadowMappingClass::firstPass(std::vector<SceneObject*> objects)
{
	immediateContext->OMSetRenderTargets(0, nullptr, dsView);
	immediateContext->PSSetSamplers(0, 1, &shadowSampler);
	immediateContext->VSSetShader(shadowVertex, nullptr, 0);

	//Texture2D depthTexture
}
