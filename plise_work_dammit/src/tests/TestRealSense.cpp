#include "TestRealSense.h"
#include "Renderer.h"

#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test {
	TestRealSense::TestRealSense()
		: m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
		m_Model(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
	{
		float textured_square_positions[] = {
			//	Xr,     Yr,   Xt,   Yt
			-1.0f,   -1.0f, 0.0f, 0.0f,	// 0 Bottom Left.
			 1.0f,   -0.0f, 1.0f, 0.0f,	// 1 Bottom Right.
			 1.0f,    0.0f, 1.0f, 1.0f,	// 2 Top Right.
			-1.0f,    0.0f, 0.0f, 1.0f	// 3 Top Left.
		};

		// The values of this array represent the index of a vector in the position array.
		// No matter what type that is chosen for this array, it must be unsigned.
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,
		};

		m_Shader = std::make_unique<Shader>("res/shaders/Texture.shader");
		m_VAO = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>(textured_square_positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;

		layout.Push<float>(2);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

		// RealSense init stuff
		// Create a pipeline to config and init camera.
		m_Pipe = std::make_unique<rs2::pipeline>();
		// Start first device with its default stream.
		// The function returns the pipeline profile which the pipeline used to start the device.
		m_Profile = std::make_unique<rs2::pipeline_profile>();
		*m_Profile = m_Pipe->start();
	}
	TestRealSense::~TestRealSense()
	{
	}
	void TestRealSense::OnUpdate(float deltaTime)
	{
	}
	void TestRealSense::OnRender()
	{
	}
	void TestRealSense::OnImGuiRender()
	{
	}
}

