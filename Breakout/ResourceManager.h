#pragma once
#include <GLAD/glad.h>
#include <string>
#include <map>

#include "Texture2D.h"

class ResourceManager
{
public:
	static std::map<std::string, Texture2D> Textures;

	static Texture2D LoadTexture(const char* file, bool alpha, std::string name);
	static Texture2D GetTexture(std::string name);

	static void Clear();
private:
	ResourceManager();
	static Texture2D loadTextureFromFile(const char* file, bool alpha);
};