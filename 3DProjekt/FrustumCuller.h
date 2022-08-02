#pragma once
#include "SceneObject.h"
#include <vector>
#include <d3d11.h>
#include <DirectXCollision.h>

#define ITERATIONS 4
#define CHILDRENAMOUNT 4

struct Node	//Root node fits whole scene
{
	Node* children[CHILDRENAMOUNT]{nullptr};
	DirectX::BoundingBox boundingBox;
	std::vector<SceneObject*> objects;
	Node()
	{
		for (int i = 0; i < CHILDRENAMOUNT; i++)
		{
			children[i] = nullptr;
		}
	};
};



class FrustumCuller
{
private:
	std::vector<Node*> nodes;
	void makeBondingBoxForNode(Node* node, const DirectX::XMFLOAT3 topLeft, const DirectX::XMFLOAT3 bottomRight);
	void recursiveNodeFunction(Node* &parent, DirectX::XMFLOAT3 topLeft, DirectX::XMFLOAT3 bottomRight, int i);
	void revursiveIntersect(Node* node, DirectX::BoundingFrustum frustum, std::vector<SceneObject*>& objects);
	void addObjectBB(Node* node, std::vector<SceneObject>* objects);
	bool objectExists(std::vector<SceneObject*> objects, SceneObject* obj);

public:
	FrustumCuller();
	~FrustumCuller();
	bool initiateCuller(std::vector<SceneObject> *objects, int width, int height);
	std::vector<SceneObject*> cullObjects(DirectX::BoundingFrustum frustum);
};