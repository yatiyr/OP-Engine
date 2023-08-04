#pragma once

#include <vulkan/vulkan.hpp>
#include <Op/ResourceManager.h>

namespace OP
{
	class VulkanRenderSystem
	{
	public:
		static void Init();
		static void Cleanup();
		static void Render(float ts);
		static void RecreateSwapchain();
		static void UpdateUniformBuffer(uint32_t currentImage, float ts);
	private:
		static void CreateFramebuffers();
		static void CreateCommandBuffer();

		static void CreateDescriptorPool();
		static void CreateDescriptorSets();
	};
}