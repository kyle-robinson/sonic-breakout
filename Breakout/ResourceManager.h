#pragma once

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <GLAD/glad.h>
#include <string>
#include <map>

#include "Texture2D.h"
#include "Shader.h"

class ResourceManager
{
public:
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Texture2D> Textures;

	static Shader LoadShader(const std::string& filepath, std::string name);
	static Shader GetShader(std::string name);

	static Texture2D LoadTexture(const char* file, bool alpha, std::string name);
	static Texture2D GetTexture(std::string name);

	static void Clear();
private:
	ResourceManager();
	static Shader loadShaderFromFile(const std::string& filepath);
	static Texture2D loadTextureFromFile(const char* file, bool alpha);
};

#endif