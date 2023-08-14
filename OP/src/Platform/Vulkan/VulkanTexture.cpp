#include <Precomp.h>
#include <Platform/Vulkan/VulkanTexture.h>
#include <Platform/Vulkan/VulkanBufferUtils.h>
#include <Platform/Vulkan/VulkanContext.h>
#include <Platform/Vulkan/VulkanUtils.h>

namespace OP
{
	VulkanTexture::VulkanTexture(uint32_t width, uint32_t height, unsigned char* data, uint32_t channels)
	{

		VkDevice device = VulkanContext::GetContext()->GetDevice();
		VkDeviceSize size = width * height * channels;


		BufferUtils::CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
								  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
								      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
								  m_StagingBuffer,
								  m_StagingBufferMemory);

		void* d;
		vkMapMemory(device, m_StagingBufferMemory, 0, size, 0, &d);
			memcpy(d, data, static_cast<size_t>(size));
		vkUnmapMemory(device, m_StagingBufferMemory);

		BufferUtils::CreateImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
								VK_IMAGE_USAGE_TRANSFER_DST_BIT | 
									VK_IMAGE_USAGE_SAMPLED_BIT,
			                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
								m_TextureImage, m_TextureImageMemory);

		BufferUtils::TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		BufferUtils::CopyBufferToImage(m_StagingBuffer, m_TextureImage, width, height);

		BufferUtils::TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device, m_StagingBuffer, nullptr);
		vkFreeMemory(device, m_StagingBufferMemory, nullptr);

		CreateTextureImageView();
		CreateSampler();
	}

	VulkanTexture::~VulkanTexture()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();

		vkDestroySampler(device, m_TextureSampler, nullptr);
		vkDestroyImageView(device, m_TextureImageView, nullptr);

		vkDestroyImage(device, m_TextureImage, nullptr);
		vkFreeMemory(device, m_TextureImageMemory, nullptr);
	}

	void VulkanTexture::CreateTextureImageView()
	{
		m_TextureImageView = VulkanUtils::CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB);
	}

	void VulkanTexture::CreateSampler()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		VkPhysicalDevice physicalDevice = VulkanContext::GetContext()->GetPhysicalDevice();

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(device, &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to create texture sampler!");
		}

	}

}