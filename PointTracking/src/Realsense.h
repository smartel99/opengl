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

using namespace cv;

class Realsense {
public:
	Realsense(bool d = false);
	~Realsense();

	bool OnUpdate();
	void OnRender();
	void OnImGuiRender();

	std::unique_ptr<rs2::pipeline> pipe;
	std::unique_ptr<Texture> Frame;

	inline bool get_has_failed() const { return m_has_failed; }

private:
	void Print_Error(const rs2::error& e);
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<VertexBuffer> m_VertexBuffer;

	rs2::pipeline_profile m_Profile;
	// rs2::align allows to perform alignement of depth frames to other frames.
	rs2::align m_align;
	float m_DepthScale;

	Ptr<BackgroundSubtractorKNN> m_bg_sub;

	// Create a orthographic projection matrix which is equivalent to the resolution of the 
	// window.
	glm::mat4 m_Proj;

	bool m_has_failed;
	bool m_debug;
	bool m_emitter_activated;

	int m_thresh;
	int m_thresh_multiplier;
	int m_max_recursion;

	Rect m_detected_zone;
	Point3f m_detected_zone_coordinate;

	Rect FindBiggestContour(const Mat* src, Rect loc, int r);
	int GetMaxAreaContourId(std::vector<std::vector<Point>> contours);
	void getXYPoint(Mat src, Rect loc);
	double getZPoint(const Mat& src, Point loc, int r);
	void toggleEmitter();
};
#endif
