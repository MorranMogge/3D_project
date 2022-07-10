#include "Camera.h"
#include "memoryLeakChecker.h"

Camera::Camera()
{
}

void Camera::moveCamera(ID3D11DeviceContext* immediateContext, Camera& cam, float dt)
{
	bool temp = false;
	if (GetAsyncKeyState((VK_SHIFT)))
	{
		dt *= 2.f;
	}

	if (GetAsyncKeyState((VK_LCONTROL)))
	{
		dt *= 0.5f;
	}

	if (GetAsyncKeyState('W'))
	{
		//forwardVec = XMVector3TransformCoord(DEFAULT_FORWARD, rotationForward);
		cameraPos += forwardVec * 10 * dt;
		lookAtPos += forwardVec * 10 * dt;
	}

	else if (GetAsyncKeyState('S'))
	{
		//forwardVec = XMVector3TransformCoord(DEFAULT_FORWARD, rotationForward);
		cameraPos -= forwardVec * 10 * dt;
		lookAtPos -= forwardVec * 10 * dt;
	}

	if (GetAsyncKeyState('D'))
	{
		rightVec = XMVector3TransformCoord(DEFAULT_RIGHT, rotationMX);
		cameraPos += rightVec * 10 * dt;
		lookAtPos += rightVec * 10 * dt;
	}

	else if (GetAsyncKeyState('A'))
	{
		rightVec = XMVector3TransformCoord(DEFAULT_RIGHT, rotationMX);
		cameraPos -= rightVec * 10 * dt;
		lookAtPos -= rightVec * 10 * dt;
	}

	if (GetAsyncKeyState('I'))
	{
		this->AdjustRotation(-1.5f*dt,0, immediateContext);
	}
	if (GetAsyncKeyState('J'))
	{
		this->AdjustRotation(0, -1.5f * dt, immediateContext);
	}
	if (GetAsyncKeyState('K'))
	{
		this->AdjustRotation(1.5f * dt, 0, immediateContext);
	}
	if (GetAsyncKeyState('L'))
	{
		this->AdjustRotation(0, 1.5f * dt, immediateContext);
	}

	if (GetAsyncKeyState('E'))
	{
		//upVec = XMVector3TransformCoord(DEFAULT_UP, rotationMX);
		cameraPos += upVec * 2.5 * dt;
		lookAtPos += upVec * 2.5 * dt;
	}

	if (GetAsyncKeyState('Q'))
	{
		//upVec = XMVector3TransformCoord(DEFAULT_UP, rotationMX);
		cameraPos -= upVec * 2.5 * dt;
		lookAtPos -= upVec * 2.5 * dt;
	}

	if (GetAsyncKeyState(' '))
	{
		rotation.x = 0.0f;
		rotation.y = 0.0f;
		rotVector = XMLoadFloat3(&rotation);

		rotationFor.y = 0.0f;
		rotVectorFor = XMLoadFloat3(&rotationFor);

		forwardVec = DEFAULT_FORWARD;
		upVector = DEFAULT_UP;
		rightVec = DEFAULT_RIGHT;
		upVec = DEFAULT_UP;

		rotationForward = XMMatrixRotationRollPitchYawFromVector(rotVectorFor);
		rotationMX = XMMatrixRotationRollPitchYawFromVector(rotVector);
		cameraPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		lookAtPos = XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
	}

	running = temp;
	/*Bouncing();*/
}

const XMMATRIX& Camera::GetViewMatrix() const
{
	return this->VMBB;
}

const XMVECTOR& Camera::GetPositionVector() const
{
	return this->cameraPos;
}

const XMFLOAT3& Camera::GetPositionFloat3() const
{
	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, cameraPos);
	return newPos;
}

const XMFLOAT3& Camera::GetRotationFloat3() const
{
	return this->rotation;
}

void Camera::SetPosition(float x, float y, float z)
{
	XMFLOAT3 newPos;
	newPos.x = x;
	newPos.y = y;
	newPos.z = z;
	cameraPos = XMVectorSet(newPos.x, newPos.y, newPos.z, 0.0f);
	lookAtPos = XMVector3TransformCoord(DEFAULT_FORWARD, rotationMX) + cameraPos;
}

void Camera::SetRotation(float x, float y, float z)
{
	rotation.x = x;
	rotation.y = y;
	rotVector = XMLoadFloat3(&rotation);
	rotationMX = XMMatrixRotationRollPitchYawFromVector(rotVector);
	lookAtPos = XMVector3TransformCoord(DEFAULT_FORWARD, rotationMX) + cameraPos;
	upVector = XMVector3TransformCoord(DEFAULT_UP, rotationMX);
}

const XMVECTOR& Camera::GetForwardVector()
{
	forwardVec = XMVector3TransformCoord(DEFAULT_FORWARD, rotationMX);
	return forwardVec;
}

const XMVECTOR& Camera::GetRightVector()
{
	rightVec = XMVector3TransformCoord(DEFAULT_RIGHT, rotationMX);
	return this->rightVec;
}

const XMVECTOR& Camera::GetBackwardVector()
{
	forwardVec = XMVector3TransformCoord(DEFAULT_FORWARD, rotationMX);
	return -this->forwardVec;
}

const XMVECTOR& Camera::GetLeftVector()
{
	rightVec = XMVector3TransformCoord(DEFAULT_RIGHT, rotationMX);
	return -this->rightVec;
}

void Camera::ChangeProjectionMatrix(float FOV, float aspectRatio, float nearZ, float farZ)
{
	this->projection = DirectX::XMMatrixPerspectiveFovLH(FOV, aspectRatio, nearZ, farZ);
}

bool Camera::CreateCBuffer(ID3D11DeviceContext* immediateContext, ID3D11Device* device)
{
	rotation.x = 0.0f;
	rotation.y = 0.0f;
	rotVector = XMLoadFloat3(&rotation);

	rotationFor.y = 0.0f;
	rotVectorFor = XMLoadFloat3(&rotationFor);

	forwardVec = DEFAULT_FORWARD;
	upVector = DEFAULT_UP;
	rightVec = DEFAULT_RIGHT;
	upVec = DEFAULT_UP;

	rotationForward = XMMatrixRotationRollPitchYawFromVector(rotVectorFor);
	rotationMX = XMMatrixRotationRollPitchYawFromVector(rotVector);
	cameraPos = XMVectorSet(0.0f, 6.0f, 0.0f, 0.0f);
	lookAtPos = XMVectorSet(0.0f, 6.0f, 5.0f, 0.0f);

	VMBB = XMMatrixLookAtLH(cameraPos, lookAtPos, upVector);
	viewMatrix = XMMatrixLookAtLH(cameraPos, lookAtPos, upVector);
	projection = DirectX::XMMatrixPerspectiveFovLH(0.8f, 1024.f / 576, 0.1f, 800.0f);
	viewMatrix *= projection;
	viewMatrix = XMMatrixTranspose(viewMatrix);
	XMStoreFloat4x4(&VP.viewProj, viewMatrix);

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(VP);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, 0, &ConstBuf);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_BUFFER_DESC otherBufferDesc = {};
	otherBufferDesc.ByteWidth = sizeof(computeMatrix);
	otherBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	otherBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	otherBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	otherBufferDesc.MiscFlags = 0;
	otherBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&otherBufferDesc, 0, &computeConstBuf);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_BUFFER_DESC thirdBufferDesc = {};
	thirdBufferDesc.ByteWidth = sizeof(vectors);
	thirdBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	thirdBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	thirdBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	thirdBufferDesc.MiscFlags = 0;
	thirdBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&thirdBufferDesc, 0, &vectorBuffer);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

void Camera::AdjustRotation(float x, float y, ID3D11DeviceContext* immediateContext)
{
	const float max = 1.5f;
	const float min = -1.5f;

	rotation.x += x;
	rotation.y += y;
	rotVector = XMLoadFloat3(&rotation);

	rotationFor.y += y;
	rotVectorFor = XMLoadFloat3(&rotationFor);

	if (rotation.x > max)
	{
		rotation.x = max;
	}
	else if (rotation.x < min)
	{
		rotation.x = min;
	}

	rotationForward = XMMatrixRotationRollPitchYawFromVector(rotVectorFor);
	rotationMX = XMMatrixRotationRollPitchYawFromVector(rotVector);
	upVector = XMVector3TransformCoord(DEFAULT_UP, rotationMX);
	forwardVec = XMVector3TransformCoord(DEFAULT_FORWARD, rotationForward);
	lookAtPos = XMVector3TransformCoord(DEFAULT_FORWARD, rotationMX) + cameraPos;
}

void Camera::sendProjection(ID3D11DeviceContext* immediateContext, bool cs)
{
	XMStoreFloat4x4(&VP.viewProj, XMMatrixTranspose(projection));

	D3D11_MAPPED_SUBRESOURCE subData = {};
	immediateContext->Map(ConstBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &subData);
	memcpy(subData.pData, &VP, sizeof(VP));
	immediateContext->Unmap(ConstBuf, 0);
	if (!cs) immediateContext->VSSetConstantBuffers(1, 1, &ConstBuf);
	else immediateContext->CSSetConstantBuffers(1, 1, &ConstBuf);
}

void Camera::sendGeometryMatrix(ID3D11DeviceContext* immediateContext)
{
	VMBB = XMMatrixLookAtLH(cameraPos, lookAtPos, upVector);
	XMStoreFloat4x4(&computeMatrix.worldView, XMMatrixTranspose(VMBB));
	XMStoreFloat4x4(&computeMatrix.proj, XMMatrixTranspose(projection));

	D3D11_MAPPED_SUBRESOURCE subData = {};
	immediateContext->Map(computeConstBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &subData);
	memcpy(subData.pData, &computeMatrix, sizeof(computeMatrix));
	immediateContext->Unmap(computeConstBuf, 0);
	immediateContext->GSSetConstantBuffers(0, 1, &computeConstBuf);
}

void Camera::sendView(ID3D11DeviceContext* immediateContext, int index, bool cs)
{
	VMBB = XMMatrixLookAtLH(cameraPos, lookAtPos, upVector);
	viewMatrix = XMMatrixLookAtLH(cameraPos, lookAtPos, upVector);
	viewMatrix *= projection;
	viewMatrix = XMMatrixTranspose(viewMatrix);

	XMStoreFloat4x4(&VP.viewProj, viewMatrix);
	D3D11_MAPPED_SUBRESOURCE subData = {};
	ZeroMemory(&subData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	immediateContext->Map(ConstBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &subData);
	memcpy(subData.pData, &VP, sizeof(VP));
	immediateContext->Unmap(ConstBuf, 0);
	if (!cs) immediateContext->VSSetConstantBuffers(index, 1, &ConstBuf);
	else immediateContext->CSSetConstantBuffers(index, 1, &ConstBuf);
}

void Camera::sendVectorsGeometry(ID3D11DeviceContext* immediateContext)
{
	upVec = XMVector3TransformCoord(DEFAULT_UP, rotationMX);
	forwardVec = XMVector3TransformCoord(DEFAULT_FORWARD, rotationMX);
	DirectX::XMStoreFloat3(&vectors.forwardVector, forwardVec);
	DirectX::XMStoreFloat3(&vectors.upVector, upVec);
	forwardVec = XMVector3TransformCoord(DEFAULT_FORWARD, rotationForward);

	D3D11_MAPPED_SUBRESOURCE subData = {};
	immediateContext->Map(vectorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subData);
	memcpy(subData.pData, &vectors, sizeof(vectors));
	immediateContext->Unmap(vectorBuffer, 0);
	immediateContext->GSSetConstantBuffers(1, 1, &vectorBuffer);
}

void Camera::noMoreMemoryLeaks()
{
	ConstBuf->Release();
	computeConstBuf->Release();
	vectorBuffer->Release();
}
