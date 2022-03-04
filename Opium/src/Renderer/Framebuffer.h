#pragma once

#include <Opium/Core.h>

namespace Opium
{
	struct FramebufferSpecification
	{
		uint32_t Width, Height;

		// FramebufferFormat Format =

		uint32_t Samples = 1;

		// if it is true, this means glBindFramebuffer(0) for opengl for example
		// eren understands it :D
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() {}
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;

		// virtual FramebufferSpecification& GetSpecification() = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}