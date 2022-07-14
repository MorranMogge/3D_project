#pragma once
#include <d3d11.h>

#include "Camera.h"

class TesselatorClass
{
private:
	//Given pointer to
	ID3D11DeviceContext* immediateContext;
	Camera* cameraPtr;

	//Need to release
	ID3D11RasterizerState* currentRasterizer;
	ID3D11RasterizerState* rasterizerState;
	ID3D11RasterizerState* wireFrameRasterizerState;
	ID3D11HullShader* hullShader;					
	ID3D11DomainShader* domainShader;	
	ID3D11Buffer* cameraBuffer;

	bool setUpCameraBuffer(ID3D11Device* device);
	bool setUpShaders(ID3D11Device* device);
	bool setUpRasterizerState(ID3D11Device* device);
public:
	TesselatorClass();
	~TesselatorClass();
	bool setUpTesselator(ID3D11DeviceContext* immediateContext, ID3D11Device* device, Camera* camera);
	void setRasterizerState();
	void setWireFrame(float wireframe);



};