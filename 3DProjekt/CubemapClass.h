#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Camera.h"

#define NINETYDEGREES DirectX::XM_PI/2
#define CUBEMAPSIZE 1024

class CubemapClass
{
private:
	//Given pointer to
	ID3D11DeviceContext* immediateContext;

	Camera cam;
	
	ID3D11ShaderResourceView* srv;	//Need to be released
	ID3D11RenderTargetView* rtv[6];		//Need to be released

	bool setUpSRVAndRTV(ID3D11Device* device);

public:
	CubemapClass();
	~CubemapClass();

	bool initiateCubemap(ID3D11DeviceContext* immediateContext, ID3D11Device* device);


};

