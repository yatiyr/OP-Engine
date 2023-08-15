#pragma once

#include <vulkan/vulkan.hpp>

namespace OP
{

	class VulkanTexture
	{
	public:
		VulkanTexture(uint32_t width, uint32_t height, unsigned char* data, uint32_t channels);
		~VulkanTexture();

		VkImageView& GetImageView();
		VkSampler& GetSampler();

	private:
		void CreateTextureImageView();
		void CreateSampler();
	private:
		VkSampler m_TextureSampler;
		VkImageView m_TextureImageView;
		VkBuffer m_StagingBuffer;
		VkDeviceMemory m_StagingBufferMemory;
		VkImage m_TextureImage;
		VkDeviceMemory m_TextureImageMemory;
	};
}