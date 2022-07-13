#include <Precomp.h>
#include "PingPongRenderPass.h"
#include <glad/glad.h>

namespace OP
{
	PingPongRenderPass::PingPongRenderPass(std::string name, FramebufferSpecification spec)
	 : RenderPass(name, spec) {
		m_PingPongFramebuffer = Framebuffer::Create(spec);
		m_PingPongFramebuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}
	PingPongRenderPass::PingPongRenderPass(std::string name, FramebufferSpecification spec, Ref<Shader> shader)
	 : RenderPass(name, spec, shader) {

		// We create another framebuffer for ping pong stuff as well.
		m_PingPongFramebuffer = Framebuffer::Create(spec);
		m_PingPongFramebuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}

	Ref<Framebuffer> PingPongRenderPass::GetPingPongFramebuffer()
	{
		return m_PingPongFramebuffer;
	}

	void PingPongRenderPass::ResizeFramebuffer(uint32_t width, uint32_t height)
	{
		// This time, we resize both of the framebuffers
		m_Framebuffer->Resize(width, height);
		m_PingPongFramebuffer->Resize(width, height);
	}

	void PingPongRenderPass::InvokeCommandsPP(std::function<void(void)> commands1, std::function<void(void)> commands2)
	{
		m_Framebuffer->Bind();
		commands1();
		m_PingPongFramebuffer->Bind();
		commands2();
		m_PingPongFramebuffer->Unbind();
	}

	uint32_t PingPongRenderPass::GetDepthAttachmentPP(uint32_t index)
	{
		return m_PingPongFramebuffer->GetDepthAttachmentRendererID(index);
	}

	uint32_t PingPongRenderPass::GetColorAttachmentPP(uint32_t index)
	{
		return m_PingPongFramebuffer->GetColorAttachmentRendererID(index);
	}

	Ref<PingPongRenderPass> PingPongRenderPass::Create(std::string name, FramebufferSpecification spec)
	{
		return std::make_shared<PingPongRenderPass>(name, spec);
	}

	Ref<PingPongRenderPass> PingPongRenderPass::Create(std::string name, FramebufferSpecification spec, Ref<Shader> shader)
	{
		return std::make_shared<PingPongRenderPass>(name, spec, shader);
	}

	uint32_t PingPongRenderPass::GetColorInput(Ref<PingPongRenderPass> inputPass, uint32_t index)
	{
		return inputPass->GetColorAttachment(index);
	}

	uint32_t PingPongRenderPass::GetDepthInput(Ref<PingPongRenderPass> inputPass, uint32_t index)
	{
		return inputPass->GetDepthAttachment(index);
	}

	uint32_t PingPongRenderPass::GetColorInputPP(Ref<PingPongRenderPass> inputPass, uint32_t index)
	{
		return inputPass->GetColorAttachmentPP(index);
	}

	uint32_t PingPongRenderPass::GetDepthInputPP(Ref<PingPongRenderPass> inputPass, uint32_t index)
	{
		return inputPass->GetDepthAttachmentPP(index);
	}

}
