#include "pipeline.h"

using namespace vkInit;

vk::PipelineVertexInputStateCreateInfo vkInit::create_vertex_input_info(
		const vk::VertexInputBindingDescription& bindingDescription,
		const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions) {

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		return vertexInputInfo;
	}

	vk::PipelineInputAssemblyStateCreateInfo vkInit::create_input_assembly_info() {

		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
		inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
		inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;

		return inputAssemblyInfo;
	}

	vk::PipelineShaderStageCreateInfo vkInit::create_shader_info(const vk::ShaderModule& shaderModule, const vk::ShaderStageFlagBits& stage) {

		vk::PipelineShaderStageCreateInfo shaderInfo = {};
		shaderInfo.flags = vk::PipelineShaderStageCreateFlags();
		shaderInfo.stage = stage;
		shaderInfo.module = shaderModule;
		shaderInfo.pName = "main";
		return shaderInfo;
	}

	vk::Viewport vkInit::create_viewport(const GraphicsPipelineInBundle& specification) {

		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)specification.swapchainExtent.width;
		viewport.height = (float)specification.swapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		return viewport;
	}

	vk::Rect2D vkInit::create_scissor(const GraphicsPipelineInBundle& specification) {

		vk::Rect2D scissor = {};
		scissor.offset.x = 0.0f;
		scissor.offset.y = 0.0f;
		scissor.extent = specification.swapchainExtent;

		return scissor;
	}

	vk::PipelineViewportStateCreateInfo vkInit::create_viewport_state(const vk::Viewport& viewport, const vk::Rect2D& scissor) {

		vk::PipelineViewportStateCreateInfo viewportState = {};
		viewportState.flags = vk::PipelineViewportStateCreateFlags();
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		return viewportState;
	}

	vk::PipelineRasterizationStateCreateInfo vkInit::create_rasterizer_info() {

		vk::PipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
		rasterizer.depthClampEnable = VK_FALSE; //discard out of bounds fragments, don't clamp them
		rasterizer.rasterizerDiscardEnable = VK_FALSE; //This flag would disable fragment output
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise;
		rasterizer.depthBiasEnable = VK_FALSE; //Depth bias can be useful in shadow maps.

		return rasterizer;
	}

	vk::PipelineMultisampleStateCreateInfo vkInit::create_multisampling_info() {

		vk::PipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

		return multisampling;
	}

	vk::PipelineColorBlendAttachmentState vkInit::create_color_blend_attachment_state() {

		vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_FALSE;

		return colorBlendAttachment;
	}

	vk::PipelineColorBlendStateCreateInfo vkInit::create_color_blend_attachment_stage(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment) {

		vk::PipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.flags = vk::PipelineColorBlendStateCreateFlags();
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		return colorBlending;
	}

	vk::PipelineLayout vkInit::create_pipeline_layout(vk::Device device, std::vector<vk::DescriptorSetLayout> descriptorSetLayouts) {

		/*
		typedef struct VkPipelineLayoutCreateInfo {
			VkStructureType                 sType;
			const void*                     pNext;
			VkPipelineLayoutCreateFlags     flags;
			uint32_t                        setLayoutCount;
			const VkDescriptorSetLayout*    pSetLayouts;
			uint32_t                        pushConstantRangeCount;
			const VkPushConstantRange*      pPushConstantRanges;
		} VkPipelineLayoutCreateInfo;
		*/

		vk::PipelineLayoutCreateInfo layoutInfo;
		layoutInfo.flags = vk::PipelineLayoutCreateFlags();

		layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		layoutInfo.pSetLayouts = descriptorSetLayouts.data();

		layoutInfo.pushConstantRangeCount = 0;

		try {
			return device.createPipelineLayout(layoutInfo);
		}
		catch (vk::SystemError err) {
			vkLogging::Logger::get_logger()->print("Failed to create pipeline layout!");
		}
	}

	vk::RenderPass vkInit::create_renderpass(
		vk::Device device, vk::Format swapchainImageFormat, vk::Format depthFormat) {

		std::vector<vk::AttachmentDescription> attachments;
		std::vector<vk::AttachmentReference> attachmentReferences;

		//Color Buffer
		attachments.push_back(create_color_attachment(swapchainImageFormat));
		attachmentReferences.push_back(create_color_attachment_reference());

		//Depth Buffer
		attachments.push_back(create_depth_attachment(depthFormat));
		attachmentReferences.push_back(create_depth_attachment_reference());

		//Renderpasses are broken down into subpasses, there's always at least one.
		vk::SubpassDescription subpass = create_subpass(attachmentReferences);

		//Now create the renderpass
		vk::RenderPassCreateInfo renderpassInfo = create_renderpass_info(attachments, subpass);
		try {
			return device.createRenderPass(renderpassInfo);
		}
		catch (vk::SystemError err) {
			vkLogging::Logger::get_logger()->print("Failed to create renderpass!");
		}

	}

	vk::AttachmentDescription vkInit::create_color_attachment(const vk::Format& swapchainImageFormat) {

		vk::AttachmentDescription colorAttachment = {};
		colorAttachment.flags = vk::AttachmentDescriptionFlags();
		colorAttachment.format = swapchainImageFormat;
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		return colorAttachment;
	}

	vk::AttachmentReference vkInit::create_color_attachment_reference() {

		vk::AttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		return colorAttachmentRef;
	}

	vk::AttachmentDescription vkInit::create_depth_attachment(const vk::Format& depthFormat) {

		vk::AttachmentDescription depthAttachment = {};
		depthAttachment.flags = vk::AttachmentDescriptionFlags();
		depthAttachment.format = depthFormat;
		depthAttachment.samples = vk::SampleCountFlagBits::e1;
		depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
		depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		return depthAttachment;
	}

	vk::AttachmentReference vkInit::create_depth_attachment_reference() {

		vk::AttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		return depthAttachmentRef;
	}

	vk::SubpassDescription vkInit::create_subpass(
		const std::vector<vk::AttachmentReference>& attachments) {

		vk::SubpassDescription subpass = {};
		subpass.flags = vk::SubpassDescriptionFlags();
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &attachments[0];
		subpass.pDepthStencilAttachment = &attachments[1];

		return subpass;
	}

	vk::RenderPassCreateInfo vkInit::create_renderpass_info(
		const std::vector<vk::AttachmentDescription>& attachments, 
		const vk::SubpassDescription& subpass) {

		vk::RenderPassCreateInfo renderpassInfo = {};
		renderpassInfo.flags = vk::RenderPassCreateFlags();
		renderpassInfo.attachmentCount = attachments.size();
		renderpassInfo.pAttachments = attachments.data();
		renderpassInfo.subpassCount = 1;
		renderpassInfo.pSubpasses = &subpass;

		return renderpassInfo;
	}
	
	/**
		Make a graphics pipeline, along with renderpass and pipeline layout
		\param specification the struct holding input data, as specified at the top of the file.
		\param debug whether the system is running in debug mode
		\returns the bundle of data structures created
	*/
	GraphicsPipelineOutBundle vkInit::create_graphics_pipeline(GraphicsPipelineInBundle& specification) {
		/*
		* Build and return a graphics pipeline based on the given info.
		*/

		//The info for the graphics pipeline
		vk::GraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.flags = vk::PipelineCreateFlags();

		//Shader stages, to be populated later
		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

		//Vertex Input
		vk::VertexInputBindingDescription bindingDescription = vkMesh::getVertexBindingDescription();
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = vkMesh::getVertexAttributeDescriptions();
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = create_vertex_input_info(bindingDescription, attributeDescriptions);
		pipelineInfo.pVertexInputState = &vertexInputInfo;

		//Input Assembly
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = create_input_assembly_info();
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

		//Vertex Shader
		vkLogging::Logger::get_logger()->print("Create vertex shader module");
		vk::ShaderModule vertexShader = vkUtil::createModule(
			specification.vertexFilepath, specification.device
		);
		vk::PipelineShaderStageCreateInfo vertexShaderInfo = create_shader_info(vertexShader, vk::ShaderStageFlagBits::eVertex);
		shaderStages.push_back(vertexShaderInfo);

		//Viewport and Scissor
		vk::Viewport viewport = create_viewport(specification);
		vk::Rect2D scissor = create_scissor(specification);
		vk::PipelineViewportStateCreateInfo viewportState = create_viewport_state(viewport, scissor);
		pipelineInfo.pViewportState = &viewportState;

		//Rasterizer
		vk::PipelineRasterizationStateCreateInfo rasterizer = create_rasterizer_info();
		pipelineInfo.pRasterizationState = &rasterizer;

		//Fragment Shader
		vkLogging::Logger::get_logger()->print("Create fragment shader module");
		vk::ShaderModule fragmentShader = vkUtil::createModule(
			specification.fragmentFilepath, specification.device
		);
		vk::PipelineShaderStageCreateInfo fragmentShaderInfo = create_shader_info(fragmentShader, vk::ShaderStageFlagBits::eFragment);
		shaderStages.push_back(fragmentShaderInfo);
		//Now both shaders have been made, we can declare them to the pipeline info
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();

		//Depth-Stencil
		vk::PipelineDepthStencilStateCreateInfo depthState;
		depthState.flags = vk::PipelineDepthStencilStateCreateFlags();
		depthState.depthTestEnable = true;
		depthState.depthWriteEnable = true;
		depthState.depthCompareOp = vk::CompareOp::eLess;
		depthState.depthBoundsTestEnable = false;
		depthState.stencilTestEnable = false;
		pipelineInfo.pDepthStencilState = &depthState;

		//Multisampling
		vk::PipelineMultisampleStateCreateInfo multisampling = create_multisampling_info();
		pipelineInfo.pMultisampleState = &multisampling;

		//Color Blend
		vk::PipelineColorBlendAttachmentState colorBlendAttachment = create_color_blend_attachment_state();
		vk::PipelineColorBlendStateCreateInfo colorBlending = create_color_blend_attachment_stage(colorBlendAttachment);
		pipelineInfo.pColorBlendState = &colorBlending;

		//Pipeline Layout
		vkLogging::Logger::get_logger()->print("Create Pipeline Layout");
		vk::PipelineLayout pipelineLayout = create_pipeline_layout(specification.device, specification.descriptorSetLayouts);
		pipelineInfo.layout = pipelineLayout;

		//Renderpass
		vkLogging::Logger::get_logger()->print("Create RenderPass");
		vk::RenderPass renderpass = create_renderpass(
			specification.device, specification.swapchainImageFormat, 
			specification.depthFormat
		);
		pipelineInfo.renderPass = renderpass;
		pipelineInfo.subpass = 0;

		//Extra stuff
		pipelineInfo.basePipelineHandle = nullptr;

		//Make the Pipeline
		vkLogging::Logger::get_logger()->print("Create Graphics Pipeline");
		vk::Pipeline graphicsPipeline;
		try {
#ifdef VK_MAKE_VERSION
			graphicsPipeline = (specification.device.createGraphicsPipeline(nullptr, pipelineInfo));
#else
			graphicsPipeline = (specification.device.createGraphicsPipeline(nullptr, pipelineInfo)).value;
#endif
		}
		catch (vk::SystemError err) {
			vkLogging::Logger::get_logger()->print("Failed to create Pipeline");
		}

		GraphicsPipelineOutBundle output;
		output.layout = pipelineLayout;
		output.renderpass = renderpass;
		output.pipeline = graphicsPipeline;

		//Finally clean up by destroying shader modules
		specification.device.destroyShaderModule(vertexShader);
		specification.device.destroyShaderModule(fragmentShader);

		return output;
}

