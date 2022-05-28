#pragma once
#include <d3d11.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

bool createSRVforPic(ID3D11Device* device, ID3D11ShaderResourceView*& srv, std::string fileName);
bool LoadTexutres(ID3D11Device* device, std::vector<ID3D11ShaderResourceView*>& textureSrvs);

