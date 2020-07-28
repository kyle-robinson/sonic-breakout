#pragma once

#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

#include <GLAD/glad.h>
#include <GLM/glm.hpp>

#include "Texture2D.h"
#include "Shader.h"
#include "SpriteRenderer.h"

class PostProcessor
{
public:
	Shader PostProcessingShader;
	Texture2D Texture;
	unsigned int Width, Height;

	bool Chaos, Confuse, Shake, Circle;

	PostProcessor(Shader shader, unsigned int width, unsigned int height);
	void BeginRender();
	void EndRender();
	void Render(float time);
private:
	unsigned int MSFBO, FBO; // MSFBO = Multisampled FBO
	unsigned int RBO; // for multisampled color buffer
	unsigned int VAO;

	void InitRenderData();
};

#endif // !POSTPROCESSOR_H