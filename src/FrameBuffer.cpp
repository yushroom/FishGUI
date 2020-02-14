#include <FishGUI/Render/FrameBuffer.hpp>
#include <FishGUI/GLEnvironment.hpp>
#include <iostream>

#include <cassert>

namespace FishGUI
{

	void FrameBuffer::Init(int width, int height)
	{
		m_size.width = width;
		m_size.height = height;
		glEnable(GL_MULTISAMPLE);
		// framebuffer configuration
		// -------------------------
		glGenFramebuffers(1, &m_framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		// create a color attachment texture
		glGenTextures(1, &m_colorbuffer);
		auto target = m_enableMSAA ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		glBindTexture(target, m_colorbuffer);
		if (m_enableMSAA)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, m_size.width, m_size.height, GL_TRUE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.width, m_size.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		glBindTexture(target, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		//unsigned int rbo;
		glGenRenderbuffers(1, &m_depthbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthbuffer);
		if (m_enableMSAA)
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, m_size.width, m_size.height);
		else
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_size.width, m_size.height); // use a single renderbuffer object for both a depth AND stencil buffer.
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, m_colorbuffer, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthbuffer); // now actually attach it

		// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			abort();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (m_enableMSAA)
		{
			glGenFramebuffers(1, &m_framebuffer2);
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer2);
			glGenTextures(1, &m_colorbuffer2);
			glBindTexture(GL_TEXTURE_2D, m_colorbuffer2);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.width, m_size.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorbuffer2, 0);
			glCheckError();
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
				abort();
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		m_initialized = true;
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_framebuffer);
		glDeleteTextures(1, &m_colorbuffer);
		glDeleteRenderbuffers(1, &m_depthbuffer);
	}

	void FrameBuffer::Resize(int width, int height)
	{
		if (m_size.width == width && m_size.height == height)
			return;
		m_size.width = width;
		m_size.height = height;
		auto target = m_enableMSAA ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		glBindTexture(target, m_colorbuffer);
		if (m_enableMSAA)
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, m_size.width, m_size.height, GL_TRUE);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.width, m_size.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(target, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthbuffer);
		if (m_enableMSAA)
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, m_size.width, m_size.height);
		else
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_size.width, m_size.height); // use a single renderbuffer object for both a depth AND stencil buffer.
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_size.width, m_size.height); // use a single renderbuffer object for both a depth AND stencil buffer.
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		if (m_enableMSAA)
		{
			glBindTexture(GL_TEXTURE_2D, m_colorbuffer2);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.width, m_size.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glCheckError();
	}

	void FrameBuffer::Bind()
	{
//		GLint defaultFBO;
//		GLint defaultRBO;
//		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
//		glGetIntegerv(GL_RENDERBUFFER_BINDING, &defaultRBO);
//		assert(defaultFBO == 0);
//		assert(defaultRBO == 0);

		assert(m_initialized);
		glCheckError();
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	}

	void FrameBuffer::Unbind()
	{
		if (m_enableMSAA)
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer2);
			glBlitFramebuffer(0, 0, m_size.width, m_size.height, 0, 0, m_size.width, m_size.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCheckError();
	}
}
