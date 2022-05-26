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
};

bool SetupPipeline(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& srv, ID3D11SamplerState*& samplerState);