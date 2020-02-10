#pragma once

#include "../Math.hpp"
#include <cassert>

namespace FishGUI
{
	class FrameBuffer
	{
	public:
		FrameBuffer() = default;
		~FrameBuffer();

		void Init(int width, int height);

		void Resize(int width, int height);

		void Bind();

		void Unbind();

		FishGUI::Size GetSize() const
		{
			return m_size;
		}

		unsigned int GetColorTexture() const
		{
			if (m_enableMSAA)
				return m_colorbuffer2;
			return m_colorbuffer;
		}

		unsigned int GetFramebuffer() const
		{
			return m_framebuffer;
		}

		void EnableMSAA()
		{
			assert(!m_enableMSAA);
			m_enableMSAA = true;
		}

	protected:
		bool m_initialized = false;
		FishGUI::Size m_size;
		unsigned int m_framebuffer = 0;
		unsigned int m_colorbuffer = 0;
		unsigned int m_depthbuffer = 0;

		bool m_enableMSAA = false;
		unsigned int m_framebuffer2 = 0;	// resolve multi sample framebuffer
		unsigned int m_colorbuffer2 = 0;	// resolve multi sample framebuffer
	};
}
