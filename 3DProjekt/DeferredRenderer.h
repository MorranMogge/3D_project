#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include "Camera.h"

struct ImGuiValues
{
	float imposition;
	float imnormal;
	float imcolour;
	float imwireframe;
};



//With this we decide the amount of gBuffers
#define G_BUFFER_SIZE 5
//const int G_BUFFER_SIZE = 5;

class DeferredRenderer
{
private:
	int width;
	int height;

	//Given pointer to
	ID3D11DeviceContext* immediateContext;
	ID3D11DepthStencilView* dsView;
	ImGuiValues* imGuiStruct;
	Camera* camPtr;

	ID3D11ComputeShader* cShader;
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11InputLayout* inputLayout;

	ID3D11UnorderedAccessView* uaView;
	//ID3D11Buffer* buffers[G_BUFFER_SIZE];
	ID3D11Texture2D* texture[G_BUFFER_SIZE];
	ID3D11ShaderResourceView* srv[G_BUFFER_SIZE];
	ID3D11RenderTargetView* rtv[G_BUFFER_SIZE];

	bool bindSrvAndRtv(ID3D11Device* device);
	bool setUpShaders(ID3D11Device* device, std::string& vShaderByteCode);
	bool setUpInputLayout(ID3D11Device* device, const std::string& vShaderByteCode);
	bool setUpUav(ID3D11Device* device, IDXGISwapChain* swapChain);

public:
	DeferredRenderer(int width, int height);
	~DeferredRenderer();

	bool setComputeShader(ID3D11ComputeShader* cShader);
	bool initiateDeferredRenderer(ID3D11DeviceContext* immediateContext, ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11DepthStencilView* dsView, Camera* camera, ImGuiValues* imGuiStruct);
	void firstPass();
	void secondPass();
};
