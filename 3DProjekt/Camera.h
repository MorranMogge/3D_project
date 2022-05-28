#pragma once
#include <directxmath.h>
#include <iostream>
#include <d3d11.h>
#include <fstream>
#include <string>

using namespace DirectX;

struct WorldProjMatrix
{
	XMFLOAT4X4 viewProj;
};

class Camera
{
public:
	Camera();

	void moveCamera(ID3D11DeviceContext* immediateContext, Camera& cam, float dt);

	const XMMATRIX& GetViewMatrix() const;
	const XMVECTOR& GetPositionVector() const;
	const XMFLOAT3& GetPositionFloat3() const;
	const XMFLOAT3& GetRotationFloat3() const;

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(float x, float y, ID3D11DeviceContext* immediateContext);

	const XMVECTOR& GetForwardVector();
	const XMVECTOR& GetRightVector();
	const XMVECTOR& GetBackwardVector();
	const XMVECTOR& GetLeftVector();

	//OLIVER LOVAR ATT �NDRA - Han failade // Klara
	void ChangeProjectionMatrix(float FOV, float aspectRatio, float nearZ, float farZ);
	bool CreateCBuffer(ID3D11DeviceContext* immediateContext, ID3D11Device* device);
	void sendProjection(ID3D11DeviceContext* immediateContext);
	void sendView(ID3D11DeviceContext* immediateContext, int index = 1);
	bool getRunning();
	void noMoreMemoryLeaks();

private:
	ID3D11Buffer* ConstBuf;
	XMMATRIX VMBB;
	XMMATRIX viewMatrix;
	XMMATRIX projection;
	WorldProjMatrix VP;

	//New ones
	XMMATRIX rotationMX;
	XMMATRIX rotationForward;
	XMVECTOR cameraPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR lookAtPos = XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);

	const XMVECTOR DEFAULT_RIGHT = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_UP = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_FORWARD = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	XMVECTOR forwardVec = DEFAULT_FORWARD;
	XMVECTOR upVector = DEFAULT_UP;
	XMVECTOR rightVec = DEFAULT_RIGHT;
	XMVECTOR upVec = DEFAULT_UP;

	XMFLOAT3 rotation;
	XMVECTOR rotVector = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT3 rotationFor;
	XMVECTOR rotVectorFor = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR temp;
	bool running;
};