#pragma once

#include <array>
#include <d3d11.h>

struct SimpleVertex
{
	float pos[3];
	float n[3];
	float uv[2];

	SimpleVertex(const std::array<float, 3>& position, const std::array<float, 3> normal, const std::array<float, 2>& UV)
	{
		for (int i = 0; i < 3; i++)
		{
			pos[i] = position[i];
			n[i] = normal[i];
			
		}
		for (int i = 0; i < 2; i++)
		{
			uv[i] = UV[i];
		}
	}

	bool operator==(SimpleVertex const& other)
	{
		if (this->n[0] == other.n[0] && this->n[1] == other.n[1] && this->n[2] == other.n[2] &&
			this->pos[0] == other.pos[0] && this->pos[1] == other.pos[1] && this->pos[2] == other.pos[2] &&
			this->uv[0] == other.uv[0] && this->uv[1] == other.uv[1])
		{
			return true;
		}
		return false;
	}
};



bool SetupPipeline(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, ID3D11SamplerState*& samplerState);