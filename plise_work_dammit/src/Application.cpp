// http://docs.gl/ 
// OpenGl Documentation.


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void HSVtoRGB(int H, float S, float V, float* r, float* g, float* b);

int main(void) {
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Blinky Recty", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error at glewInit!" << std::endl;

	std::cout << "Using OpenGL Version " << glGetString(GL_VERSION) << std::endl;
	{
		// Create an openGL buffer.
		float positions[] = {
			-0.5f, -0.5f,	// 0
			 0.5f, -0.5f,	// 1
			 0.5f,  0.5f,	// 2
			-0.5f,  0.5f	// 3
		};

		// The values of this array represent the index of a vector in the position array.
		// No matter what type that is chosen for this array, it must be unsigned.
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};		

		VertexArray va;
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();

		shader.SetUniform4f("u_Color", 1.f, 1.f, 1.f, 1.f);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;
		float sat = 1.0f;	// HSV Saturation.
		int hue = 0;		// HSV Hue.

		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 1.0f;
		float increment = 0.01f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window)) {
			/* Render here */
			renderer.Clear();

			shader.Bind();
			shader.SetUniform4f("u_Color", r, g, b, a);

			vb.Bind();
			vb.UpdateBufferData(positions, 4 * 2 * sizeof(float));

			renderer.Draw(va, ib, shader);

			if (hue > 360)
				hue = 0;

			HSVtoRGB(hue, sat, 1.0f, &r, &g, &b);
			hue++;

			if (positions[1] > 0.0f) {
				increment = -0.01f;
			}
			else if (positions[1] < -1.0f) {
				increment = 0.01f;
			}
			for (int i = 0; i < 8; i++) {
				if (i % 2) {
					positions[i] += increment;
				}
			}
			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}
	glfwTerminate();
	return 0;
}

void HSVtoRGB(int H, float S, float V, float* r, float* g, float* b)
{
	float C = S * V;
	float X = C * (1 - float(abs(fmod(H / 60.0, 2) - 1)));
	float m = V - C;
	float Rs, Gs, Bs;

	if (H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;
	}
	else if (H >= 60 && H < 120) {
		Rs = X;
		Gs = C;
		Bs = 0;
	}
	else if (H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;
	}
	else if (H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;
	}
	else if (H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}

	*r = (Rs + m);
	*g = (Gs + m);
	*b = (Bs + m);
}
