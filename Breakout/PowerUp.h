#pragma once
#ifndef POWERUP_H
#define POWERUP_H

#include <GLAD/glad.h>
#include <GLM/glm.hpp>

#include "GameObject.h"
#include <string>

const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject
{
public:
	std::string Type;
	float Duration;
	bool Activated;

	PowerUp(std::string type, glm::vec2 size, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture)
		: GameObject(position, size, texture, color, VELOCITY), Type(type), Duration(duration), Activated() { }
};

#endif // POWERUP_H