#pragma once
#include "PipelineHelper.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

struct objThing
{
	std::vector<SimpleVertex> mesh;
};

struct materialChecker
{
	std::vector<ID3D11ShaderResourceView*> textureSrvs;
	std::vector<std::string> textureNames;
};

struct newObjThing
{
	std::vector<int> indexes;
	std::vector<float> specularComp;
	std::vector<ID3D11ShaderResourceView*> textureSrvs;
	std::vector<SimpleVertex> mesh;
	std::vector<DWORD> indices;
	std::vector<int> verticeCount;
};

void readModels(std::vector<objThing> &test);
void newReadModels(ID3D11Device* device, ID3D11ShaderResourceView* &missingTexture, materialChecker &mat, std::vector<newObjThing>& objArr);