#pragma once
#include <d3d11.h>
#include <array>

struct lightBuffer
{
	//Light properties
	float ambientPart[3];
	float padd1;
	float diffusePart[3];
	float padd2;
	float specularLight[3];
	float padd3;
	float lightOrigin[3];
	float padd4;
	float cameraPos[3];
	float padd5;

	
	lightBuffer(const std::array<float, 4>& ambient, const std::array<float, 4> diffuse, const std::array<float, 3>& specular,
				const std::array<float, 3>& origin, const std::array<float, 3>& camera)
	{

		for (int i = 0; i < 3; i++)
		{
			ambientPart[i] = ambient[i];
			diffusePart[i] = diffuse[i];
			specularLight[i] = specular[i];
			lightOrigin[i] = origin[i];
			cameraPos[i] = camera[i];
		}
		padd1 = padd2 = padd3 = padd4 = padd5 = 0.0f;
	}
};

class LightBuffer
{
private:
	lightBuffer lightB;

public:
	LightBuffer();
	bool setLightBuffer(ID3D11Device* device, ID3D11Buffer*& lightBuffer);
	void setNormal(ID3D11DeviceContext* immediateContext, ID3D11Buffer*& lightBuffer, bool normal);


	
};
