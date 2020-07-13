#pragma once

#include <GLAD/glad.h>
#include <GLM/glm.hpp>
#include <string>
#include <unordered_map>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_FilePath;
	std::unordered_map<std::string, int> m_UniformLocationCache;

public:
	unsigned int ID;
	Shader() {}
	Shader &Use();
	
	//void Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
	void Compile(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource = "");

	void SetUniformMatrix4fv(const std::string& name, const glm::mat4& mat);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform4f(const std::string& name, glm::vec4 value);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform3f(const std::string& name, glm::vec3 value);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform2f(const std::string& name, glm::vec2 value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform1i(const std::string& name, int value);
	
private:
	int GetUniformLocation(const std::string& name);
	void CheckCompileErrors(unsigned int object, std::string type);
};