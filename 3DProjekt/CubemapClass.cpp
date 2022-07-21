#include "CubemapClass.h"

bool CubemapClass::setUpSRVAndRTV(ID3D11Device* device)
{
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = CUBEMAPSIZE;
    desc.Height = CUBEMAPSIZE;
    desc.ArraySize = 6;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    //Order Matters
    //0: +x     1:  -x
    //2: +y     3:  -y
    //4: +z     5:  -z

    ID3D11Texture2D* tempTexture;
    HRESULT hr = device->CreateTexture2D(&desc, NULL, &tempTexture);
    if (FAILED(hr)) std::cout << "Failed to create cube map texture2D\n"; return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

    hr = device->CreateShaderResourceView(tempTexture, &srvDesc, &srv);
    if (FAILED(hr)) std::cout << "Failed to create cube map srv\n"; return false;

    for (int i = 0; i < desc.ArraySize; i++)
    {
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
        rtvDesc.Format = desc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
        rtvDesc.Texture2DArray =
        {
        rtvDesc.Texture2DArray.FirstArraySlice = (UINT)i,
        rtvDesc.Texture2DArray.ArraySize = 1
        };

        hr = device->CreateRenderTargetView(tempTexture, &rtvDesc, &rtv[6]);
        if (FAILED(hr)) std::cout << "Failed to create cube map rtv\n"; return false;
    }

	return true;
}

CubemapClass::CubemapClass()
{
}

CubemapClass::~CubemapClass()
{
    srv->Release();
    for (int i = 0; i < std::size(rtv); i++)
    {
        rtv[i]->Release();
    }
}

bool CubemapClass::initiateCubemap(ID3D11DeviceContext* immediateContext, ID3D11Device* device)
{
    this->immediateContext = immediateContext;
    if ((this->immediateContext) == nullptr)		return false;
    if (!this->setUpSRVAndRTV(device))              return false;
	return true;
}
