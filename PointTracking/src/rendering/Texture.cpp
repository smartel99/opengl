#include "Texture.h"
#include <iostream>

Texture::Texture(const rs2::video_frame& frame)
	: m_RendererID(0), m_LocalBuffer(nullptr),
	m_Width(0), m_Height(0), m_BPP(0), m_Texture_Data()
{
	// Load the image into the local buffer.
	m_LocalBuffer = reinterpret_cast<unsigned char*>(const_cast<void*>(frame.get_data()));
	m_Width = frame.get_width();
	m_Height = frame.get_height();
	m_BPP = frame.get_bytes_per_pixel();
	ASSERT(m_BPP == 3);
	m_Texture_Data = cv::Mat(cv::Size(m_Width, m_Height), CV_8UC3, (void*)frame.get_data(), cv::Mat::AUTO_STEP);

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	// !!! The next four parameters are mandatory to render the texture properly !!!
	// What to do if the texture is bigger than the render area.
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

	// What to do if the texture is smaller than the render area.
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	// -----------------------------------------------------------------------------

	// Send the texture data to OpenGL.
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE,
		m_LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
