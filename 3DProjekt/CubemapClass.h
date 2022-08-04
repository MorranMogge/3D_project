#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Camera.h"
#include "SceneObject.h"
#include "ParticleHandler.h"
#include "PipelineHelper.h"

#define NINETYDEGREES DirectX::XM_PI/2
#define CUBEMAPRESOLUTION 1024
#define AMOUNTOFSIDESACUBEHAS 6

struct cubeInfo
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
};

class CubemapClass
{
private:
	float timer;
	float rot[3]{ 0.0f, 0.0f, 0.0f };

	UINT stride;
	UINT offset;

	//Given pointer to
	ID3D11DeviceContext* immediateContext;

	Camera cam;
	CamData camData;
	
	ID3D11PixelShader* pShader;								//Need to be released
	ID3D11ShaderResourceView* srv;							//Need to be released
	ID3D11Texture2D* dsTexture;								//Need to be released
	ID3D11Buffer* camBuffer;								//Need to be released
	ID3D11DepthStencilView* dsView;							//Need to be released
	ID3D11RenderTargetView* rtv[AMOUNTOFSIDESACUBEHAS];		//Need to be released

	ID3D11VertexShader* cubeVshader;
	ID3D11PixelShader* cubePshader;
	ID3D11InputLayout* cubeInputLayout;
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* constBuf;

	DirectX::XMMATRIX worldMtx;
	DirectX::XMFLOAT4X4 wrlMtx;
	std::vector<cubeInfo> cube;

	void setRot(int index);
	void updateWrldMtx();
	void updateCamBuffer();
	bool setUpSRVAndRTV(ID3D11Device* device);
	bool setUpShaders(ID3D11Device* device, std::string& vShaderByteCode);
	bool setUpCamBuffer(ID3D11Device* device);
	bool setUpDepthStencil(ID3D11Device* device);
	bool setUpVertexBuffer(ID3D11Device* device);
	bool setUpInputLayout(ID3D11Device* device, const std::string& vShaderByteCode);
	bool setUpConstBuf(ID3D11Device* device);

public:
	CubemapClass();
	~CubemapClass();

	bool initiateCubemap(ID3D11DeviceContext* immediateContext, ID3D11Device* device, ID3D11DepthStencilView* dsView);

	void createCube(newObjThing vertices);
	void draw(std::vector<SceneObject>& o, ParticleHandler& pHandler, ID3D11DepthStencilView* &view);
	void drawCube();

};

