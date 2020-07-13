#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

Shader& Shader::Bind()
{
	glUseProgram(this->ID);
	return *this;
}

void Shader::Compile(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource)
{
	unsigned int sVertex, sFragment, sGeometry;

	// vertex
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	const char* src = vertexSource.c_str();
	glShaderSource(sVertex, 1, &src, NULL);
	glCompileShader(sVertex);
	CheckCompileErrors(sVertex, "VERTEX");

	// fragment
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	src = fragmentSource.c_str();
	glShaderSource(sFragment, 1, &src, NULL);
	glCompileShader(sFragment);
	CheckCompileErrors(sFragment, "FRAGMENT");

	// geometry
	if (geometrySource != "")
	{
		sGeometry = glCreateShader(GL_GEOMETRY_SHADER);
		src = geometrySource.c_str();
		glShaderSource(sGeometry, 1, &src, NULL);
		glCompileShader(sGeometry);
		CheckCompileErrors(sGeometry, "GEOMETRY");
	}

	this->ID = glCreateProgram();
	glAttachShader(this->ID, sVertex);
	glAttachShader(this->ID, sFragment);
	if (geometrySource != "")
		glAttachShader(this->ID, sGeometry);
	glLinkProgram(this->ID);
	CheckCompileErrors(this->ID, "PROGRAM");

	// delete shaders
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (geometrySource != "")
		glDeleteShader(sGeometry);
}

void Shader::SetUniformMatrix4fv(const std::string& name, const glm::mat4& mat)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform4f(const std::string& name, glm::vec4 value)
{
	glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform3f(const std::string& name, glm::vec3 value)
{
	glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
	glUniform2f(GetUniformLocation(name), v0, v1);
}

void Shader::SetUniform2f(const std::string& name, glm::vec2 value)
{
	glUniform2f(GetUniformLocation(name), value.x, value.y);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	int location = glGetUniformLocation(ID, name.c_str());
	if (location == -1)
		std::cout << "Warning: uniform " << name << " doesn't exist!" << std::endl;

	m_UniformLocationCache[name] = location;
	return location;
}

void Shader::CheckCompileErrors(unsigned int object, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type : " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Link-time error: Type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}