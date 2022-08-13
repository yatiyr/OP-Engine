#pragma once
#include <string>
#include <functional>
#include <Renderer/Framebuffer.h>
#include <Renderer/Shader.h>

namespace OP
{
	class RenderPass
	{
	public:

		// One framebuffer for each shader pass
		RenderPass(std::string name, FramebufferSpecification spec);
		RenderPass(std::string name, FramebufferSpecification spec, Ref<Shader> shader);
		RenderPass(std::string name, Ref<Framebuffer> framebuffer);
		~RenderPass();
		// Color or depth?
		// This might change and be Texture Class itself
		virtual uint32_t GetColorAttachment(uint32_t index);
		virtual uint32_t GetDepthAttachment(uint32_t index = 0);


		void BindShader(Ref<Shader> shader);

		// We define what draw calls to do by using lambda expressions
		void InvokeCommands(std::function<void(void)> commands);



		Ref<Framebuffer> GetFramebuffer();

		// Temp
		Ref<Shader> m_Shader;

		void FreeFramebuffer();

		virtual void ResizeFramebuffer(uint32_t width, uint32_t height);

	public:
		static Ref<RenderPass> Create(std::string name, FramebufferSpecification spec);
		static Ref<RenderPass> Create(std::string name, FramebufferSpecification spec, Ref<Shader> shader);
		static Ref<RenderPass> Create(std::string name, Ref<Framebuffer> framebuffer);

		// With these functions we can get attachments from other shader passes
		static uint32_t GetColorInput(Ref<RenderPass> inputPass, uint32_t index);
		static uint32_t GetDepthInput(Ref<RenderPass> inputPass, uint32_t index = 0);


	protected:
		std::string m_Name;
		Ref<Framebuffer> m_Framebuffer;
	};
}