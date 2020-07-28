#pragma once

#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <GLAD/glad.h>
#include <GLM/glm.hpp>

#include "Texture2D.h"
#include "Shader.h"
#include <map>

struct Character
{
	unsigned int TextureID; // ID of glyph texture
	glm::ivec2 Size; // Size of glyph
	glm::ivec2 Bearing; // Offset from baseline
	unsigned int Advance; // Horizontal offset to next glyph
};

class TextRenderer
{
public:
	std::map<char, Character> Characters;
	Shader TextShader;
	
	TextRenderer(unsigned int width, unsigned int height);
	void Load(std::string font, unsigned int fontSize);
	void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));
private:
	unsigned int VAO, VBO;
};

#endif // !TEXTRENDERER_H