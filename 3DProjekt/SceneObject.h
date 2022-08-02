#pragma once
#include <d3d11.h>
#include <vector>
#include <array>
#include "structs.h"
#include "ObjParser.h"
#include <DirectXCollision.h>

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


struct materialInfo
{
	float shinyness;
	float padding[3];

	materialInfo(float shiny)
	{
		shinyness = shiny;
		for (int i = 0; i < std::size(padding); i++)
		{
			padding[i] = 0.0f;
		}
	}
};

class SceneObject
{
private:
	UINT stride;
	UINT offset;

	DirectX::BoundingBox bb;
	std::vector<materialInfo> shinyness;
	std::vector<ID3D11Buffer*> matBuffer;

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
	std::vector<int> indexes;
	std::vector<SimpleVertex>* vertices;
	ID3D11Buffer* vertexBuffer;
	std::vector<ID3D11ShaderResourceView*> textureSrv;
	ID3D11Buffer* indexBuffer;
	std::vector<DWORD> *indices;
	std::vector<int> verticeCount;

	void updateConstantBuffer();
	void updateWorldMatrix();
public:
	SceneObject(std::vector<SimpleVertex>* inVertices);
	SceneObject(newObjThing inObj);
	SceneObject();
	~SceneObject();
	bool setImmediateContext(ID3D11DeviceContext* immediateContext);
	void initiateObject(ID3D11DeviceContext* immediateContext, ID3D11Device* device, std::vector<SimpleVertex>* inVertices, std::vector<DWORD>* indices);
	void draw();
	void draw(bool testDraw);
	void setVertices(objThing obj);
	void setVertices(std::vector<SimpleVertex>* inVertices);
	void setIndices(std::vector<DWORD>* indices);
	void setBoundingBox();

	bool setVertexBuffer(ID3D11Device* device);
	bool setMatBuffer(ID3D11Device* device);
	bool setTextureSrv(ID3D11ShaderResourceView* &texture);
	bool createConstBuf(ID3D11Device* device);
	bool createIndexBuffer(ID3D11Device* device);

	void releaseCom();
    int getVerticeAmount() const;
    DirectX::XMMATRIX getWorldMatrix() const;
	ID3D11Buffer* getVertexBuffer();
	void setWorldPos(float arr[]);
	void setWorldPos(float x, float y, float z);
	void setRot(float arr[]);
	void setRot(DirectX::XMFLOAT3 newRot);
	void setScale(float arr[]);
	void setWorldPos(DirectX::XMFLOAT3 newPos);
	DirectX::BoundingBox getBB()const;
};