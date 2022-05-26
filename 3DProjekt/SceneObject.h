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
    std::vector<SimpleVertex> vertices;
	SimpleVertex* testVertices;
    DirectX::XMMATRIX worldMatrix;
	ID3D11Buffer* vertexBuffer;

public:
	SceneObject(std::vector<SimpleVertex>* inVertices);
	SceneObject();
	~SceneObject();
	void Draw();
	void setVertices(objThing obj);

	bool setVertexBuffer(ID3D11Device* device);

    int getVerticeAmount() const;
    DirectX::XMMATRIX getWorldMatrix() const;
	ID3D11Buffer* getVertexBuffer();
	void setWorldPos(float* arr);
	void setRot(float* arr);
	void setScale(float* arr);

};