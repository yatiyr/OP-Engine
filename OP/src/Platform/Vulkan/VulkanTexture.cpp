#include <Precomp.h>
#include <Platform/Vulkan/VulkanTexture.h>
#include <Platform/Vulkan/VulkanContext.h>
#include <Platform/Vulkan/VulkanBufferUtils.h>
#include <Platform/Vulkan/VulkanUtils.h>
#include <Platform/Vulkan/VulkanTextureUtils.h>

namespace OP
{
	VulkanTexture::VulkanTexture(uint32_t width, uint32_t height, unsigned char* data, uint32_t channels)
	{

		VulkanContext* context = VulkanContext::GetContext();
		VkDevice device = context->GetDevice();
		VkPhysicalDevice physicalDevice = context->GetPhysicalDevice();
		VkCommandPool commandPool = context->GetCommandPool();
		VkQueue queue = context->GetGraphicsQueue();

		VkDeviceSize size = width * height * channels;


		BufferUtils::CreateBuffer(device,
			                      physicalDevice,
			                      size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
								  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
								      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
								  m_StagingBuffer,
								  m_StagingBufferMemory);

		void* d;
		vkMapMemory(device, m_StagingBufferMemory, 0, size, 0, &d);
			memcpy(d, data, static_cast<size_t>(size));
		vkUnmapMemory(device, m_StagingBufferMemory);

		TextureUtils::CreateImage(device, physicalDevice,
			                      width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
								  VK_IMAGE_USAGE_TRANSFER_DST_BIT | 
									VK_IMAGE_USAGE_SAMPLED_BIT,
			                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
								  m_TextureImage, m_TextureImageMemory);

		TextureUtils::TransitionImageLayout(device, commandPool, queue, m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		TextureUtils::CopyBufferToImage(device, commandPool, queue, m_StagingBuffer, m_TextureImage, width, height);

		TextureUtils::TransitionImageLayout(device, commandPool, queue, m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
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

	VkImageView& VulkanTexture::GetImageView()
	{
		return m_TextureImageView;
	}

	VkSampler& VulkanTexture::GetSampler()
	{
		return m_TextureSampler;
	}

	void VulkanTexture::CreateTextureImageView()
	{
		VulkanContext* context = VulkanContext::GetContext();
		VkDevice device = context->GetDevice();

		m_TextureImageView = TextureUtils::CreateImageView(device, m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
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