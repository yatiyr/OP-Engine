#include <Precomp.h>
#include <Platform/Vulkan/VulkanGraphicsPipeline.h>

namespace OP
{

	VkShaderStageFlagBits GiveShaderStage(uint32_t stage)
	{
		switch (stage)
		{
			case 0:
				return VK_SHADER_STAGE_VERTEX_BIT;
			case 1:
				return VK_SHADER_STAGE_FRAGMENT_BIT;
			case 2:
				return VK_SHADER_STAGE_GEOMETRY_BIT;
		}
	}

	void VulkanGraphicsPipeline::InitializePipeline(Ref<VulkanDescriptorSetLayout> descriptorSetLayout, AttachmentSample samples)
	{

		// Obtain the context
		VulkanContext* context = VulkanContext::GetContext();

		// Obtain shader modules as a map
		std::map<uint32_t, VkShaderModule> vulkanShaderModules = m_Shaders->GetShaderModules();

		// This vector will be filled with shader stages
		std::vector<VkPipelineShaderStageCreateInfo> createInfos;

		// Create Stages from vulkan shader modules
		for (auto&& [stage, module] : vulkanShaderModules)
		{
			VkPipelineShaderStageCreateInfo stageCreateInfo{};
			stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stageCreateInfo.stage = GiveShaderStage(stage);
			stageCreateInfo.module = module;
			stageCreateInfo.pName = "main";
			createInfos.push_back(stageCreateInfo);
		}

		// Dynamic states
		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		// Vertex Input - This will be changed afterwards
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_VertexInputDescs.Bindings.size());
		vertexInputInfo.pVertexBindingDescriptions = m_VertexInputDescs.Bindings.data();
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_VertexInputDescs.Attributes.size());
		vertexInputInfo.pVertexAttributeDescriptions = m_VertexInputDescs.Attributes.data();

		// Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// Viewport
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)VulkanContext::GetContext()->GetSwapChainExtent().width;
		viewport.height = (float)VulkanContext::GetContext()->GetSwapChainExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// Scissor
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = VulkanContext::GetContext()->GetSwapChainExtent();

		// Dynamic state stuff
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Rasterizer
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		// Multisampling
		VkSampleCountFlagBits maxSamples = context->GetMaxSampleCount();
		VkSampleCountFlagBits candidateSamples = TextureUtils::GiveSampleCount(samples);
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_TRUE;
		multisampling.rasterizationSamples = maxSamples < candidateSamples ? maxSamples : candidateSamples;
		multisampling.minSampleShading = 0.2f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		// Depth stencil testing
		// 
		// 
		// Will be added later
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};
		depthStencil.back = {};


		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		// Color blend state
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		// Pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout->GetLayout();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(VulkanContext::GetContext()->GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to create pipeline layout!");
		}


		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = createInfos.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = m_RenderPass->GetVkRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;


		if (vkCreateGraphicsPipelines(VulkanContext::GetContext()->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to initialize graphics pipeline!");
		}
	}

	void VulkanGraphicsPipeline::ConfigureVertexInput(const VertexInput& input, InputRate inputRate)
	{

		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = input.GetStride();
		bindingDescription.inputRate = OpToVkInputRate(inputRate);

		m_VertexInputDescs.Bindings.push_back(bindingDescription);

		// Attribute descriptions
		uint32_t loc = 0;
		for (auto& element : input)
		{
			VkVertexInputAttributeDescription attrDesc{};
			attrDesc.binding = 0;
			attrDesc.location = loc;
			attrDesc.format = GiveFormatFromBufferType(element.Type);
			attrDesc.offset = element.Offset;
			m_VertexInputDescs.Attributes.push_back(attrDesc);
			loc++;
		}
	}

	VulkanGraphicsPipeline::VulkanGraphicsPipeline(Ref<VulkanShaderModule> shaders, Ref<VulkanRenderPass> renderPass) : m_Shaders(shaders) , m_RenderPass(renderPass) {}

	VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		vkDestroyPipeline(device, m_Pipeline, nullptr);
		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
	}

	VkPipeline VulkanGraphicsPipeline::GetPipeline()
	{
		return m_Pipeline;
	}

	VkPipelineLayout VulkanGraphicsPipeline::GetPipelineLayout()
	{
		return m_PipelineLayout;
	}


}