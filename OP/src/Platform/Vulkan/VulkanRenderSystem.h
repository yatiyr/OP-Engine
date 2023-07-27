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
		static void Render();
		static void RecreateSwapchain();
	private:
		static void CreateFramebuffers();
		static void CreateCommandBuffer();
	};
}