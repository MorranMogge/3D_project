#include "SceneObject.h"
#include "PipelineHelper.h"
#include "memoryLeakChecker.h"

void SceneObject::updateConstantBuffer()
{
	DirectX::XMStoreFloat4x4(&wrlMtx, worldMatrix);

	D3D11_MAPPED_SUBRESOURCE mapRes;
	ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HRESULT hr = immediateContext->Map(constantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapRes);
	memcpy(mapRes.pData, &wrlMtx, sizeof(wrlMtx));
	immediateContext->Unmap(constantBuffer, 0);
}

SceneObject::SceneObject(std::vector<SimpleVertex> *inVertices)
	:stride(sizeof(SimpleVertex)), offset(0)
{
	
	this->vertices = inVertices;
		
	worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix *= DirectX::XMMatrixTranslation(0.f, 0.f, -1.f);
	worldMatrix *= DirectX::XMMatrixRotationY(0);
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
}

SceneObject::SceneObject()
	:stride(sizeof(SimpleVertex)), offset(0)
{
	this->vertices->push_back(SimpleVertex({ -10.0f, -0.35f, 10.0f }, { 0, 0, -1 }, { 0, 0 }));
	this->vertices->push_back(SimpleVertex({ 10.0, -0.5f, -10.0f }, { 0, 0, -1 }, { 1, 1 }));
	this->vertices->push_back(SimpleVertex({ -10.0, -0.5f, -10.0f }, { 0, 0, -1 }, { 0, 1 }));
	this->vertices->push_back(SimpleVertex({ -10.0f, -0.35f, 10.0f }, { 0, 0, -1 }, { 0, 0 }));
	this->vertices->push_back(SimpleVertex({ 10.0f, -0.35f, 10.0f }, { 0, 0, -1 }, { 1, 0 }));
	this->vertices->push_back(SimpleVertex({ 10.0f, -0.5f, -10.0f }, { 0, 0, -1 }, { 1, 1 }));

	worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix *= DirectX::XMMatrixTranslation(0.f, 0.f, -1.f);
	worldMatrix *= DirectX::XMMatrixRotationY(0);
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
}

SceneObject::~SceneObject()
{
}

bool SceneObject::setImmediateContext(ID3D11DeviceContext* immediateContext)
{
	this->immediateContext = immediateContext;
	return this->immediateContext!=nullptr;
}

void SceneObject::draw()
{
	//Some objects differ in their pipeline so we need
	//to make sure each is correct for each object

	//Set correct positions
	updateConstantBuffer();
	immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	//Set the correct vertices
	immediateContext->IASetVertexBuffers(0,1, &vertexBuffer, &stride, &offset);

	//Set the correct texture
	immediateContext->PSSetShaderResources(0, 1, &textureSrv);

	//Start the pipeline
	immediateContext->Draw(vertices->size(), 0);
}

void SceneObject::setVertices(objThing obj)
{
	this->vertices = &obj.mesh;
}

bool SceneObject::setVertexBuffer(ID3D11Device* device)
{

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(SimpleVertex)*this->vertices->size();
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertices->data();
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

bool SceneObject::createConstBuf(ID3D11Device* device)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(wrlMtx);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, 0, &constantBuffer);
	return !FAILED(hr);
}

int SceneObject::getVerticeAmount() const
{
	return this->vertices->size();
}

DirectX::XMMATRIX SceneObject::getWorldMatrix() const
{
	return this->worldMatrix;
}

ID3D11Buffer* SceneObject::getVertexBuffer()
{
	return this->vertexBuffer;
}

void SceneObject::setWorldPos(float arr[])
{
	//worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix *= DirectX::XMMatrixTranslation(arr[0], arr[1], arr[2]);
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
}

void SceneObject::setRot(float arr[])
{
	worldMatrix *=	DirectX::XMMatrixRotationZ(arr[2])* 
					DirectX::XMMatrixRotationX(arr[0])* 
					DirectX::XMMatrixRotationY(arr[1]);
	//worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
}

void SceneObject::setScale(float arr[])
{
	worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix *= DirectX::XMMatrixScaling(arr[0], arr[1], arr[2]);
}
