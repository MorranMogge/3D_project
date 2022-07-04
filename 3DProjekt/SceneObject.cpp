#include "SceneObject.h"
#include "PipelineHelper.h"
#include "memoryLeakChecker.h"

void SceneObject::updateConstantBuffer()
{
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
	DirectX::XMStoreFloat4x4(&wrlMtx, worldMatrix);

	D3D11_MAPPED_SUBRESOURCE mapRes;
	ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HRESULT hr = immediateContext->Map(constantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapRes);
	memcpy(mapRes.pData, &wrlMtx, sizeof(wrlMtx));
	immediateContext->Unmap(constantBuffer, 0);
}

void SceneObject::updateWorldMatrix()
{
	worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix *= DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	worldMatrix *= DirectX::XMMatrixRotationZ(rot.z) * DirectX::XMMatrixRotationX(rot.x) * DirectX::XMMatrixRotationY(rot.y);
	worldMatrix *= DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	
}

SceneObject::SceneObject(std::vector<SimpleVertex> *inVertices)
	:stride(sizeof(SimpleVertex)), offset(0), pos({ 0,0,0 }), rot({ 0,0,0 }), scale({1,1,1})
{
	
	this->vertices = inVertices;
	
	worldMatrix = DirectX::XMMatrixIdentity();
	//worldMatrix *= DirectX::XMMatrixTranslation(0.f, 0.f, -1.f);
	//worldMatrix *= DirectX::XMMatrixRotationY(0);
	//worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
}

SceneObject::SceneObject(newObjThing inObj)
	:stride(sizeof(SimpleVertex)), offset(0), pos({ 0,0,0 }), rot({ 0,0,0 }), scale({ 1,1,1 })
{
	for (int i = 0; i < inObj.indexes.size(); i++)
	{
		indexes.push_back(inObj.indexes[i]);
		shinyness.push_back(materialInfo(inObj.specularComp[i]));
	}
	for (int i = 0; i < inObj.textureSrvs.size(); i++)
	{
		textureSrv.push_back(inObj.textureSrvs[i]);
	}
	for (int i = 0; i < inObj.verticeCount.size(); i++)
	{
		this->verticeCount.push_back(inObj.verticeCount[i]);
	}

	/*for (int i = 0; i < inObj.mesh.size(); i++)
	{
		vertices->push_back(inObj.mesh[i]);
	}*/
	//this->vertices = &inObj.mesh;
	/*for (int i = 0; i < inObj.mesh.size(); i++)
	{
		textureSrv.push_back(inObj.textureSrvs[i]);
	}*/
	worldMatrix = DirectX::XMMatrixIdentity();
}

SceneObject::SceneObject()
	:stride(sizeof(SimpleVertex)), offset(0), pos({ 0,0,0 }), rot({ 0,0,0 }), scale({ 1,1,1 })
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

void SceneObject::initiateObject(ID3D11DeviceContext* immediateContext, ID3D11Device* device, std::vector<SimpleVertex>* inVertices, std::vector<DWORD> *indices)
{
	this->setImmediateContext(immediateContext);
	this->createConstBuf(device);
	this->setVertices(inVertices);
	this->setMatBuffer(device);
	this->setIndices(indices);
	this->createIndexBuffer(device);
}

void SceneObject::draw()
{
	//Some objects differ in their pipeline so we need
	//to make sure each is correct for each object

	//Update
	updateWorldMatrix();
	updateConstantBuffer();


	//Set correct positions
	immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	//Set the correct vertices
	immediateContext->IASetVertexBuffers(0,1, &vertexBuffer, &stride, &offset);

	//Set the correct texture
	immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	//Start the pipeline
	for (int i = 0; i < indexes.size(); i++)
	{
		immediateContext->PSSetShaderResources(0, 1, &textureSrv[i * 3 + 0]);
		immediateContext->PSSetShaderResources(1, 1, &textureSrv[i * 3 + 1]);
		immediateContext->PSSetShaderResources(2, 1, &textureSrv[i * 3 + 2]);
		immediateContext->DrawIndexed(verticeCount[i], indexes[i], 0);
		//immediateContext->DrawIndexed(20, indexes[i], 0);
	}
	//immediateContext->PSSetShaderResources(0, 1, &textureSrv[rand()%((indexes.size() - 1)*3)]);
	//immediateContext->DrawIndexed(indexes[indexes.size() - 1] - indexes[indexes.size() - 2], indexes[indexes.size() - 2], 0);
	//immediateContext->Draw(vertices->size(), 0);
}

void SceneObject::draw(bool testDraw)
{
	//Some objects differ in their pipeline so we need
	//to make sure each is correct for each object

	//Update
	updateWorldMatrix();
	updateConstantBuffer();


	//Set correct positions
	immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	//Set the correct vertices
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	//Set the correct texture
	immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	//Start the pipeline
	/*for (int i = 0; i < indexes.size(); i++)
	{
		immediateContext->PSSetShaderResources(0, 1, &textureSrv[i * 3 + 0]);
		immediateContext->PSSetShaderResources(1, 1, &textureSrv[i * 3 + 1]);
		immediateContext->PSSetShaderResources(2, 1, &textureSrv[i * 3 + 2]);
		immediateContext->DrawIndexed(verticeCount[i], indexes[i], 0);
	}*/

	if (!testDraw)
	{
		immediateContext->PSSetShaderResources(0, 1, &textureSrv[0]);
		immediateContext->Draw(indices->size(), 0);
	}
	else
	{

		for (int i = 0; i < indexes.size(); i++)
		{
			//immediateContext->PSSetConstantBuffers(0, 1, &matBuffer[i]);
			immediateContext->PSSetShaderResources(0, 1, &textureSrv[i * 3 + 0]);
			immediateContext->PSSetShaderResources(1, 1, &textureSrv[i * 3 + 1]);
			immediateContext->PSSetShaderResources(2, 1, &textureSrv[i * 3 + 2]);
			immediateContext->DrawIndexed(verticeCount[i], indexes[i], 0);
		}
	}
}

void SceneObject::setVertices(objThing obj)
{
	this->vertices = &obj.mesh;
}

void SceneObject::setVertices(std::vector<SimpleVertex>* inVertices)
{
	this->vertices = inVertices;
}

void SceneObject::setIndices(std::vector<DWORD>* indices)
{
	this->indices = indices;
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

bool SceneObject::setMatBuffer(ID3D11Device* device)
{
	for (int i = 0; i < shinyness.size(); i++)
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(materialInfo);
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = &shinyness[i];
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		ID3D11Buffer* tempBuffer;
		
		HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &tempBuffer);
		if (FAILED(hr))
		{
			return false;
		}
		matBuffer.push_back(tempBuffer);
	}

	return true;
}

bool SceneObject::setTextureSrv(ID3D11ShaderResourceView*& texture)
{
	textureSrv.push_back(texture);
	//textureSrv[0] = texture;
	return textureSrv[0]!=nullptr;
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

bool SceneObject::createIndexBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * indices->size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = indices->data();
	HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);
	return !FAILED(hr);
}

void SceneObject::releaseCom()
{
	constantBuffer->Release();
	vertexBuffer->Release();
	indexBuffer->Release();
	for (int i = 0; i < shinyness.size(); i++)
	{
		matBuffer[i]->Release();
	}
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
	pos.x = arr[0];
	pos.y = arr[1];
	pos.z = arr[2];
}

void SceneObject::setRot(float arr[])
{
	rot.x = arr[0];
	rot.y = arr[1];
	rot.z = arr[2];
}

void SceneObject::setScale(float arr[])
{
	scale.x = arr[0];
	scale.y = arr[1];
	scale.z = arr[2];
}
