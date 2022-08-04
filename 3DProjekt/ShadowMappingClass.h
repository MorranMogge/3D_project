#pragma once
#include "Camera.h"
#include "SceneObject.h"
#include <vector>

#define SHADOWMAPRESOLUTION 1024
#define LIGHTAMOUNT 1

class ShadowMappingClass
{
private:
	Camera cameras[LIGHTAMOUNT];

	ID3D11DeviceContext* immediateContext;

	ID3D11DepthStencilView* dsView;
	ID3D11ShaderResourceView* srv;
	ID3D11VertexShader* shadowVertex;
	ID3D11SamplerState* shadowSampler;
	ID3D11InputLayout* inputLayout;


	bool setUpShaders(ID3D11Device* device, std::string& vShaderByteCode);
	bool setUpDepthStencilAndSRV(ID3D11Device* device);
	bool setUpInputLayout(ID3D11Device* device, const std::string& vShaderByteCode);
	bool setUpSamplerState(ID3D11Device* device);

public:
	ShadowMappingClass();
	~ShadowMappingClass();
	bool initiateShadowMapping(ID3D11DeviceContext* immediateContext, ID3D11Device* device);
	void firstPass(std::vector<SceneObject> objects);
	void firstPass(std::vector<SceneObject*> objects);
	void secondPass(int index = 2);
	void clearSecondPass();
};

