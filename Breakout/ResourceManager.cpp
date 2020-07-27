#include "ResourceManager.h"
#include "stb_image.h"

#include <iostream>
#include <sstream>
#include <fstream>

std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture2D> ResourceManager::Textures;

Shader ResourceManager::LoadShader(const std::string& filepath, std::string name)
{
	Shaders[name] = loadShaderFromFile(filepath);
	return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, std::string name)
{
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
	return Textures[name];
}

void ResourceManager::Clear()
{
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);

	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const std::string& filepath)
{
	enum ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
	};
	
	std::string vertexCode;
	std::string fragmentCode;

	std::ifstream shaderFile(filepath);
	std::string line;
	std::stringstream shaderStream[3];
	ShaderType type = ShaderType::NONE;

	try
	{

		while (getline(shaderFile, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("fragment") != std::string::npos)
					type = ShaderType::FRAGMENT;
				else if (line.find("geometry") != std::string::npos)
					type = ShaderType::GEOMETRY;
			}
			else
			{
				shaderStream[(int)type] << line << '\n';
			}
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files!" << std::endl;
	}

	Shader shader;
	shader.Compile(shaderStream[0].str(), shaderStream[1].str(), shaderStream[2].str());
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
	Texture2D texture;
	if (alpha)
	{
		texture.Internal_Format = GL_RGBA;
		texture.Image_Format = GL_RGBA;
	}

	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
	texture.Generate(width, height, data);
	stbi_image_free(data);
	return texture;
}