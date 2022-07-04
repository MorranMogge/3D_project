#include "ParticleHandler.h"

ParticleHandler::ParticleHandler()
{
	//Setup description
	bufferDesc.ByteWidth = sizeof(particles);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	
}

bool ParticleHandler::setUpUAV(ID3D11Device* device)
	{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc =

	{
	uavDesc.Format = DXGI_FORMAT_R32_FLOAT,
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER,

	uavDesc.Buffer = {
						uavDesc.Buffer.FirstElement = 0,
						uavDesc.Buffer.NumElements = std::size(particles) * 3,
						uavDesc.Buffer.Flags = 0
						}
	};

	return false;
	}
