#include "FrustumCuller.h"

bool FrustumCuller::objectExists(std::vector<SceneObject*> objects, SceneObject* obj)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i] == obj) return true;
	}
	return false;
}

void FrustumCuller::recurve(Node* node, DirectX::BoundingFrustum frustum, std::vector<SceneObject*>& objects)
{
	if (node->children[0] != nullptr)
	{
		for (int i = 0; i < CHILDRENAMOUNT; i++)
		{
			recurve(node->children[i], frustum, objects);
		}
	}
	else
	{
		if (frustum.Intersects(node->boundingBox))
		{
			for (int i = 0; i < node->objects.size(); i++)
			{
				objects.push_back(node->objects[i]);
				//if (!objectExists(objects, node->objects[i])) objects.push_back(node->objects[i]);
			}
		}
	}
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
		/*if (k == 0 || k == 4) { topLeft = DirectX::XMFLOAT3(topLeft.x, topLeft.y, topLeft.z);						bottomRight = DirectX::XMFLOAT3(bottomRight.x / 2, bottomRight.y, bottomRight.z / 2); }
		if (k == 1 || k == 5) { topLeft = DirectX::XMFLOAT3(bottomRight.x / 2, topLeft.y, topLeft.z);				bottomRight = DirectX::XMFLOAT3(bottomRight.x, bottomRight.y, bottomRight.z / 2); }
		if (k == 2 || k == 6) { topLeft = DirectX::XMFLOAT3(topLeft.x, topLeft.y, bottomRight.z / 2);				bottomRight = DirectX::XMFLOAT3(bottomRight.x / 2, bottomRight.y, bottomRight.z); }
		if (k == 3 || k == 7) { topLeft = DirectX::XMFLOAT3(bottomRight.x / 2, topLeft.y, bottomRight.z / 2);		bottomRight = DirectX::XMFLOAT3(bottomRight.x, bottomRight.y, bottomRight.z); }
		if (k >= 4) { topLeft.y = bottomRight.y; bottomRight.y *= 2; }*/
		if (k == 0 || k == 4) topLeft = DirectX::XMFLOAT3(topLeft.x, topLeft.y, topLeft.z);						
		if (k == 1 || k == 5) topLeft = DirectX::XMFLOAT3(topLeft.x + size, topLeft.y, topLeft.z);
		if (k == 2 || k == 6) topLeft = DirectX::XMFLOAT3(topLeft.x, topLeft.y, topLeft.z + size);
		if (k == 3 || k == 7) topLeft = DirectX::XMFLOAT3(topLeft.x + size, topLeft.y, topLeft.z + size);	
		bottomRight = DirectX::XMFLOAT3(topLeft.x + size, bottomRight.y, topLeft.z + size);
		if (k >= 4) { topLeft.y = bottomRight.y; bottomRight.y *= 2; }
		this->makeBondingBoxForNode(newChild, topLeft, bottomRight);
		parent->children[k] = newChild;
		if (i+1 < DEPTH) this->recursiveNodeFunction(newChild, topLeft, bottomRight, i+1);
	}
}

void FrustumCuller::revursiveIntersect(Node* node, DirectX::BoundingFrustum frustum, std::vector<SceneObject*> &objects)
{
	for (int i = 0; i < CHILDRENAMOUNT; i++)	//Loop through all children and see if they intersect with the objects
	{
		Node* child = node->children[i];
		if (child->children[0] != nullptr) revursiveIntersect(child, frustum, objects);
		else
		{
			if (frustum.Contains(child->boundingBox))
			{
				for (int j = 0; j < child->objects.size(); j++)
				{
					if (!this->objectExists(objects, child->objects[j]))
					{
						objects.push_back(child->objects[j]); //Add the object(s)'
					}
				}
				return;
			}
		}
	}

	////First we divide into sub-nodes since we start with parent node
	//for (int i = 0; i < CHILDRENAMOUNT; i++)
	//{
	//	Node* child = node->children[i];
	//	if (frustum.Contains(child->boundingBox))//.Intersects(frustum)) //If the node collided with the frustum
	//	{
	//		if (child->children[0] != nullptr)	//If it is not a leaf node continue down the quad tree
	//		{
	//			this->revursiveIntersect(child, frustum, objects);
	//		}
	//		else    //If the node is a leaf we check whether or not to add the object(s)
	//		{
	//			for (int j = 0; j < child->objects.size(); j++)
	//			{
	//				if (!this->objectExists(objects, child->objects[j]))
	//				{
	//					objects.push_back(child->objects[j]); //Add the object(s)'
	//				}
	//			}
	//			return;
	//			//End the function
	//		}
	//	}
	//	//else return;
	//	//This node did not collide with the frustum
	//}
}

void FrustumCuller::addObjectBB(Node* node, std::vector<SceneObject> *objects)
{
	//for (int i = 0; i < CHILDRENAMOUNT; i++)	//Loop through all children and see if they intersect with the objects
	//{
	//	Node* child = node->children[i];
	//	for (int j = 0; j < objects->size(); j++)
	//	{
	//		if (objects->at(j).getBB().Intersects(child->boundingBox))//child->boundingBox.Intersects(objects->at(j).getBB()))
	//		{
	//			if (child->children[0] != nullptr) addObjectBB(child, objects);	//If it is not a leaf node
	//			else 
	//			{ 
	//				child->objects.push_back(&objects->at(j));
	//			}
	//		}
	//	}
	//}

	for (int j = 0; j < objects->size(); j++)
	{
		for (int i = 0; i < CHILDRENAMOUNT; i++)	//Loop through all children and see if they intersect with the objects
		{
			Node* child = node->children[i];
			if (objects->at(j).getBB().Intersects(child->boundingBox))
			{
				if (child->children[0] != nullptr) addObjectBB(child, objects);	//If it is not a leaf node
				else
				{
					if (!objectExists(child->objects, &objects->at(j))) child->objects.push_back(&objects->at(j));
				}
			}
		}
	}

	//for (int i = 0; i < CHILDRENAMOUNT; i++)	//Loop through all children and see if they intersect with the objects
	//{
	//	Node* child = node->children[i];
	//	if (child->children[0] != nullptr) addObjectBB(child, objects);
	//	else
	//	{
	//		for (int j = 0; j < objects->size(); j++)
	//		{
	//			/*if ((child->boundingBox.Center.x + child->boundingBox.Extents.x > objects->at(j).getBB().Center.x - objects->at(j).getBB().Extents.x
	//				|| child->boundingBox.Center.x - child->boundingBox.Extents.x < objects->at(j).getBB().Center.x + objects->at(j).getBB().Extents.x)
	//				&& (child->boundingBox.Center.z + child->boundingBox.Extents.z > objects->at(j).getBB().Center.z - objects->at(j).getBB().Extents.z
	//					|| child->boundingBox.Center.z - child->boundingBox.Extents.z < objects->at(j).getBB().Center.z + objects->at(j).getBB().Extents.z))
	//			{
	//				child->objects.push_back(&objects->at(j));
	//			}*/
	//				if (child->boundingBox.Contains(objects->at(j).getBB()))
	//			{
	//				child->objects.push_back(&objects->at(j));
	//			}
	//		
	//		}
	//	}
	//}
}

void FrustumCuller::newRecursiveIntersect(Node* node, DirectX::BoundingFrustum frustum, std::vector<SceneObject*>& objects)
{
	if (frustum.Intersects(node->boundingBox))
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
				newRecursiveIntersect(node->children[i], frustum, objects);
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

bool FrustumCuller::initiateCuller(std::vector<SceneObject> *objects, int width, int height)
{
	Node* rootNode = new Node();
	nodes.push_back(rootNode);
	DirectX::XMFLOAT3 topLeft(-width, -50, -height);
	DirectX::XMFLOAT3 bottomRight(width, 50, height);
	this->recursiveNodeFunction(rootNode, topLeft, bottomRight, 0);
	this->addObjectBB(nodes[0], objects);
	return true;
}

std::vector<SceneObject*> FrustumCuller::cullObjects(DirectX::BoundingFrustum frustum)
{
	std::vector<SceneObject*> objects;
	for (int i = 0; i < CHILDRENAMOUNT; i++)
	{
		this->newRecursiveIntersect(nodes[0]->children[i], frustum, objects);
	}
	return objects;
}
