#include "ObjParser.h"
#include "TextureLoader.h"

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

void newReadModels(ID3D11Device* device, std::vector<newObjThing>& objArr)
{
	std::ifstream objFile("objectFile.txt");
	std::string fileName;
	bool going = true;
	if (!objFile.is_open()) { std::cout << "Could not open objFile.txt!\n"; return; }
	ID3D11ShaderResourceView* noTexture;
	if (!createSRVforPic(device, noTexture, "Textures/noTexture.png")) return;
	while (std::getline(objFile, fileName))
	{
		std::ifstream file("Models/" + fileName + ".obj");
		if (!file.is_open()) { std::cout << "Could not open obj!\n"; return; }
		objArr.push_back(newObjThing());
		std::string loadLines;
		std::string wantedString;
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT2> UV;
		std::string mtlFileName;
		int index[3];
		while (std::getline(file, loadLines))
		{
			std::stringstream readCharacters(loadLines);
			std::getline(readCharacters, wantedString, ' ');
			//std::getline(file, loadLines, ' ');
			if (wantedString == "mtllib") 
			{ 
				std::getline(readCharacters, wantedString); 
				mtlFileName = wantedString; 
			}
			else if (wantedString == "v")
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
				std::getline(readCharacters, wantedString);
				std::string savedMtlName = wantedString;
				std::ifstream mtilFile("mtlFiles/" + mtlFileName);
				if (!mtilFile.is_open()) { std::cout << "Could not open mtl!\n"; continue; }
				//objArr[objArr.size() - 1].indexes.push_back(objArr.size() - 1);
				for (int i = 0; i < 3; i++)
				{
					objArr[objArr.size() - 1].textureSrvs.push_back(nullptr);
				}
				
				while (std::getline(mtilFile, loadLines) && going)
				{
					

					std::stringstream newReadCharacters(loadLines);
					std::getline(newReadCharacters, wantedString, ' ');
					std::getline(newReadCharacters, wantedString);
					if (wantedString == savedMtlName)
					{
						while (std::getline(mtilFile, loadLines))
						{
							std::stringstream newNewReadCharacters(loadLines);
							std::getline(newNewReadCharacters, wantedString, ' ');
							if (loadLines == "") { going = false;  break; }
							else if (wantedString == "Ns") { std::getline(newNewReadCharacters, wantedString);  objArr[objArr.size() - 1].specularComp.push_back(std::stof(wantedString)); }
							else if (wantedString == "map_Kd")
							{
								std::getline(newNewReadCharacters, wantedString);
								objArr[objArr.size() - 1].textureSrvs[0 + objArr[objArr.size() - 1].indexes.size()*3] = createSRVforPic(device, "Textures/" + wantedString);

							}
							else if (wantedString == "map_Ks")
							{
								std::getline(newNewReadCharacters, wantedString);
								objArr[objArr.size() - 1].textureSrvs[1 + objArr[objArr.size() - 1].indexes.size() * 3] = createSRVforPic(device, "Textures/" + wantedString);
							}
							else if (wantedString == "map_Ka")
							{
								std::getline(newNewReadCharacters, wantedString);
								objArr[objArr.size() - 1].textureSrvs[2 + objArr[objArr.size() - 1].indexes.size() * 3] = createSRVforPic(device, "Textures/" + wantedString);
							}
						}
					}
				
				}
				going = true;
				for (int i = 0; i < 3; i++)
				{
					if (objArr[objArr.size() - 1].textureSrvs[i + objArr[objArr.size() - 1].indexes.size() * 3] == nullptr)
					{
						objArr[objArr.size() - 1].textureSrvs[i + objArr[objArr.size() - 1].indexes.size() * 3] = noTexture;
					}
				}
				objArr[objArr.size() - 1].indexes.push_back(objArr[objArr.size() - 1].indices.size());
			}
			else if (wantedString == "f")
			{
				int tmp = 3;
				bool add = true;
				for (int i = 0; i < tmp; i++)
				{
					add = true;
					std::getline(readCharacters, wantedString, '/');
					index[0] = std::stoi(wantedString) - 1;
					std::getline(readCharacters, wantedString, '/');
					index[1] = std::stoi(wantedString) - 1;
					std::getline(readCharacters, wantedString, ' ');
					index[2] = std::stoi(wantedString) - 1;
					

					SimpleVertex tempVertice({ positions[index[0]].x, positions[index[0]].y, positions[index[0]].z }, { normals[index[2]].x, normals[index[2]].y, normals[index[2]].z }, { UV[index[1]].x,UV[index[1]].y });
					for (int j = 0; j < objArr[objArr.size() - 1].mesh.size(); j++)
					{
						if (objArr[objArr.size() - 1].mesh[j] == tempVertice)
						{
							add = false;
							objArr[objArr.size() - 1].indices.push_back(j);
							break;
						}
					}
						/*for (int j = 0; j < objArr[objArr.size() - 1].mesh.size(); j++)
					{
						if (objArr[objArr.size() - 1].mesh[j].pos[0] == positions[index[0]].x &&
							objArr[objArr.size() - 1].mesh[j].pos[1] == positions[index[0]].y &&
							objArr[objArr.size() - 1].mesh[j].pos[2] == positions[index[0]].z &&
							objArr[objArr.size() - 1].mesh[j].n[0] == normals[index[2]].x &&
							objArr[objArr.size() - 1].mesh[j].n[1] == normals[index[2]].y &&
							objArr[objArr.size() - 1].mesh[j].n[2] == normals[index[2]].z &&
							objArr[objArr.size() - 1].mesh[j].uv[0] == UV[index[1]].x &&
							objArr[objArr.size() - 1].mesh[j].uv[1] == UV[index[1]].y)
							add = false; break;
					}*/ 
					if (add)
					{
						objArr[objArr.size() - 1].indices.push_back(objArr[objArr.size() - 1].mesh.size());
						objArr[objArr.size() - 1].mesh.push_back(tempVertice);
						
					}
				}
			}
		}
		if (objArr[objArr.size() - 1].indexes.size() > 0)
		{
			for (int i = 0; i < objArr[objArr.size() - 1].indexes.size() - 1; i++)
			{
				objArr[objArr.size() - 1].verticeCount.push_back(objArr[objArr.size() - 1].indexes[i + 1] - objArr[objArr.size() - 1].indexes[i]);
			}
			int tempIndex = objArr[objArr.size() - 1].indexes.size()-1;
			objArr[objArr.size() - 1].verticeCount.push_back(objArr[objArr.size() - 1].indices.size() - objArr[objArr.size() - 1].indexes[tempIndex]);
		}
		
	}
}
