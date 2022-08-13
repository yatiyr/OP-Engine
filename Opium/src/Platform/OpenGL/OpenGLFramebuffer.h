#pragma once

#include <Renderer/Framebuffer.h>

namespace OP
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();
		virtual void FreeFramebuffer() override;

		virtual void Bind() override;
		virtual void BindRead() override;
		virtual void BindDraw() override;
		virtual void BindNoResize() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y)  override;


		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { return m_ColorAttachments[index]; }
		virtual uint32_t GetDepthAttachmentRendererID(uint32_t index = 0) const override { return m_DepthAttachment; }
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

		virtual void GetSize(float& x, float &y) override;
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_RenderBufferID = 0;
		FramebufferSpecification m_Specification;


		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;


		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}