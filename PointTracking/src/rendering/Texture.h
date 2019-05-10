#pragma once

#include "Renderer.h"
#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>

class Texture {
private:
	unsigned int m_RendererID;
	cv::Mat m_Texture_Data;
public:
	Texture(const rs2::video_frame& frame);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline cv::Mat* GetData() { return &m_Texture_Data; }
};