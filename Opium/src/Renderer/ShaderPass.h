
#include <string>
#include <functional>
#include <Renderer/Framebuffer.h>
#include <Renderer/Shader.h>

namespace OP
{
	class ShaderPass
	{
	public:

		// One framebuffer for each shader pass
		ShaderPass(std::string name, FramebufferSpecification spec);
		ShaderPass(std::string name, FramebufferSpecification spec, Ref<Shader> shader);
		ShaderPass(std::string name, Ref<Framebuffer> framebuffer);

		// Color or depth?
		// This might change and be Texture Class itself
		uint32_t GetColorAttachment(uint32_t index);
		uint32_t GetDepthAttachment(uint32_t index = 0);


		void BindShader(Ref<Shader> shader);

		// We define what draw calls to do by using lambda expressions
		void InvokeCommands(std::function<void(void)> commands);



		Ref<Framebuffer> GetFramebuffer();

		// Temp
		Ref<Shader> m_Shader;


		void ResizeFramebuffer(uint32_t width, uint32_t height);

	public:
		static Ref<ShaderPass> Create(std::string name, FramebufferSpecification spec);
		static Ref<ShaderPass> Create(std::string name, FramebufferSpecification spec, Ref<Shader> shader);
		static Ref<ShaderPass> Create(std::string name, Ref<Framebuffer> framebuffer);

		// With these functions we can get attachments from other shader passes
		static uint32_t GetColorInput(Ref<ShaderPass> inputPass, uint32_t index);
		static uint32_t GetDepthInput(Ref<ShaderPass> inputPass, uint32_t index = 0);


	private:
		std::string m_Name;
		Ref<Framebuffer> m_Framebuffer;
	};
}