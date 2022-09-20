#pragma once

#include <Renderer/RenderPass.h>
namespace OP
{
	class PingPongRenderPass : public RenderPass
	{
	public:

		// One framebuffer for each shader pass
		PingPongRenderPass(std::string name, FramebufferSpecification spec);
		PingPongRenderPass(std::string name, FramebufferSpecification spec, Ref<Shader> shader);


		Ref<Framebuffer> GetPingPongFramebuffer();

		virtual void ResizeFramebuffer(uint32_t width, uint32_t height) override;

		void InvokeCommandsPP(std::function<void(void)> commands1, std::function<void(void)> commands2);


		uint32_t GetDepthAttachmentPP(uint32_t index);
		uint32_t GetColorAttachmentPP(uint32_t index);

	public:
		static Ref<PingPongRenderPass> Create(std::string name, FramebufferSpecification spec);
		static Ref<PingPongRenderPass> Create(std::string name, FramebufferSpecification spec, Ref<Shader> shader);


		static uint32_t GetColorInput(Ref<PingPongRenderPass> inputPass, uint32_t index);
		static uint32_t GetDepthInput(Ref<PingPongRenderPass> inputPass, uint32_t index = 0);

		static uint32_t GetColorInputPP(Ref<PingPongRenderPass> inputPass, uint32_t index);
		static uint32_t GetDepthInputPP(Ref<PingPongRenderPass> inputPass, uint32_t index = 0);
		

	private:
		Ref<Framebuffer> m_PingPongFramebuffer;
	};
}