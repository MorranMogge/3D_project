#include "ObjParser.h"

void readModels(std::vector<objThing> &objArr)
{
	std::ifstream objFile("objectFile.txt");
	std::string fileName;
	if (!objFile.is_open()) { std::cout << "Could not open objFile.txt!\n"; return; }
	while (std::getline(objFile, fileName))
	{
		objArr.push_back(objThing());
		std::ifstream file("Models/"+fileName + ".obj");
		if (!file.is_open()) { std::cout << "Could not open obj!\n"; return; }

		std::string loadLines;
		std::string wantedString;
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT2> UV;
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
				for (int i = 0; i < 3; i++)
				{
					std::getline(readCharacters, wantedString, '/');
					index[0] = std::stoi(wantedString) - 1;
					std::getline(readCharacters, wantedString, '/');
					index[1] = std::stoi(wantedString) - 1;
					std::getline(readCharacters, wantedString, ' ');
					index[2] = std::stoi(wantedString) - 1;

					objArr[objArr.size() - 1].mesh.push_back(SimpleVertex({ positions[index[0]].x, positions[index[0]].y, positions[index[0]].z },
						{ normals[index[2]].x, normals[index[2]].y, normals[index[2]].z }, { UV[index[1]].x,UV[index[1]].y }));
				}
			}
		}
	}
}

void newReadModels(std::vector<newObjThing>& objArr)
{
	std::ifstream objFile("objectFile.txt");
	std::string fileName;
	if (!objFile.is_open()) { std::cout << "Could not open objFile.txt!\n"; return; }
	while (std::getline(objFile, fileName))
	{
		std::ifstream file("Models/" + fileName + ".obj");
		if (!file.is_open()) { std::cout << "Could not open obj!\n"; return; }

		std::string loadLines;
		std::string wantedString;
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT2> UV;
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
			else if (wantedString == "usemtl") 
			{
				//Save amount of vertices
				objArr[objArr.size() - 1].indexes.push_back(objArr.size() - 1);
				std::getline(readCharacters, wantedString);
				std::ifstream mtilFile(wantedString);
				while (std::getline(mtilFile, loadLines))
				{
					std::getline(readCharacters, wantedString, ' ');
					if (wantedString == "Ns") { std::getline(readCharacters, wantedString);  objArr[objArr.size() - 1].specularComp.push_back(std::stof(wantedString));}
					else if (wantedString == "map_Kd") 
					{
						
					}
					else if (wantedString == "map_Ks")
					{

					}
					else if (wantedString == "map_Ka")
					{

					}
				}
			}
			else if (wantedString == "f")
			{
				for (int i = 0; i < 3; i++)
				{
					std::getline(readCharacters, wantedString, '/');
					index[0] = std::stoi(wantedString) - 1;
					std::getline(readCharacters, wantedString, '/');
					index[1] = std::stoi(wantedString) - 1;
					std::getline(readCharacters, wantedString, ' ');
					index[2] = std::stoi(wantedString) - 1;

					objArr[objArr.size() - 1].mesh.push_back(SimpleVertex({ positions[index[0]].x, positions[index[0]].y, positions[index[0]].z },
						{ normals[index[2]].x, normals[index[2]].y, normals[index[2]].z }, { UV[index[1]].x,UV[index[1]].y }));
				}
			}
		}
	}
}
