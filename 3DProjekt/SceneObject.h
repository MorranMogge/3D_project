#pragma once
#include <d3d11.h>
#include <vector>
#include <array>
#include "ObjParser.h"
#include <DirectXCollision.h>

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

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 rot;
	DirectX::XMFLOAT3 scale;

	//No need to send this in draw argument anymore
	ID3D11DeviceContext* immediateContext;

	ID3D11Buffer* constantBuffer;
	ID3D11Buffer* indexBuffer;
	std::vector<ID3D11Buffer*> matBuffer;


	DirectX::XMMATRIX worldMatrix;
	DirectX::XMFLOAT4X4 wrlMtx;

	//Vertices and texture
	std::vector<int*> indexes;
	std::vector<SimpleVertex>* vertices;
	ID3D11Buffer* vertexBuffer;
	std::vector<ID3D11ShaderResourceView*> textureSrv;
	std::vector<DWORD> *indices;
	std::vector<int*> verticeCount;

	void updateConstantBuffer();
	void updateWorldMatrix();
	bool setVertices(std::vector<SimpleVertex>* inVertices);
	bool setMatBuffer(ID3D11Device* device);
	bool setIndices(std::vector<DWORD>* indices);
	bool setImmediateContext(ID3D11DeviceContext* immediateContext);
	bool createIndexBuffer(ID3D11Device* device);
	bool createConstBuf(ID3D11Device* device);

public:
	bool setVertexBuffer(ID3D11Device* device);
	SceneObject(objectInfo&inObj);
	~SceneObject();
	bool initiateObject(ID3D11DeviceContext* immediateContext, ID3D11Device* device, std::vector<SimpleVertex>* inVertices, std::vector<DWORD>* indices);
	void draw(int submeshAmount = 0);
	
	void setBoundingBox();

	bool setTextureSrv(ID3D11ShaderResourceView* &texture);

    int getVerticeAmount() const;
    DirectX::XMMATRIX getWorldMatrix() const;
	ID3D11Buffer* getVertexBuffer();
	void setWorldPos(float arr[]);
	void setWorldPos(float x, float y, float z);
	void setRot(float arr[]);
	void setRot(DirectX::XMFLOAT3 newRot);
	void setScale(float arr[]);
	void setScale(float x, float y, float z);
	void setWorldPos(DirectX::XMFLOAT3 newPos);
	DirectX::BoundingBox getBB()const;
};