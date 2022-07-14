#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

struct ImGuiValues
{
	float imposition;
	float imnormal;
	float imcolour;
	float imwireframe;
};

struct CamData
{
	DirectX::XMFLOAT3 cameraPosition;
	float tesselationConst;
};

//With this we decide the amount of gBuffers
const int G_BUFFER_SIZE = 4;

class DeferredRenderer
{
private:
	int width;
	int height;

	ID3D11ComputeShader* cShader;

	ID3D11UnorderedAccessView* uaView;
	ID3D11Buffer* buffers[G_BUFFER_SIZE];
	ID3D11ShaderResourceView* srvs[G_BUFFER_SIZE];
	ID3D11RenderTargetView* rtvs[G_BUFFER_SIZE];

public:
	DeferredRenderer(int width, int height);
	~DeferredRenderer();

	bool setComputeShader(ID3D11ComputeShader* cShader);
	bool initiateDeferredRenderer();
	bool setEverything(ID3D11UnorderedAccessView* uaView, ID3D11Buffer* buffers[], ID3D11ShaderResourceView* srvs[], ID3D11RenderTargetView* rtvs[]);

	void updateBuffers(CamData camData, ImGuiValues imGuiStuff);
	void ComputeShaderPass();
};
