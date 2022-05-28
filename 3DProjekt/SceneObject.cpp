#include "SceneObject.h"
#include "PipelineHelper.h"

SceneObject::SceneObject(std::vector<SimpleVertex> *inVertices)
{
	
	this->vertices = *inVertices;
	//this->vertices = new SimpleVertex[]
	//{
	//	//First triangle
	//	{ {-0.5f, 0.5f, 0.0f}, {0, 0, -1}, {0, 0} },
	//	{ {0.5f, -0.5f, 0.0f}, {0, 0, -1}, {1, 1} },
	//	{ {-0.5f, -0.5f, 0.0f}, {0, 0, -1}, {0, 1} },

	//	//Second triangle
	//	{ {-0.5f, 0.5f, 0.0f}, {0, 0, -1}, {0, 0} },
	//	{ {0.5f, 0.5f, 0.0f}, {0, 0, -1}, {1, 0} },
	//	{ {0.5f, -0.5f, 0.0f}, {0, 0, -1}, {1, 1} }
	//};
		

	worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix *= DirectX::XMMatrixTranslation(0.f, 0.f, -1.f);
	worldMatrix *= DirectX::XMMatrixRotationY(0);
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
}

SceneObject::SceneObject()
{
	//this->vertices = new std::vector<SimpleVertex>;
	/*this->vertices->push_back(SimpleVertex({ -10.0f, -0.35f, 10.0f }, { 0, 0, -1 }, { 0, 0 }));
	this->vertices->push_back(SimpleVertex({ 10.0, -0.5f, -10.0f }, { 0, 0, -1 }, { 1, 1 }));
	this->vertices->push_back(SimpleVertex({ -10.0, -0.5f, -10.0f }, { 0, 0, -1 }, { 0, 1 }));
	this->vertices->push_back(SimpleVertex({ -10.0f, -0.35f, 10.0f }, { 0, 0, -1 }, { 0, 0 }));
	this->vertices->push_back(SimpleVertex({ 10.0f, -0.35f, 10.0f }, { 0, 0, -1 }, { 1, 0 }));
	this->vertices->push_back(SimpleVertex({ 10.0f, -0.5f, -10.0f }, { 0, 0, -1 }, { 1, 1 }));*/

	
	this->vertices.push_back(SimpleVertex({ -10.0f, -0.35f, 10.0f }, { 0, 0, -1 }, { 0, 0 }));
	this->vertices.push_back(SimpleVertex({ 10.0, -0.5f, -10.0f }, { 0, 0, -1 }, { 1, 1 }));
	this->vertices.push_back(SimpleVertex({ -10.0, -0.5f, -10.0f }, { 0, 0, -1 }, { 0, 1 }));
	this->vertices.push_back(SimpleVertex({ -10.0f, -0.35f, 10.0f }, { 0, 0, -1 }, { 0, 0 }));
	this->vertices.push_back(SimpleVertex({ 10.0f, -0.35f, 10.0f }, { 0, 0, -1 }, { 1, 0 }));
	this->vertices.push_back(SimpleVertex({ 10.0f, -0.5f, -10.0f }, { 0, 0, -1 }, { 1, 1 }));

	//int yes = vertices.size();
	//SimpleVertex tempo[vertices.size()];
	//this->testVertices = this->vertices.data();
	//std::copy(this->vertices->begin(), this->vertices->end(), this->testVertices);
	this->testVertices = new SimpleVertex[]
		{
			//First triangle
			{ {-10.0f, -0.35f, 10.0f}, {0, 0, -1}, {0, 0} },
			{ {10.0, -0.5f, -10.0f}, {0, 0, -1}, {1, 1} },
			{ {-10.0, -0.5f, -10.0f}, {0, 0, -1}, {0, 1} },

			//Second triangle
			{ {-10.0f, -0.35f, 10.0f}, {0, 0, -1}, {0, 0} },
			{ {10.0f, -0.35f, 10.0f}, {0, 0, -1}, {1, 0} },
			{ {10.0f, -0.5f, -10.0f}, {0, 0, -1}, {1, 1} }
		};
	/*for (int i = 0; i < 6; i++)
	{
		this->testVertices[i] = &this->vertices[i];
	}*/
	//this->vertices = new SimpleVertex[]
	//{
	//	//First triangle
	//	{ {-10.0f, -0.35f, 10.0f}, {0, 0, -1}, {0, 0} },
	//	{ {10.0, -0.5f, -10.0f}, {0, 0, -1}, {1, 1} },
	//	{ {-10.0, -0.5f, -10.0f}, {0, 0, -1}, {0, 1} },

	//	//Second triangle
	//	{ {-10.0f, -0.35f, 10.0f}, {0, 0, -1}, {0, 0} },
	//	{ {10.0f, -0.35f, 10.0f}, {0, 0, -1}, {1, 0} },
	//	{ {10.0f, -0.5f, -10.0f}, {0, 0, -1}, {1, 1} }
	//};

	worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix *= DirectX::XMMatrixTranslation(0.f, 0.f, -1.f);
	worldMatrix *= DirectX::XMMatrixRotationY(0);
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
}

SceneObject::~SceneObject()
{
}

void SceneObject::Draw()
{
}

void SceneObject::setVertices(objThing obj)
{
	this->vertices = obj.mesh;
}

bool SceneObject::setVertexBuffer(ID3D11Device* device)
{

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(SimpleVertex)*this->vertices.size();
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertices.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);

	return !FAILED(hr);
}

bool SceneObject::setTextureSrv(ID3D11ShaderResourceView*& texture)
{
	textureSrv = texture;
	return textureSrv!=nullptr;
}

int SceneObject::getVerticeAmount() const
{
	return this->vertices.size();
}

DirectX::XMMATRIX SceneObject::getWorldMatrix() const
{
	return this->worldMatrix;
}

ID3D11Buffer* SceneObject::getVertexBuffer()
{
	return this->vertexBuffer;
}

void SceneObject::setWorldPos(float* arr)
{
	//worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix *= DirectX::XMMatrixTranslation(arr[0], arr[1], arr[2]);
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
}

void SceneObject::setRot(float* arr)
{
	worldMatrix *=	DirectX::XMMatrixRotationZ(arr[2])* 
					DirectX::XMMatrixRotationX(arr[0])* 
					DirectX::XMMatrixRotationY(arr[1]);
	//worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
}

void SceneObject::setScale(float* arr)
{
	worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix *= DirectX::XMMatrixScaling(arr[0], arr[1], arr[2]);
}
