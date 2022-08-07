#pragma once
#include <DirectXMath.h>
#include <vector>
#include <d3d11.h>
#include <string>
#include <math.h>
#include <iostream>
#include "Camera.h"

struct camMtx
{
	DirectX::XMFLOAT4X4 worldView;
	DirectX::XMFLOAT4X4 proj;
};

struct particleAnimation
{
	float time;
	float particlesPerThread;
	float padding[2];
};

class ParticleHandler
{
private:
	const int AMOUNT_OF_PARTICLES = 1000;
	const int ThreadsPerGroup = 10;

	UINT stride;
	UINT offset;

	//Given pointer to
	ID3D11DeviceContext* immediateContext;
	Camera* cameraPtr;


	ID3D11InputLayout* inputLayout;
	ID3D11UnorderedAccessView* particleUAV;

	//Buffers
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* constBuffer;
	ID3D11Buffer* timerBuffer;
	
	//Shaders
	ID3D11VertexShader* pVertex;
	ID3D11GeometryShader* pGeometry;
	ID3D11PixelShader* pPixel;
	ID3D11ComputeShader* pCompute;

	//Static
	std::vector<DirectX::XMFLOAT3> particles;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMFLOAT4X4 wrlMtx;
	std::string vShaderByteCode;
	particleAnimation timer;
	

	void updateWorldMatrix(int index = 0);
	bool setUpShaders(ID3D11Device* device);
	bool setUpUAV(ID3D11Device* device);
	bool setUpVertexBuffer(ID3D11Device* device);
	bool setUpInputLayout(ID3D11Device* device);
	bool setUpConstBuffer(ID3D11Device* device);
	void updateTimer();
public:
	ParticleHandler();
	~ParticleHandler();
	bool InitiateHandler(ID3D11DeviceContext* immediateContext, ID3D11Device* device, Camera* camera);
	void drawParticles(Camera* cam = nullptr);
	void updateParticles();
};