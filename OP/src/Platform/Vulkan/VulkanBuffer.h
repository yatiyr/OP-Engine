#pragma once

#include <vulkan/vulkan.hpp>

namespace OP
{

	namespace Utils
	{
	}
	enum class BufferElementType : uint8_t
	{
		OP_EL_NONE = 0,
		OP_EL_INT,
		OP_EL_INT2,
		OP_EL_INT3,
		OP_EL_INT4,
		OP_EL_FLOAT,
		OP_EL_FLOAT2,
		OP_EL_FLOAT3,
		OP_EL_FLOAT4,
		OP_EL_MAT3,
		OP_EL_MAT4,
		OP_EL_BOOL
	};

	static uint32_t GiveElSize(BufferElementType type)
	{
		switch (type)
		{
			case BufferElementType::OP_EL_INT:    return 4;
			case BufferElementType::OP_EL_INT2:   return 4 * 2;
			case BufferElementType::OP_EL_INT3:   return 4 * 3;
			case BufferElementType::OP_EL_INT4:   return 4 * 4;
			case BufferElementType::OP_EL_FLOAT:  return 4;
			case BufferElementType::OP_EL_FLOAT2: return 4 * 2;
			case BufferElementType::OP_EL_FLOAT3: return 4 * 3;
			case BufferElementType::OP_EL_FLOAT4: return 4 * 4;
			case BufferElementType::OP_EL_MAT3:   return 4 * 9;
			case BufferElementType::OP_EL_MAT4:   return 4 * 16;
			case BufferElementType::OP_EL_BOOL:   return 1;
			default: return 0;
		}
	}

	static VkFormat GiveFormatFromBufferType(BufferElementType type)
	{
		switch (type)
		{
			case BufferElementType::OP_EL_INT:    return VK_FORMAT_R32_SINT;
			case BufferElementType::OP_EL_INT2:   return VK_FORMAT_R32G32_SINT;
			case BufferElementType::OP_EL_INT3:   return VK_FORMAT_R32G32B32_SINT;
			case BufferElementType::OP_EL_INT4:   return VK_FORMAT_R32G32B32A32_SINT;
			case BufferElementType::OP_EL_FLOAT:  return VK_FORMAT_R32_SFLOAT;
			case BufferElementType::OP_EL_FLOAT2: return VK_FORMAT_R32G32_SFLOAT;
			case BufferElementType::OP_EL_FLOAT3: return VK_FORMAT_R32G32B32_SFLOAT;
			case BufferElementType::OP_EL_FLOAT4: return VK_FORMAT_R32G32B32A32_SFLOAT;
			case BufferElementType::OP_EL_BOOL:   return VK_FORMAT_R8_UINT;
		}
	}

	struct VertexInputElement
	{
		std::string Name;
		BufferElementType Type;
		uint32_t Offset;
		uint32_t Size;
		bool Normalized;

		VertexInputElement() {}
		VertexInputElement(BufferElementType type,
			               const std::string& name,
			               bool normalized)
			: Name(name), Type(type), Size(GiveElSize(type)), Offset(0), Normalized(normalized) {}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case BufferElementType::OP_EL_INT:    return 1;
				case BufferElementType::OP_EL_INT2:   return 2;
				case BufferElementType::OP_EL_INT3:   return 3;
				case BufferElementType::OP_EL_INT4:   return 4;
				case BufferElementType::OP_EL_FLOAT:  return 1;
				case BufferElementType::OP_EL_FLOAT2: return 2;
				case BufferElementType::OP_EL_FLOAT3: return 3;
				case BufferElementType::OP_EL_FLOAT4: return 4;
				case BufferElementType::OP_EL_MAT3:   return 9;
				case BufferElementType::OP_EL_MAT4:   return 16;
				case BufferElementType::OP_EL_BOOL:   return 1;
				default: return 0;
			}
		}
	};

	class VertexInput
	{
	public:
		VertexInput() {}
		VertexInput(const std::initializer_list<VertexInputElement>& elements) :
			m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}

		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<VertexInputElement>& GetElements() const { m_Elements; }

		std::vector<VertexInputElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<VertexInputElement>::iterator end() { return m_Elements.end(); }
		std::vector<VertexInputElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<VertexInputElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<VertexInputElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VulkanVertexBuffer
	{
	public:
		VulkanVertexBuffer(void* data, uint32_t size);

		~VulkanVertexBuffer();
		void SetData(void* data, uint32_t size);

		VkBuffer& GetBuffer();
	private:
		void Populate(void* data, uint32_t size);
		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;

		VkBuffer m_StagingBuffer;
		VkDeviceMemory m_StagingBufferMemory;

		VertexInput m_Input;
		
	};

	class VulkanIndexBuffer
	{
	public:
		VulkanIndexBuffer(void* data, uint32_t size);

		~VulkanIndexBuffer();
		void SetData(void* data, uint32_t size);
		VkBuffer& GetBuffer();
	private:
		void Populate(void* data, uint32_t size);
	private:
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexBufferMemory;

		VkBuffer m_StagingBuffer;
		VkDeviceMemory m_StagingBufferMemory;

		
	};
}