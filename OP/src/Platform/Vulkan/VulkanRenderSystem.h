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
	private:
		static void CreateFramebuffers();
		static void CreateCommandBuffer();
	};
}