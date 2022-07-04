#pragma once
#include <DirectXMath.h>
#include <vector>
#include <d3d11.h>

struct Particle
{
	DirectX::XMFLOAT4X4 worldPos;
};

class ParticleHandler
{
private:
	std::vector<Particle> particles;
	D3D11_BUFFER_DESC bufferDesc;
	ID3D11UnorderedAccessView* particleUAV;
	ID3D11Buffer* vBuffer;

	bool setUpUAV(ID3D11Device* device);
	bool setUPVertexBuffer(ID3D11Device* device);

public:
	ParticleHandler();
	~ParticleHandler();
	bool InitiateHandler();
};