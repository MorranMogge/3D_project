#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
#include <array>

#include "ObjParser.h"

//struct Vertex
//{
//	float pos[3];
//	float n[3];
//	float uv[2];
//
//	Vertex(const std::array<float, 3>& position, const std::array<float, 3> normal, const std::array<float, 2>& UV)
//	{
//		for (int i = 0; i < 3; i++)
//		{
//			pos[i] = position[i];
//			n[i] = normal[i];
//
//		}
//		for (int i = 0; i < 2; i++)
//		{
//			uv[i] = UV[i];
//		}
//	}
//};


class SceneObject
{
private:
	UINT stride;
	UINT offset;    

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 rot;
	DirectX::XMFLOAT3 scale;

	//No need to send this in draw argument anymore
	ID3D11DeviceContext* immediateContext;

	//Position
	ID3D11Buffer* constantBuffer;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMFLOAT4X4 wrlMtx;

	//Vertices and texture
	std::vector<SimpleVertex>* vertices;
	ID3D11Buffer* vertexBuffer;
	ID3D11ShaderResourceView* textureSrv;

	void updateConstantBuffer();
	void updateWorldMatrix();
public:
	SceneObject(std::vector<SimpleVertex>* inVertices);
	SceneObject();
	~SceneObject();
	bool setImmediateContext(ID3D11DeviceContext* immediateContext);
	void draw();
	void setVertices(objThing obj);

	bool setVertexBuffer(ID3D11Device* device);
	bool setTextureSrv(ID3D11ShaderResourceView* &texture);
	bool createConstBuf(ID3D11Device* device);

	void releaseCom();
    int getVerticeAmount() const;
    DirectX::XMMATRIX getWorldMatrix() const;
	ID3D11Buffer* getVertexBuffer();
	void setWorldPos(float arr[]);
	void setRot(float arr[]);
	void setScale(float arr[]);

};