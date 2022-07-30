#pragma once
#include "SceneObject.h"
#include <vector>
#include <d3d11.h>
#include <DirectXCollision.h>

#define ITERATIONS 1
#define CHILDRENAMOUNT 4

struct Node	//Root node fits whole scene
{
	Node* children[4]{nullptr};
	DirectX::BoundingBox boundingBox;
	Node()
	{
		for (int i = 0; i < CHILDRENAMOUNT; i++)
		{
			children[i] = nullptr;
		}
	};
};

struct Leaf
{

};

class FrustumCuller
{
private:
	std::vector<Node*> nodes;
	void makeBondingBoxForNode(Node* node, const DirectX::XMFLOAT3 topLeft, const DirectX::XMFLOAT3 bottomRight);
	void recursiveNodeFunction(Node* &parent, DirectX::XMFLOAT3 topLeft, DirectX::XMFLOAT3 bottomRight, int i);

	int width;
	int height;

public:
	FrustumCuller();
	~FrustumCuller();
	bool initiateCuller(int width, int height);
	std::vector<SceneObject>* cullObjects();
};