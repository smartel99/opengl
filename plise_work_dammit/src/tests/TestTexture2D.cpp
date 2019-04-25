#include "TestTexture2D.h"
#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test {
	TestTexture2D::TestTexture2D()
		: m_translationA(200, 200, 0), m_translationB(400, 200, 0),
		m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
	{
		float textured_square_positions[] = {
			//	Xr,     Yr,   Xt,   Yt
			-50.0f, -50.0f, 0.0f, 0.0f,	// 0 Bottom Left.
			 50.0f, -50.0f, 1.0f, 0.0f,	// 1 Bottom Right.
			 50.0f,  50.0f, 1.0f, 1.0f,	// 2 Top Right.
			-50.0f,  50.0f, 0.0f, 1.0f	// 3 Top Left.
		};

		// The values of this array represent the index of a vector in the position array.
		// No matter what type that is chosen for this array, it must be unsigned.
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_Shader = std::make_unique<Shader>("res/shaders/Texture.shader");
		m_VAO = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>(textured_square_positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;

		layout.Push<float>(2);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);


		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

		m_Texture = std::make_unique<Texture>("res/textures/rald.png");
		m_Shader->Bind();
		m_Shader->SetUniform1i("u_Texture", 0);// Set the uniform to be the desired texture, in this
												// case the one in texture slot 0.
	}

	TestTexture2D::~TestTexture2D()
	{
	}
	void TestTexture2D::OnUpdate(float deltaTime)
	{
	}
	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(RENDER_COLOR_BLACK));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		m_Texture->Bind();

		{
			// Create a model matrix that will translate the model according to the values of the slider.
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translationA);
			// Create a model view projection matrix.
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}
		{
			// Create a model matrix that will translate the model according to the values of the slider.
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translationB);
			// Create a model view projection matrix.
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}
	}
	void TestTexture2D::OnImGuiRender()
	{
		ImGui::SliderFloat3("TranslationA", &m_translationA.x, -960.0f, 960.0f);
		ImGui::SliderFloat3("TranslationB", &m_translationB.x, -960.0f, 960.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}

