#pragma once

#ifndef REALSENSE_H
#define REALSENSE_H

#include <librealsense2/rs.hpp>

#include "rendering/Texture.h"
#include "rendering/Shader.h"
#include "rendering/VertexArray.h"
#include "rendering/VertexBuffer.h"
#include "rendering/VertexBufferLayout.h"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

class Realsense {
public:
	Realsense();
	~Realsense();

	bool OnUpdate();
	void OnRender();
	void OnImGuiRender();

	std::unique_ptr<rs2::pipeline> pipe;
	std::unique_ptr<Texture> Frame;

	bool has_failed;

private:
	void Print_Error(const rs2::error& e);
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<VertexBuffer> m_VertexBuffer;

	rs2::pipeline_profile m_Profile;

	// Create a orthographic projection matrix which is equivalent to the resolution of the 
	// window.
	glm::mat4 m_Proj;
	// Create a view matrix that will simulate a camera translation.
	// Since there is no such thing as a camera per say, we instead move the scene to accomplish 
	// this effect.
	glm::mat4 m_View;
	// Create a model matrix that will translate the model according to the values of the slider.
	glm::mat4 m_Model;

	float m_DepthScale;
};
#endif
