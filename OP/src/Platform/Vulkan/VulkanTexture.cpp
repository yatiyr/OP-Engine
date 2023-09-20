#include <Precomp.h>
#include <Platform/Vulkan/VulkanTexture.h>
#include <Platform/Vulkan/VulkanContext.h>
#include <Platform/Vulkan/VulkanBufferUtils.h>
#include <Platform/Vulkan/VulkanUtils.h>
#include <Platform/Vulkan/VulkanTextureUtils.h>

namespace OP
{
	VulkanTexture::VulkanTexture(uint32_t width, uint32_t height, uint32_t mipLevels, unsigned char* data, uint32_t channels)
	{
		uint32_t maxPossibleMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

		if (mipLevels == 0)
			InitializeTexture(width, height, data, channels, maxPossibleMipLevels);
		else if (mipLevels < 1)
			InitializeTexture(width, height, data, channels, 1);
		else if (mipLevels < maxPossibleMipLevels)
			InitializeTexture(width, height, data, channels, mipLevels);
		else
			InitializeTexture(width, height, data, channels, maxPossibleMipLevels);

	}


	VulkanTexture::VulkanTexture(uint32_t width, uint32_t height, unsigned char* data, uint32_t channels)
	{
		// If mip levels is not given to the constructor, we assume that it is 1.
		InitializeTexture(width, height, data, channels, 1);
	}

	VulkanTexture::~VulkanTexture()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();

		vkDestroySampler   (device, m_TextureSampler,     nullptr);
		vkDestroyImageView (device, m_TextureImageView,   nullptr);

		vkDestroyImage     (device, m_TextureImage,       nullptr);
		vkFreeMemory       (device, m_TextureImageMemory, nullptr);
	}

	VkImageView& VulkanTexture::GetImageView()
	{
		return m_TextureImageView;
	}

	VkSampler& VulkanTexture::GetSampler()
	{
		return m_TextureSampler;
	}

	void VulkanTexture::CreateTextureImageView(uint32_t mipLevel)
	{
		VulkanContext* context = VulkanContext::GetContext();
		VkDevice       device  = context->GetDevice();

		m_TextureImageView = TextureUtils::CreateImageView(device,
			                                               m_TextureImage,
			                                               VK_FORMAT_R8G8B8A8_SRGB,
			                                               VK_IMAGE_ASPECT_COLOR_BIT, mipLevel);
	}

	void VulkanTexture::CreateSampler(uint32_t mipLevels)
	{
		VkPhysicalDevice physicalDevice = VulkanContext::GetContext()->GetPhysicalDevice();
		VkDevice                 device = VulkanContext::GetContext()->GetDevice();


		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType            = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter        = VK_FILTER_LINEAR;
		samplerInfo.minFilter        = VK_FILTER_LINEAR;
		samplerInfo.addressModeU     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy    = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor      = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable           = VK_FALSE;
		samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod     = 0.0f;
		samplerInfo.maxLod     = static_cast<float>(mipLevels);

		if (vkCreateSampler(device, &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to create texture sampler!");
		}

	}

	void VulkanTexture::InitializeTexture(uint32_t width, uint32_t height, unsigned char* data, uint32_t channels, uint32_t mipLevels)
	{

		// Get Vulkan context
		VulkanContext* context = VulkanContext::GetContext();

		// Get Necessary objects from the Vulkan Context
		VkPhysicalDevice physicalDevice = context->GetPhysicalDevice();
		VkCommandPool    commandPool    = context->GetCommandPool();
		VkDevice         device         = context->GetDevice();
		VkQueue          queue          = context->GetGraphicsQueue();

		// Calculate size of the texture
		VkDeviceSize size = width * height * channels;

		// Create the buffer for staging
		BufferUtils::CreateBuffer(device,
			physicalDevice,
			size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_StagingBuffer,
			m_StagingBufferMemory);

		// Map memory and copy data to the buffer memory
		void* d;
		vkMapMemory(device, m_StagingBufferMemory, 0, size, 0, &d);
		memcpy(d, data, static_cast<size_t>(size));
		vkUnmapMemory(device, m_StagingBufferMemory);

		// Create the texture image and put get handles of texture image and texture image memory
		TextureUtils::CreateImage(device, physicalDevice,
			width, height, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
			VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_TextureImage, m_TextureImageMemory);

		// Change layout of the image for copying
		TextureUtils::TransitionImageLayout(device, commandPool, queue, m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

		// Copy the texture data into the image
		TextureUtils::CopyBufferToImage(device, commandPool, queue, m_StagingBuffer, m_TextureImage, width, height);

		// Change layout so that the texture can be read from the shader with a sampler
		//TextureUtils::TransitionImageLayout(device, commandPool, queue, m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
  		//	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

		if (mipLevels > 1)
			TextureUtils::GenerateMipMaps(device, physicalDevice, commandPool, queue, m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, width, height, mipLevels);
		else
		{
			TextureUtils::TransitionImageLayout(device, commandPool, queue, m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				mipLevels);
		}


		// We do not need staging buffers anymore so we delete them
		vkDestroyBuffer(device, m_StagingBuffer, nullptr);
		vkFreeMemory(device, m_StagingBufferMemory, nullptr);

		// These calls create image view and the sampler
		CreateTextureImageView(mipLevels);
		CreateSampler(mipLevels);

	}

}