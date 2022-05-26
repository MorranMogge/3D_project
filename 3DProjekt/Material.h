#pragma once
#include <d3d11.h>
#include <array>

struct material
{
	float ambientC;
	float diffuseC;
	float specularC;
	float shininess;

	material(float ambient, float diffuse, float specular, float shiny)
	{
		ambientC = ambient;
		diffuseC = diffuse;
		specularC = specular;
		shininess = shiny;
	}
};

class Material
{
private:
	material materialInfo;

public:
	Material();
	bool setMaterial(ID3D11Device* device, ID3D11Buffer*& lightBuffer);




};