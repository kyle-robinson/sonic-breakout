#pragma once

#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <GLAD/glad.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include "Texture2D.h"
#include "Shader.h"

class SpriteRenderer
{
public:
	SpriteRenderer(Shader shader);
	~SpriteRenderer();
	void DrawSprite(Texture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

private:
	Shader shader;
	unsigned int quadVAO;
	void InitRenderData();
};

#endif