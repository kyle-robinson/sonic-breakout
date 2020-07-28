#include "TextRenderer.h"
#include "ResourceManager.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <GLM/gtc/matrix_transform.hpp>

TextRenderer::TextRenderer(unsigned int width, unsigned int height)
{
	this->TextShader = ResourceManager::LoadShader("res/shaders/Text.shader", "text");
	this->TextShader.SetUniformMatrix4fv("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f));
	this->TextShader.SetUniform1i("text", 0);

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TextRenderer::Load(std::string font, unsigned int fontSize)
{
	this->Characters.clear(); // clear previously stored characters
	
	// initialize and load freetype library
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library!" << std::endl;

	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, font.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font!" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, fontSize); // set size to load glyphs

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);// disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++)
	{
		// load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYPE: Failed to load Glyph!" << std::endl;
			continue;
		}

		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// store character for later use
		Character character = {
			texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), face->glyph->advance.x
		};
		glBindTexture(GL_TEXTURE_2D, 0);

		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
	this->TextShader.Bind().SetUniform3f("color", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->VAO);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 0.0f },

			{ xpos,     ypos + h,   0.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}