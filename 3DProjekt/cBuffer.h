#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

using namespace DirectX;

struct constantBuffs
{
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 projectionMatrix;
};

class CBuffer
{
private:
	XMMATRIX world;
	XMMATRIX viewAndProj;
	constantBuffs matrices;
public:
	CBuffer();
	void setNewBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer*& constantBuffer, XMMATRIX objWorldMatrix);
	bool setConstBuf(ID3D11Device* device, ID3D11Buffer*& constantBuffer);

};