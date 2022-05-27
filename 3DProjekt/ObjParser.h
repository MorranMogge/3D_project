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
	ID3D11ShaderResourceView* srv;
	ID3D11Buffer* vertexBuf;
	std::vector<SimpleVertex> mesh;
};

static void readModels(objThing *test)
{
	//objThing test;
	std::ifstream file("cubeMaterial_1.obj");
	//std::ifstream file("sphere.obj");
	if (!file.is_open()) { std::cout << "Could not open file!\n"; return; }
	std::string loadLines;
	
	std::string wantedString;

	std::vector<DirectX::XMINT3> indexes;
	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> UV;
	float specComp;
	int index[3];
	while (std::getline(file, loadLines))
	{
		std::stringstream readCharacters(loadLines);
		std::getline(readCharacters, wantedString, ' ');
		//std::getline(file, loadLines, ' ');
		if (wantedString == "v")
		{
			positions.push_back({ 0.0f,0.0f,0.0f });
			std::getline(readCharacters, wantedString, ' ');
			positions[positions.size() - 1].x = std::stof(wantedString);
			std::getline(readCharacters, wantedString, ' ');
			positions[positions.size() - 1].y = std::stof(wantedString);
			std::getline(readCharacters, wantedString);
			positions[positions.size() - 1].z = std::stof(wantedString);
		}
		else if (wantedString == "vt")
		{
			UV.push_back({ 0.0f,0.0f });
			std::getline(readCharacters, wantedString, ' ');
			UV[UV.size() - 1].x = std::stof(wantedString);
			std::getline(readCharacters, wantedString);
			UV[UV.size() - 1].y = std::stof(wantedString);
		}
		else if (wantedString == "vn")
		{
			normals.push_back({ 0.0f,0.0f,0.0f });
			std::getline(readCharacters, wantedString, ' ');
			normals[normals.size() - 1].x = std::stof(wantedString);
			std::getline(readCharacters, wantedString, ' ');
			normals[normals.size() - 1].y = std::stof(wantedString);
			std::getline(readCharacters, wantedString);
			normals[normals.size() - 1].z = std::stof(wantedString);
		}
		//else if (wantedString == "s") { std::getline(readCharacters, wantedString); specComp = std::stof(wantedString); }
		else if (wantedString == "f")
		{
			//std::vector<DirectX::XMFLOAT3> tempNormals;
			//std::vector<DirectX::XMFLOAT2> tempUV;
			for (int i = 0; i < 3; i++)
			{
				std::getline(readCharacters, wantedString, '/');
				index[0] = std::stoi(wantedString) - 1;
				std::getline(readCharacters, wantedString, '/');
				index[1] = std::stoi(wantedString) - 1;
				std::getline(readCharacters, wantedString, ' ');
				index[2] = std::stoi(wantedString) - 1;
				/*tempPositions[0] = positions[index].x;
				tempPositions[1] = positions[index].y;
				tempPositions[2] = positions[index].z;
				
				std::getline(file, loadLines, '/');
				index = std::stoi(loadLines);
				tempUV[0] = UV[index].x;
				tempUV[1] = UV[index].y;

				std::getline(file, loadLines);
				index = std::stoi(loadLines);
				tempNormals[0] = normals[index].x;
				tempNormals[1] = normals[index].y;
				tempNormals[2] = normals[index].z;*/

				//std::getline(file, loadLines, '/');
				//tempUV.push_back(UV[std::stoi(loadLines)]);

				//std::getline(file, loadLines, ' ');
				//tempNormals.push_back(normals[std::stoi(loadLines)]);
				//test.mesh.push_back(SimpleVertex(tempPositions, tempNormals, tempUV));
				test->mesh.push_back(SimpleVertex({ positions[index[0]].x, positions[index[0]].y, positions[index[0]].z },
					{ normals[index[2]].x, normals[index[2]].y, normals[index[2]].z }, { UV[index[1]].x,UV[index[1]].y}));
			}
		
			
			//test.mesh.push_back(tempPositions, tempNormals, tempUV);

			/*std::getline(file, loadLines, ' ');
			normals[normals.size() - 1].y = std::stof(loadLines);
			std::getline(file, loadLines);
			normals[normals.size() - 1].z = std::stof(loadLines);*/
		}

	}
	//return &test;
}