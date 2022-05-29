#include "TextureLoader.h"
#include "memoryLeakChecker.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


bool LoadTexutres(ID3D11Device* device, std::vector<ID3D11ShaderResourceView*>& textureSrvs)
{
	std::ifstream file("textureFile.txt");
	if (!file.is_open()) { std::cout << "Could not open textureFile!\n"; return 0; }
	std::string textureName;
	while (std::getline(file, textureName))
	{
		ID3D11ShaderResourceView* newSrv;
		if (!createSRVforPic(device, newSrv, textureName)) return 0;
		textureSrvs.push_back(newSrv);
	}
	file.close();
	return 1;
}

bool createSRVforPic(ID3D11Device* device, ID3D11ShaderResourceView*& srv, std::string fileName)
{
	int width, height, channel;
	unsigned char* picture = stbi_load(fileName.c_str(), &width, &height, &channel, STBI_rgb_alpha);

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
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));

	subResource.pSysMem = picture;
	subResource.SysMemPitch = (UINT)width * 4;

	ID3D11Texture2D* texture = {};
	HRESULT hr = device->CreateTexture2D(&textureDesc, &subResource, &texture);
	if (FAILED(hr)) { std::cout << "Could not create Texture2D!\n"; return 1; }

	if (texture != nullptr)
	{
		hr = device->CreateShaderResourceView(texture, nullptr, &srv);
		if (FAILED(hr)) { std::cout << "Could not create srv!\n"; return 1; }
	}

	stbi_image_free(picture);
	texture->Release();
	return !FAILED(hr);
}