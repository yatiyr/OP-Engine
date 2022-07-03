
#include <Precomp.h>
#include <Renderer/ShaderPass.h>


namespace OP
{
	ShaderPass::ShaderPass(std::string name, FramebufferSpecification spec) : m_Name(name)
	{
		m_Framebuffer = Framebuffer::Create(spec);
	}

	ShaderPass::ShaderPass(std::string name, FramebufferSpecification spec, Ref<Shader> shader) : m_Name(name)
	{
		m_Framebuffer = Framebuffer::Create(spec);
		m_Shader = shader;
	}

	ShaderPass::ShaderPass(std::string name, Ref<Framebuffer> framebuffer) : m_Name(name)
	{
		m_Framebuffer = framebuffer;
	}

	uint32_t ShaderPass::GetColorAttachment(uint32_t index)
	{
		return m_Framebuffer->GetColorAttachmentRendererID(index);
	}

	uint32_t ShaderPass::GetDepthAttachment(uint32_t index)
	{
		return m_Framebuffer->GetDepthAttachmentRendererID(index);
	}

	uint32_t ShaderPass::GetColorInput(Ref<ShaderPass> inputPass, uint32_t index)
	{
		return inputPass->GetColorAttachment(index);
	}

	uint32_t ShaderPass::GetDepthInput(Ref<ShaderPass> inputPass, uint32_t index)
	{
		return inputPass->GetDepthAttachment(index);
	}

	Ref<Framebuffer> ShaderPass::GetFramebuffer()
	{
		return m_Framebuffer;
	}

	void ShaderPass::BindShader(Ref<Shader> shader)
	{
		m_Shader = shader;
	}

	Ref<ShaderPass> ShaderPass::Create(std::string name, FramebufferSpecification spec)
	{
		return std::make_shared<ShaderPass>(name, spec);
	}

	Ref<ShaderPass> ShaderPass::Create(std::string name, FramebufferSpecification spec, Ref<Shader> shader)
	{
		return std::make_shared<ShaderPass>(name, spec, shader);
	}

	Ref<ShaderPass> ShaderPass::Create(std::string name, Ref<Framebuffer> framebuffer)
	{
		return std::make_shared<ShaderPass>(name, framebuffer);
	}

	void ShaderPass::ResizeFramebuffer(uint32_t width, uint32_t height)
	{
		m_Framebuffer->Resize(width, height);
	}

	void ShaderPass::InvokeCommands(std::function<void(void)> commands)
	{
		m_Framebuffer->Bind();
		commands();
		m_Framebuffer->Unbind();
	}
}
