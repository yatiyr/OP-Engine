#pragma once

#include <Opium/Core.h>

namespace OP
{

	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RGBA32F,
		SRGB,
		RED_INTEGER,
		SM_VARIANCE32F,

		// Depth/stencil
		DEPTH24STENCIL8,
		
		// For Shadow Maps
		DEPTH16,
		SHADOWMAP_ARRAY_DEPTH,
		SHADOWMAP_OD_ARRAY_DEPTH,

		// For omnidirectional shadow mapping
		CUBEMAP_ARRAY_DEPTH,
		SM_POINT_LIGHT_BLUR,

		// Default
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {}


		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		uint32_t layerCount = MAX_DIR_LIGHTS * MAX_CASCADE_SIZE + MAX_SPOT_LIGHTS;
		uint32_t pointLightLayerCount = MAX_POINT_LIGHTS * 6;
		// TODO: filtering/wrap
	};


	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		FramebufferAttachmentSpecification Attachments;

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
		virtual void BindNoResize() = 0;

		virtual void BindRead() = 0;
		virtual void BindDraw() = 0;

		virtual void Unbind() = 0;

		virtual void GetSize(float& x, float& y) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;


		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint32_t GetDepthAttachmentRendererID(uint32_t index = 0) const = 0;

		// virtual FramebufferSpecification& GetSpecification() = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}