#include "ObjParser.h"
#include "TextureLoader.h"

int getIndex(std::string picName)
{
	int returnValue = -1;
	if (picName == "map_Ka") returnValue = 0;
	else if (picName == "map_Kd") returnValue = 1;
	else returnValue = 2;
	return returnValue;
}

void addFaces(objectInfo* objPtr, std::stringstream& readCharacters, std::string& wantedString,
	std::vector<DirectX::XMFLOAT3> positions, std::vector<DirectX::XMFLOAT3> normals, std::vector<DirectX::XMFLOAT2> UV)
{
	int tmp = 3;
	int index[3];
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
		for (int j = 0; j < objPtr->mesh.size(); j++)
		{
			if (objPtr->mesh[j] == tempVertice)
			{
				add = false;
				objPtr->indices.push_back(j);
				break;
			}
		}
		if (add)
		{
			objPtr->indices.push_back(objPtr->mesh.size());
			objPtr->mesh.push_back(tempVertice);

		}
	}
}

void addFloat3(std::vector<DirectX::XMFLOAT3>& component, std::stringstream& readCharacters, std::string& wantedString)
{
	component.push_back({ 0.0f,0.0f,0.0f });
	std::getline(readCharacters, wantedString, ' ');
	component[component.size() - 1].x = std::stof(wantedString);
	std::getline(readCharacters, wantedString, ' ');
	component[component.size() - 1].y = std::stof(wantedString);
	std::getline(readCharacters, wantedString);
	component[component.size() - 1].z = std::stof(wantedString);
}

void addFloat2(std::vector<DirectX::XMFLOAT2>& component, std::stringstream& readCharacters, std::string& wantedString)
{
	component.push_back({ 0.0f,0.0f });
	std::getline(readCharacters, wantedString, ' ');
	component[component.size() - 1].x = std::stof(wantedString);
	std::getline(readCharacters, wantedString);
	component[component.size() - 1].y = std::stof(wantedString);
}

void readModels(ID3D11Device* device, ID3D11ShaderResourceView*& missingTexture, materialChecker& mat, std::vector<objectInfo>& objArr)
{
	std::ifstream objFile("objectFile.txt");
	std::string fileName;
	bool going = true;
	if (!objFile.is_open()) { std::cout << "Could not open objFile.txt!\n"; return; }
	if (!createSRVforPic(device, missingTexture, "Textures/White.png")) return;
	while (std::getline(objFile, fileName))
	{
		std::ifstream file("Models/" + fileName + ".obj");
		if (!file.is_open()) { std::cout << "Could not open obj!\n"; return; }
		objArr.push_back(objectInfo());
		objectInfo* objPtr = &objArr[objArr.size() - 1];	//Optimisation
		std::string loadLines;
		std::string wantedString;
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT2> UV;
		std::string mtlFileName;

		while (std::getline(file, loadLines))
		{
			std::stringstream readCharacters(loadLines);
			std::getline(readCharacters, wantedString, ' ');

			if (wantedString == "v") 
			{ 
				addFloat3(positions, readCharacters, wantedString);
				if (positions.size() == 1) { objPtr->topLeft = objPtr->bottomRight = positions.back(); }
				else
				{
					if (objPtr->topLeft.x >= positions.back().x && objPtr->topLeft.y >= positions.back().y && objPtr->topLeft.z >= positions.back().z) objPtr->topLeft = positions.back();
					else if (objPtr->bottomRight.x <= positions.back().x && objPtr->bottomRight.y <= positions.back().y && objPtr->bottomRight.z <= positions.back().z) objPtr->bottomRight = positions.back();
				}
			}

			else if (wantedString == "vt")	addFloat2(UV, readCharacters, wantedString);

			else if (wantedString == "vn")	addFloat3(normals, readCharacters, wantedString);

			else if (wantedString == "f")	addFaces(objPtr, readCharacters, wantedString, positions, normals, UV);

			else if (wantedString == "mtllib") { std::getline(readCharacters, wantedString); mtlFileName = wantedString; }

			else if (wantedString == "usemtl")
			{
				std::getline(readCharacters, wantedString);
				std::string savedMtlName = wantedString;
				std::ifstream mtilFile("mtlFiles/" + mtlFileName);
				for (int i = 0; i < 3; i++)
				{
					objPtr->textureSrvs.push_back(nullptr);
				}
				if (!mtilFile.is_open()) 
				{ 
					std::cout << "Could not open mtl!\n";
					for (int i = 0; i < 3; i++)
					{
						if (objPtr->textureSrvs[i + objPtr->indexes.size() * 3] == nullptr)
						{
							objPtr->textureSrvs[i + objPtr->indexes.size() * 3] = missingTexture;
						}
					}
					continue; 
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
							else if (wantedString == "Ns") { std::getline(newNewReadCharacters, wantedString);  objPtr->specularComp.push_back(std::stof(wantedString)); }
							else if (wantedString == "map_Ka" || wantedString == "map_Kd" || wantedString == "map_Ks")
							{
								int tempIndex = getIndex(wantedString);
								std::getline(newNewReadCharacters, wantedString);
								for (int i = 0; i < mat.textureNames.size(); i++)
								{
									if (wantedString == mat.textureNames[i])
									{
										objPtr->textureSrvs[objPtr->indexes.size() * 3 + tempIndex] = mat.textureSrvs[i];
									}
								}

								if (objPtr->textureSrvs[objPtr->indexes.size() * 3 + tempIndex] == nullptr)
								{
									objPtr->textureSrvs[objPtr->indexes.size() * 3 + tempIndex] = createSRVforPic(device, "Textures/" + wantedString);
									mat.textureNames.push_back(wantedString);
									mat.textureSrvs.push_back(objPtr->textureSrvs[objPtr->indexes.size() * 3 + tempIndex]);
								}

							}
						}
					}

				}
				going = true;
				for (int i = 0; i < 3; i++)
				{
					if (objPtr->textureSrvs[i + objPtr->indexes.size() * 3] == nullptr)
					{
						objPtr->textureSrvs[i + objPtr->indexes.size() * 3] = missingTexture;
					}
				}
				objPtr->indexes.push_back(objPtr->indices.size());
				mtilFile.close();
			}
		}
		if (objPtr->indexes.size() > 0)
		{
			for (int i = 0; i < objPtr->indexes.size() - 1; i++)
			{
				objPtr->verticeCount.push_back(objPtr->indexes[i + 1] - objPtr->indexes[i]);
			}
			int tempIndex = objPtr->indexes.size() - 1;
			objPtr->verticeCount.push_back(objPtr->indices.size() - objPtr->indexes[tempIndex]);
		}
		if (objPtr->specularComp.size() <= 0)
		{	
			objPtr->specularComp.push_back(0);
		}
		if (objPtr->textureSrvs.size() == 0)
		{
			for (int i = 0; i < 3; i++)
			{
				objPtr->textureSrvs.push_back(missingTexture);
			}
		}
		if (objPtr->verticeCount.size() == 0) objPtr->verticeCount.push_back(objPtr->indices.size());
		if (objPtr->indexes.size() == 0) objPtr->indexes.push_back(0);
		file.close();
	}
	objFile.close();
}
