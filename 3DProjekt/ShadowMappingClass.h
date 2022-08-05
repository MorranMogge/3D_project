#pragma once
#include "Camera.h"
#include "SceneObject.h"
#include <vector>

#define SHADOWMAPRESOLUTION 1024
#define LIGHTAMOUNT 4

struct SpotLight
{
	DirectX::XMFLOAT3 position;
	float cone;
	DirectX::XMFLOAT3 direction;
	float reach;
	DirectX::XMFLOAT3 colour;
	float padding1;
	DirectX::XMFLOAT3 attenuation;
	float padding2;
};

struct DirLight
{
	DirectX::XMFLOAT3 direction;
	float padding2;
	DirectX::XMFLOAT3 colour;
	float padding3;
};

class ShadowMappingClass
{
private:
	Camera cameras[LIGHTAMOUNT];

	ID3D11DeviceContext* immediateContext;

	ID3D11DepthStencilView* dsView[LIGHTAMOUNT];
	ID3D11ShaderResourceView* srv;
	ID3D11VertexShader* shadowVertex;
	ID3D11SamplerState* shadowSampler;
	ID3D11InputLayout* inputLayout;
	SpotLight spotLights[LIGHTAMOUNT-1];
	DirLight directionalLight;
	ID3D11Buffer* lightBuffers[LIGHTAMOUNT];

	bool setUpShaders(ID3D11Device* device, std::string& vShaderByteCode);
	bool setUpDepthStencilAndSRV(ID3D11Device* device);
	bool setUpInputLayout(ID3D11Device* device, const std::string& vShaderByteCode);
	bool setUpSamplerState(ID3D11Device* device);
	bool setUpLightBuffers(ID3D11Device* device);
	void setLightPosAndRot();

public:
	ShadowMappingClass();
	~ShadowMappingClass();
	bool initiateShadowMapping(ID3D11DeviceContext* immediateContext, ID3D11Device* device);
	void firstPass(std::vector<SceneObject> objects);
	void firstPass(std::vector<SceneObject*> objects);
	void secondPass(int index = 2);
	void preDispatch(int index = 2);
	void clearSecondPass();
};

