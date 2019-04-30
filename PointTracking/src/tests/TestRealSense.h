#pragma once
#ifndef TESTREALSENSE_H
#define TESTREALSENSE_H
#include "Test.h"
#include <librealsense2/rs.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace test {
	struct Pixel {
		int Slot;
		int x;
		int y;
	};

	class TestRealSense :public Test {
	public:
		TestRealSense();
		~TestRealSense();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		void Print_Error(const rs2::error& e);
		cv::Mat frame_to_mat(const rs2::frame& f);
		void FindCorners(cv::Mat src);
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;

		std::unique_ptr<rs2::pipeline> m_Pipe;
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
}
#endif