
#include <Precomp.h>
#include <Renderer/RenderPass.h>
#include <glad/glad.h>

namespace OP
{
	RenderPass::RenderPass(std::string name, FramebufferSpecification spec) : m_Name(name)
	{
		m_Framebuffer = Framebuffer::Create(spec);
		m_Framebuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}

	RenderPass::RenderPass(std::string name, FramebufferSpecification spec, Ref<Shader> shader) : m_Name(name)
	{
		m_Framebuffer = Framebuffer::Create(spec);
		m_Shader = shader;
		m_Framebuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}

	RenderPass::RenderPass(std::string name, Ref<Framebuffer> framebuffer) : m_Name(name)
	{
		m_Framebuffer = framebuffer;
		m_Framebuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}

	uint32_t RenderPass::GetColorAttachment(uint32_t index)
	{
		return m_Framebuffer->GetColorAttachmentRendererID(index);
	}

	uint32_t RenderPass::GetDepthAttachment(uint32_t index)
	{
		return m_Framebuffer->GetDepthAttachmentRendererID(index);
	}

	uint32_t RenderPass::GetColorInput(Ref<RenderPass> inputPass, uint32_t index)
	{
		return inputPass->GetColorAttachment(index);
	}

	uint32_t RenderPass::GetDepthInput(Ref<RenderPass> inputPass, uint32_t index)
	{
		return inputPass->GetDepthAttachment(index);
	}

	Ref<Framebuffer> RenderPass::GetFramebuffer()
	{
		return m_Framebuffer;
	}

	void RenderPass::BindShader(Ref<Shader> shader)
	{
		m_Shader = shader;
	}

	Ref<RenderPass> RenderPass::Create(std::string name, FramebufferSpecification spec)
	{
		return std::make_shared<RenderPass>(name, spec);
	}

	Ref<RenderPass> RenderPass::Create(std::string name, FramebufferSpecification spec, Ref<Shader> shader)
	{
		return std::make_shared<RenderPass>(name, spec, shader);
	}

	Ref<RenderPass> RenderPass::Create(std::string name, Ref<Framebuffer> framebuffer)
	{
		return std::make_shared<RenderPass>(name, framebuffer);
	}

	void RenderPass::ResizeFramebuffer(uint32_t width, uint32_t height)
	{
		m_Framebuffer->Resize(width, height);
	}

	void RenderPass::InvokeCommands(std::function<void(void)> commands)
	{
		m_Framebuffer->Bind();
		commands();
		m_Framebuffer->Unbind();
	}
}
