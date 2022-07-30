#include "FrustumCuller.h"

void FrustumCuller::makeBondingBoxForNode(Node* node, const DirectX::XMFLOAT3 topLeft, const DirectX::XMFLOAT3 bottomRight)
{
	DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&topLeft);
	DirectX::XMVECTOR p2 = DirectX::XMLoadFloat3(&bottomRight);
	DirectX::BoundingBox::CreateFromPoints(node->boundingBox, p1, p2);
}

void FrustumCuller::recursiveNodeFunction(Node* &parent, DirectX::XMFLOAT3 topLeft, DirectX::XMFLOAT3 bottomRight, int i)
{
	if (i >= ITERATIONS) return;
	for (int k = 0; k < CHILDRENAMOUNT; k++)
	{
		Node* newChild = new Node();
		nodes.push_back(newChild);
		if (k == 0) { topLeft = DirectX::XMFLOAT3(topLeft.x, topLeft.y, topLeft.z);						bottomRight = DirectX::XMFLOAT3(bottomRight.x / 2, bottomRight.y, bottomRight.z / 2); }
		if (k == 1) { topLeft = DirectX::XMFLOAT3(bottomRight.x / 2, topLeft.y, topLeft.z);				bottomRight = DirectX::XMFLOAT3(bottomRight.x, bottomRight.y, bottomRight.z / 2); }
		if (k == 2) { topLeft = DirectX::XMFLOAT3(topLeft.x, topLeft.y, bottomRight.z / 2);				bottomRight = DirectX::XMFLOAT3(bottomRight.x / 2, bottomRight.y, bottomRight.z); }
		if (k == 3) { topLeft = DirectX::XMFLOAT3(bottomRight.x / 2, topLeft.y, bottomRight.z / 2);		bottomRight = DirectX::XMFLOAT3(bottomRight.x, bottomRight.y, bottomRight.z); }
		this->makeBondingBoxForNode(newChild, topLeft, bottomRight);
		parent->children[k] = newChild;
		this->recursiveNodeFunction(newChild, topLeft, bottomRight, i+1);
	}
}

FrustumCuller::FrustumCuller()
{
}

FrustumCuller::~FrustumCuller()
{
	for (int i = nodes.size()-1; i >= 0; i--)
	{
		delete nodes[i];
		nodes.pop_back();
	}
}

bool FrustumCuller::initiateCuller(int width, int height)
{
	int tempWidth, tempHeight;
	int counter = 0;
	Node* currentParentNode = new Node();
	nodes.push_back(currentParentNode);
	DirectX::XMFLOAT3 topLeft(0, 100, 0);
	DirectX::XMFLOAT3 bottomRight(width, -100, height);
	this->makeBondingBoxForNode(currentParentNode, topLeft, bottomRight);
	this->recursiveNodeFunction(currentParentNode, topLeft, bottomRight, 0);
	
	return true;
}

std::vector<SceneObject>* FrustumCuller::cullObjects()
{
	for (int i = 0; i < ITERATIONS; i++)
	{

	}
	return nullptr;
}
