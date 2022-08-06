#include "FrustumCuller.h"

bool FrustumCuller::objectExists(std::vector<SceneObject*> objects, SceneObject* obj)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i] == obj) return true;
	}
	return false;
}

void FrustumCuller::makeBondingBoxForNode(Node* node, const DirectX::XMFLOAT3 topLeft, const DirectX::XMFLOAT3 bottomRight)
{
	DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&topLeft);
	DirectX::XMVECTOR p2 = DirectX::XMLoadFloat3(&bottomRight);
	DirectX::BoundingBox::CreateFromPoints(node->boundingBox, p1, p2);
}

void FrustumCuller::recursiveNodeFunction(Node* &parent, DirectX::XMFLOAT3 topLeft, DirectX::XMFLOAT3 bottomRight, int i)
{
	DirectX::XMFLOAT3 inTopLeft = topLeft;
	DirectX::XMFLOAT3 inBottomRight = bottomRight;

	float size = abs(bottomRight.x - topLeft.x)/2;
	for (int k = 0; k < CHILDRENAMOUNT; k++)
	{
		topLeft = inTopLeft;
		bottomRight = inBottomRight;
		Node* newChild = new Node();
		nodes.push_back(newChild);

		if (k == 0) topLeft = DirectX::XMFLOAT3(topLeft.x, topLeft.y, topLeft.z);						
		if (k == 1) topLeft = DirectX::XMFLOAT3(topLeft.x + size, topLeft.y, topLeft.z);
		if (k == 2) topLeft = DirectX::XMFLOAT3(topLeft.x, topLeft.y, topLeft.z + size);
		if (k == 3) topLeft = DirectX::XMFLOAT3(topLeft.x + size, topLeft.y, topLeft.z + size);	
		bottomRight = DirectX::XMFLOAT3(topLeft.x + size, bottomRight.y, topLeft.z + size);
		this->makeBondingBoxForNode(newChild, topLeft, bottomRight);
		parent->children[k] = newChild;
		if (i+1 < DEPTH) this->recursiveNodeFunction(newChild, topLeft, bottomRight, i+1);
	}
}

void FrustumCuller::addObjectBB(Node* node, std::vector<SceneObject*> *objects)
{
	for (int j = 0; j < objects->size(); j++)
	{
		for (int i = 0; i < CHILDRENAMOUNT; i++)	//Loop through all children and see if they intersect with the objects
		{
			Node* child = node->children[i];
			if (child->boundingBox.Contains(objects->at(j)->getBB()) != 0)
			{
				if (child->children[0] != nullptr) addObjectBB(child, objects);	//If it is not a leaf node
				else
				{
					if (!objectExists(child->objects, objects->at(j))) child->objects.push_back(objects->at(j));
				}
			}
		}
	}
}

void FrustumCuller::recursiveIntersect(Node* node, DirectX::BoundingFrustum frustum, std::vector<SceneObject*>& objects)
{
	if (frustum.Contains(node->boundingBox) != 0)
	{
		if (node->children[0] == nullptr)
		{
			for (int i = 0; i < node->objects.size(); i++)
			{
				if (!objectExists(objects, node->objects[i])) objects.push_back(node->objects[i]);
			}
		}
		else
		{
			for (int i = 0; i < CHILDRENAMOUNT; i++)
			{
				recursiveIntersect(node->children[i], frustum, objects);
			}
		}
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

bool FrustumCuller::initiateCuller(std::vector<SceneObject*> *objects, int width, int height)
{
	Node* rootNode = new Node();
	nodes.push_back(rootNode);
	DirectX::XMFLOAT3 topLeft(-width, -50, -height);
	DirectX::XMFLOAT3 bottomRight(width, 50, height);
	this->rootNode = rootNode;
	this->recursiveNodeFunction(this->rootNode, topLeft, bottomRight, 0);
	this->addObjectBB(this->rootNode, objects);
	return true;
}

std::vector<SceneObject*> FrustumCuller::cullObjects(DirectX::BoundingFrustum frustum)
{
	std::vector<SceneObject*> objects;
	for (int i = 0; i < CHILDRENAMOUNT; i++)
	{
		this->recursiveIntersect(rootNode->children[i], frustum, objects);
	}
	return objects;
}
