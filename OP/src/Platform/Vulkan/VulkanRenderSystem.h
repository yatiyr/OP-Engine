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
	private:
		void CreateFramebuffers();
	};
}