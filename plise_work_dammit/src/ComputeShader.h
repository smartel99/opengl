#pragma once
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

class ComputeShader {
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	ComputeShader(const std::string& filepath);
	~ComputeShader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3);
	void SetUniform1i(const std::string& name, int value);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
private:
	std::string ParseComputeShader(const std::string& filepath);
	unsigned int CompileComputeShader(unsigned int type, const std::string& source);
	unsigned int CreateComputeShader(const std::string& ComputeShader);

	int GetUniformLocation(const std::string& name);
};
