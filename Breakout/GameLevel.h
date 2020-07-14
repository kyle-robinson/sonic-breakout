#pragma once

#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <GLAD/glad.h>
#include <GLM/glm.hpp>

#include "GameObject.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"

#include <vector>

class GameLevel
{
public:
	std::vector<GameObject> Bricks;

	GameLevel() { }
	void Load(const std::string &file, unsigned int levelWidth, unsigned int levelHeight);
	void Draw(SpriteRenderer &renderer);
	bool IsCompleted();

private:
	void Init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif