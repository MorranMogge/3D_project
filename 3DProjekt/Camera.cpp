#include "Camera.h"
#include "memoryLeakChecker.h"

Camera::Camera()
{
	vectors.padding1 = 0.05f;
}

Camera::~Camera()
{
	ConstBuf->Release();
	computeConstBuf->Release();
	vectorBuffer->Release();
}

void Camera::moveCamera(ID3D11DeviceContext* immediateContext, Camera& cam, float dt)
{
	float multiplier = 1.0f;
	if (GetAsyncKeyState((VK_SHIFT))) multiplier = 2.0f;

	if (GetAsyncKeyState((VK_CONTROL))) multiplier = 0.5f;
	

	if (GetAsyncKeyState('W'))
	{
		forwardVec = XMVector3TransformCoord(DEFAULT_FORWARD, rotationMX);
		cameraPos += multiplier * forwardVec * 20 * dt;
		lookAtPos += multiplier * forwardVec * 20 * dt;
	}

	else if (GetAsyncKeyState('S'))
	{
		forwardVec = XMVector3TransformCoord(DEFAULT_FORWARD, rotationMX);
		cameraPos -= multiplier * forwardVec * 20 * dt;
		lookAtPos -= multiplier * forwardVec * 20 * dt;
	}

	if (GetAsyncKeyState('D'))
	{
		rightVec = XMVector3TransformCoord(DEFAULT_RIGHT, rotationMX);
		cameraPos += multiplier * rightVec * 20 * dt;
		lookAtPos += multiplier * rightVec * 20 * dt;
	}

	else if (GetAsyncKeyState('A'))
	{
		rightVec = XMVector3TransformCoord(DEFAULT_RIGHT, rotationMX);
		cameraPos -= multiplier * rightVec * 20 * dt;
		lookAtPos -= multiplier * rightVec * 20 * dt;
	}

	if (GetAsyncKeyState('I')) this->AdjustRotation(-multiplier * 2.5f*dt,0, immediateContext);

	if (GetAsyncKeyState('J')) this->AdjustRotation(0, -multiplier * 2.5f * dt, immediateContext);

	if (GetAsyncKeyState('K')) this->AdjustRotation(multiplier * 2.5f * dt, 0, immediateContext);

	if (GetAsyncKeyState('L')) this->AdjustRotation(0, multiplier * 2.5f * dt, immediateContext);

	if (GetAsyncKeyState(' '))
	{
		
	}
}

const XMMATRIX& Camera::GetViewMatrix() const
{
	return this->VMBB;
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

void Camera::SetRotation(float pitch, float yaw, float roll, ID3D11DeviceContext* immediateContext)
{
	rotation.x = pitch;
	rotation.y = yaw;
	rotation.z = roll;

	rotVector = DirectX::XMLoadFloat3(&rotation);
	rotationMX = DirectX::XMMatrixRotationRollPitchYawFromVector(rotVector);
	rightVec = DirectX::XMVector3TransformCoord(DEFAULT_RIGHT, rotationMX);
	forwardVec = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD, rotationMX);
	lookAtPos = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD, rotationMX) + cameraPos;
	upVector = DirectX::XMVector3TransformCoord(DEFAULT_UP, rotationMX);


	viewMatrix = XMMatrixLookAtLH(cameraPos, lookAtPos, upVector);
	viewMatrix *= projection;
	viewMatrix = XMMatrixTranspose(viewMatrix);

	if (immediateContext == nullptr) return;
	
	XMStoreFloat4x4(&VP.viewProj, viewMatrix);
	D3D11_MAPPED_SUBRESOURCE subData = {};
	ZeroMemory(&subData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	immediateContext->Map(ConstBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &subData);
	memcpy(subData.pData, &VP, sizeof(VP));
	immediateContext->Unmap(ConstBuf, 0);
	immediateContext->DSSetConstantBuffers(0, 1, &ConstBuf);
}

void Camera::ChangeProjectionMatrix(float FOV, float aspectRatio, float nearZ, float farZ)
{
	this->projection = DirectX::XMMatrixPerspectiveFovLH(FOV, aspectRatio, nearZ, farZ);
}

bool Camera::initiateBuffers(ID3D11DeviceContext* immediateContext, ID3D11Device* device)
{
	this->resetCamera();

	VMBB = XMMatrixLookAtLH(cameraPos, lookAtPos, upVector);
	viewMatrix = XMMatrixLookAtLH(cameraPos, lookAtPos, upVector);
	projection = DirectX::XMMatrixPerspectiveFovLH(XM_2PI*(90.f/360.f), (32.f*32.f) / (32.f*32.f), 0.1f, 100.0f);
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

void Camera::sendView(ID3D11DeviceContext* immediateContext, int index, bool ds)
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
	if (!ds) immediateContext->VSSetConstantBuffers(index, 1, &ConstBuf);
	else immediateContext->DSSetConstantBuffers(index, 1, &ConstBuf);
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

void Camera::changeParticleSize(float size)
{
	vectors.padding1 = size;
}

void Camera::updateFrustum()
{
	lookAtPos = XMVector3TransformCoord(DEFAULT_FORWARD, rotationMX) + cameraPos;
	DirectX::BoundingFrustum::CreateFromMatrix(frustumBB, projection);
	frustumBB.Transform(frustumBB, DirectX::XMMatrixInverse(nullptr, XMMatrixLookAtLH(cameraPos, lookAtPos, upVector)));
}

void Camera::createFrustum()
{
	projection = DirectX::XMMatrixPerspectiveFovLH(XM_2PI * (90.f / 360.f), (32.f * 32.f) / (32.f * 32.f), 0.1f, 100.0f);
	DirectX::BoundingFrustum::CreateFromMatrix(frustumBB, projection);
}

float Camera::getParticleSize()
{
	return vectors.padding1;
}

DirectX::BoundingFrustum Camera::getFrustumBB() const
{
	return frustumBB;
}

void Camera::resetCamera()
{
	rotation.x = 0.0f;
	rotation.y = 0.0f;
	rotation.z = 0.0f;
	rotVector = XMLoadFloat3(&rotation);

	rotationFor.y = 0.0f;
	rotVectorFor = XMLoadFloat3(&rotationFor);

	forwardVec = DEFAULT_FORWARD;
	upVector = DEFAULT_UP;
	rightVec = DEFAULT_RIGHT;
	upVec = DEFAULT_UP;

	rotationForward = XMMatrixRotationRollPitchYawFromVector(rotVectorFor);
	rotationMX = XMMatrixRotationRollPitchYawFromVector(rotVector);
	cameraPos = XMVectorSet(0.0f, 5.0f, 0.0f, 0.0f);
	lookAtPos = XMVectorSet(0.0f, 5.0f, 5.0f, 0.0f);
}
