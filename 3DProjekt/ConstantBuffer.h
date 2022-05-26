#pragma once
#include <DirectXMath.h>
#include <d3d11.h>


using namespace DirectX;

struct constantBuf
{
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 projectionMatrix;
};

class ConstantBuffer
{
private:
	XMMATRIX world;
	XMMATRIX viewAndProj;
	constantBuf matrices;
public:
	ConstantBuffer();
	void NewUpdate(float rotation = 1.0f);
	void Update(ID3D11DeviceContext* immediateContext, ID3D11Buffer*& constantBuffer);

	bool setConstBuf(ID3D11Device* device, ID3D11Buffer*& constantBuffer);

	void setXValue(float xPos);
	void setYValue(float yPos);
	void setZValue(float zPos);

};