#pragma once
#include <vector>
#include "SceneObject.h"

class Scene
{
private:
	std::vector<SceneObject> objects;
public:
	Scene();
	~Scene();
	void Update();
	void Draw();
};