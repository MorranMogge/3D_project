#include "Scene.h"
Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Update()
{
}

void Scene::Draw()
{
	for (auto& o : objects) o.Draw();
}
