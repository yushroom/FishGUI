#pragma once

#include "../Math.hpp"

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
			return m_colorbuffer;
		}

	protected:
		bool m_initialized = false;
		FishGUI::Size m_size;
		unsigned int m_framebuffer;
		unsigned int m_colorbuffer;
		unsigned int m_depthbuffer;
	};
}
