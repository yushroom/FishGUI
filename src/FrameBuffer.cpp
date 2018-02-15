#include <FishGUI/Render/FrameBuffer.hpp>
#include <FishGUI/GLEnvironment.hpp>
#include <iostream>

namespace FishGUI
{

	FrameBuffer::FrameBuffer(int width, int height) : m_size{ width, height }
	{
		// framebuffer configuration
		// -------------------------
		//unsigned int framebuffer;
		glGenFramebuffers(1, &m_framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		// create a color attachment texture
		//unsigned int textureColorbuffer;
		glGenTextures(1, &m_colorbuffer);
		glBindTexture(GL_TEXTURE_2D, m_colorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.width, m_size.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorbuffer, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		//unsigned int rbo;
		glGenRenderbuffers(1, &m_depthbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_size.width, m_size.height); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthbuffer); // now actually attach it
																									  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			abort();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_framebuffer);
		glDeleteTextures(1, &m_colorbuffer);
		glDeleteRenderbuffers(1, &m_depthbuffer);
	}

	void FrameBuffer::Resize(int width, int height)
	{
		m_size.width = width;
		m_size.height = height;
		glBindTexture(GL_TEXTURE_2D, m_colorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.width, m_size.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_size.width, m_size.height); // use a single renderbuffer object for both a depth AND stencil buffer.
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
