#pragma once

#include "Test.h"

#include "ComputeShader.h"
#include <memory>

namespace test {
	class TestComputeShader :public Test {
	public:
		TestComputeShader();
		~TestComputeShader();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;


		unsigned int m_ComputeShader;
		unsigned int m_ComputeProgram;


		glm::vec3 m_translationA, m_translationB;
		// Create a orthographic projection matrix which is equivalent to the resolution of the 
		// window.
		glm::mat4 m_Proj;
		// Create a view matrix that will simulate a camera translation.
		// Since there is no such thing as a camera per say, we instead move the scene to accomplish 
		// this effect.
		glm::mat4 m_View;

	};
}