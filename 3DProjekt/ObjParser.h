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

void readModels(std::vector<objThing> &test);