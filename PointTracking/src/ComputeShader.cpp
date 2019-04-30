#include "ComputeShader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

#include "Renderer.h"


ComputeShader::ComputeShader(const std::string& filepath)
	:m_FilePath(filepath), m_RendererID(0)
{
	std::string source = ParseComputeShader(filepath);
	m_RendererID = CreateComputeShader(source);
}

ComputeShader::~ComputeShader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

unsigned int ComputeShader::CompileComputeShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str(); // Equivalent to &source[0].
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " ComputeShader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;

}

unsigned int ComputeShader::CreateComputeShader(const std::string & ComputeShader)
{
	unsigned int program = glCreateProgram();
	unsigned int cs = CompileComputeShader(GL_COMPUTE_SHADER, ComputeShader);

	glAttachShader(program, cs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(cs);

	return program;
}

std::string ComputeShader::ParseComputeShader(const std::string & filepath)
{
	std::ifstream stream(filepath);

	enum class ComputeShaderType {
		NONE = -1, COMPUTE =  0
	};

	std::string line;
	std::stringstream ss[1];

	ComputeShaderType type = ComputeShaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("shader") != std::string::npos) {
			if (line.find("compute") != std::string::npos) {
				type = ComputeShaderType::COMPUTE;
			}
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}

	return ss[0].str();
}

void ComputeShader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void ComputeShader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void ComputeShader::SetUniform4f(const std::string & name, float f0, float f1, float f2, float f3)
{
	GLCall(glUniform4f(GetUniformLocation(name), f0, f1, f2, f3));
}

void ComputeShader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void ComputeShader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int ComputeShader::GetUniformLocation(const std::string & name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: '" << name << "' doesn't exist!" << std::endl;

	m_UniformLocationCache[name] = location;
	return location;
}
