// http://antongerdelan.net/opengl/compute.html

#include "TestComputeShader.h"
#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

namespace test {
	TestComputeShader::TestComputeShader()
		: m_translationA(0, 0, 0), m_translationB(0, 0, 0),
		m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
	{
		float textured_square_positions[] = {
			//  Xr,     Yr,   Xt,   Yt
			  0.0f,   0.0f, 0.0f, 0.0f,	// 0 Bottom Left.
			960.0f,   0.0f, 1.0f, 0.0f,	// 1 Bottom Right.
			960.0f, 540.0f, 1.0f, 1.0f,	// 2 Top Right.
			  0.0f, 540.0f, 0.0f, 1.0f	// 3 Top Left.
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

		int work_grp_cnt[3];

		GLCall(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_cnt[0]));
		GLCall(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_cnt[1]));
		GLCall(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_cnt[2]));

		std::cout << "Max global (total) work group size \n\tx: " << work_grp_cnt[0] <<
			"\n\ty: " << work_grp_cnt[1] <<
			"\n\tz: " << work_grp_cnt[2] << std::endl;
	}

	TestComputeShader::~TestComputeShader()
	{
	}
	void TestComputeShader::OnUpdate(float deltaTime)
	{
	}
	void TestComputeShader::OnRender()
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
	}
	void TestComputeShader::OnImGuiRender()
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}

