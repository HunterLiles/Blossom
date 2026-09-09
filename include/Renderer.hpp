#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "Input.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

void check(VkResult result, std::string_view action) {
  if (result != VK_SUCCESS)
    throw std::runtime_error(std::string(action) + " failed (VkResult " + std::to_string(result) + ")");
}

void onFramebufferResize(GLFWwindow* window, int, int) {
  static_cast<Engine*>(glfwGetWindowUserPointer(window))->framebufferResized = true;
}

std::vector<char> readFile(const std::string& path) {
  std::ifstream file(path, std::ios::ate | std::ios::binary);
  if (!file) throw std::runtime_error("Cannot open shader: " + path);
  const size_t size = static_cast<size_t>(file.tellg());
  std::vector<char> bytes(size);
  file.seekg(0);
  file.read(bytes.data(), static_cast<std::streamsize>(size));
  return bytes;
}

VkShaderModule createShaderModule(const Engine& engine, const std::string& path) {
  const std::vector<char> code = readFile(path);
  VkShaderModuleCreateInfo info{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
  info.codeSize = code.size();
  info.pCode = reinterpret_cast<const uint32_t*>(code.data());
  VkShaderModule module = VK_NULL_HANDLE;
  check(vkCreateShaderModule(engine.device, &info, nullptr, &module), "create shader module");
  return module;
}

SwapchainSupport getSwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
  SwapchainSupport support{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &support.capabilities);
  uint32_t count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);
  support.formats.resize(count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, support.formats.data());
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr);
  support.presentModes.resize(count);
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, support.presentModes.data());
  return support;
}

QueueFamilies findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
  QueueFamilies result{};
  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
  std::vector<VkQueueFamilyProperties> properties(count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties.data());
  for (uint32_t i = 0; i < count; ++i) {
    if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) result.graphics = i;
    VkBool32 present = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present);
    if (present) result.present = i;
    if (result.graphics != UINT32_MAX && result.present != UINT32_MAX) break;
  }
  return result;
}

bool supportsDevice(VkPhysicalDevice device, VkSurfaceKHR surface, QueueFamilies& queues) {
  queues = findQueueFamilies(device, surface);
  if (queues.graphics == UINT32_MAX || queues.present == UINT32_MAX) return false;
  uint32_t count = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
  std::vector<VkExtensionProperties> extensions(count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data());
  const bool hasSwapchain = std::any_of(extensions.begin(), extensions.end(), [](const auto& extension) {
    return std::string_view(extension.extensionName) == VK_KHR_SWAPCHAIN_EXTENSION_NAME;
  });
  const SwapchainSupport support = getSwapchainSupport(device, surface);
  return hasSwapchain && !support.formats.empty() && !support.presentModes.empty();
}

uint32_t findMemoryType(const Engine& engine, uint32_t typeMask, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memory{};
  vkGetPhysicalDeviceMemoryProperties(engine.physicalDevice, &memory);
  for (uint32_t i = 0; i < memory.memoryTypeCount; ++i)
    if ((typeMask & (1u << i)) && (memory.memoryTypes[i].propertyFlags & properties) == properties)
      return i;
  throw std::runtime_error("No compatible Vulkan memory type");
}

void createBuffer(const Engine& engine, VkDeviceSize size, VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory) {
  VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  check(vkCreateBuffer(engine.device, &bufferInfo, nullptr, &buffer), "create buffer");
  VkMemoryRequirements requirements{};
  vkGetBufferMemoryRequirements(engine.device, buffer, &requirements);
  VkMemoryAllocateInfo allocation{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
  allocation.allocationSize = requirements.size;
  allocation.memoryTypeIndex = findMemoryType(engine, requirements.memoryTypeBits, properties);
  check(vkAllocateMemory(engine.device, &allocation, nullptr, &memory), "allocate buffer memory");
  check(vkBindBufferMemory(engine.device, buffer, memory, 0), "bind buffer memory");
}

void createImage(const Engine& engine, VkExtent2D extent, VkFormat format, VkImageUsageFlags usage,
                 VkImage& image, VkDeviceMemory& memory) {
  VkImageCreateInfo info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
  info.imageType = VK_IMAGE_TYPE_2D;
  info.extent = {extent.width, extent.height, 1};
  info.mipLevels = 1;
  info.arrayLayers = 1;
  info.format = format;
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.usage = usage;
  info.samples = VK_SAMPLE_COUNT_1_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  check(vkCreateImage(engine.device, &info, nullptr, &image), "create image");
  VkMemoryRequirements requirements{};
  vkGetImageMemoryRequirements(engine.device, image, &requirements);
  VkMemoryAllocateInfo allocation{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
  allocation.allocationSize = requirements.size;
  allocation.memoryTypeIndex = findMemoryType(engine, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  check(vkAllocateMemory(engine.device, &allocation, nullptr, &memory), "allocate image memory");
  check(vkBindImageMemory(engine.device, image, memory, 0), "bind image memory");
}

VkImageView createImageView(const Engine& engine, VkImage image, VkFormat format, VkImageAspectFlags aspect) {
  VkImageViewCreateInfo info{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
  info.image = image;
  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.format = format;
  info.subresourceRange.aspectMask = aspect;
  info.subresourceRange.levelCount = 1;
  info.subresourceRange.layerCount = 1;
  VkImageView view = VK_NULL_HANDLE;
  check(vkCreateImageView(engine.device, &info, nullptr, &view), "create image view");
  return view;
}

VkFormat findDepthFormat(const Engine& engine) {
  const std::array candidates{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
  for (VkFormat format : candidates) {
    VkFormatProperties properties{};
    vkGetPhysicalDeviceFormatProperties(engine.physicalDevice, format, &properties);
    if (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) return format;
  }
  throw std::runtime_error("No supported depth format");
}

VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
  const auto preferred = std::find_if(formats.begin(), formats.end(), [](const auto& format) {
    return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  });
  return preferred != formats.end() ? *preferred : formats.front();
}

VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& modes) {
  return std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != modes.end()
             ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseExtent(const Engine& engine, const VkSurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width != UINT32_MAX) return capabilities.currentExtent;
  int width = 0, height = 0;
  glfwGetFramebufferSize(engine.window, &width, &height);
  return {std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
          std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)};
}

void createSceneResources(Engine& engine) {
  VkDescriptorSetLayoutBinding binding{};
  binding.binding = 0;
  binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  binding.descriptorCount = 1;
  binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  VkDescriptorSetLayoutCreateInfo layout{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
  layout.bindingCount = 1;
  layout.pBindings = &binding;
  check(vkCreateDescriptorSetLayout(engine.device, &layout, nullptr, &engine.sceneSetLayout), "create scene set layout");
  VkDescriptorPoolSize poolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, framesInFlight * viewCount};
  VkDescriptorPoolCreateInfo pool{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
  pool.maxSets = framesInFlight * viewCount;
  pool.poolSizeCount = 1;
  pool.pPoolSizes = &poolSize;
  check(vkCreateDescriptorPool(engine.device, &pool, nullptr, &engine.sceneDescriptorPool), "create scene descriptor pool");
  for (uint32_t frame = 0; frame < framesInFlight; ++frame)
    for (uint32_t view = 0; view < viewCount; ++view) {
      createBuffer(engine, sizeof(UniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                   engine.uniformBuffers[frame][view], engine.uniformMemory[frame][view]);
      check(vkMapMemory(engine.device, engine.uniformMemory[frame][view], 0, sizeof(UniformData), 0,
                        &engine.uniformMapped[frame][view]), "map uniform buffer");
    }
  std::array<VkDescriptorSetLayout, framesInFlight * viewCount> layouts{};
  layouts.fill(engine.sceneSetLayout);
  VkDescriptorSetAllocateInfo allocation{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
  allocation.descriptorPool = engine.sceneDescriptorPool;
  allocation.descriptorSetCount = framesInFlight * viewCount;
  allocation.pSetLayouts = layouts.data();
  check(vkAllocateDescriptorSets(engine.device, &allocation, &engine.sceneSets[0][0]), "allocate scene descriptor sets");
  for (uint32_t frame = 0; frame < framesInFlight; ++frame)
    for (uint32_t view = 0; view < viewCount; ++view) {
      VkDescriptorBufferInfo buffer{engine.uniformBuffers[frame][view], 0, sizeof(UniformData)};
      VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
      write.dstSet = engine.sceneSets[frame][view];
      write.dstBinding = 0;
      write.descriptorCount = 1;
      write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      write.pBufferInfo = &buffer;
      vkUpdateDescriptorSets(engine.device, 1, &write, 0, nullptr);
    }
}

void destroySceneResources(Engine& engine) {
  for (uint32_t frame = 0; frame < framesInFlight; ++frame)
    for (uint32_t view = 0; view < viewCount; ++view) {
      if (engine.uniformMapped[frame][view]) vkUnmapMemory(engine.device, engine.uniformMemory[frame][view]);
      vkDestroyBuffer(engine.device, engine.uniformBuffers[frame][view], nullptr);
      vkFreeMemory(engine.device, engine.uniformMemory[frame][view], nullptr);
    }
  vkDestroyDescriptorPool(engine.device, engine.sceneDescriptorPool, nullptr);
  vkDestroyDescriptorSetLayout(engine.device, engine.sceneSetLayout, nullptr);
}

void createRenderPass(Engine& engine) {
  VkAttachmentDescription color{};
  color.format = engine.swapchainFormat;
  color.samples = VK_SAMPLE_COUNT_1_BIT;
  color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  VkAttachmentDescription depth{};
  depth.format = engine.depthFormat;
  depth.samples = VK_SAMPLE_COUNT_1_BIT;
  depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  const std::array attachments{color, depth};
  VkAttachmentReference colorReference{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
  VkAttachmentReference depthReference{1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorReference;
  subpass.pDepthStencilAttachment = &depthReference;
  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstStageMask = dependency.srcStageMask;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  VkRenderPassCreateInfo info{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  info.attachmentCount = static_cast<uint32_t>(attachments.size());
  info.pAttachments = attachments.data();
  info.subpassCount = 1;
  info.pSubpasses = &subpass;
  info.dependencyCount = 1;
  info.pDependencies = &dependency;
  check(vkCreateRenderPass(engine.device, &info, nullptr, &engine.renderPass), "create render pass");
}

void createViewRenderPass(Engine& engine) {
  VkAttachmentDescription color{};
  color.format = engine.swapchainFormat;
  color.samples = VK_SAMPLE_COUNT_1_BIT;
  color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  VkAttachmentDescription depth{};
  depth.format = engine.depthFormat;
  depth.samples = VK_SAMPLE_COUNT_1_BIT;
  depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  const std::array attachments{color, depth};
  VkAttachmentReference colorReference{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
  VkAttachmentReference depthReference{1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorReference;
  subpass.pDepthStencilAttachment = &depthReference;
  const std::array dependencies{
      VkSubpassDependency{VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
                          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0},
      VkSubpassDependency{0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                          VK_ACCESS_SHADER_READ_BIT, 0}};
  VkRenderPassCreateInfo info{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  info.attachmentCount = static_cast<uint32_t>(attachments.size());
  info.pAttachments = attachments.data();
  info.subpassCount = 1;
  info.pSubpasses = &subpass;
  info.dependencyCount = static_cast<uint32_t>(dependencies.size());
  info.pDependencies = dependencies.data();
  check(vkCreateRenderPass(engine.device, &info, nullptr, &engine.viewRenderPass), "create view render pass");
}

void createPipeline(Engine& engine) {
  const VkShaderModule vertex = createShaderModule(engine, std::string(SHADER_DIRECTORY) + "/cube.vert.spv");
  const VkShaderModule fragment = createShaderModule(engine, std::string(SHADER_DIRECTORY) + "/cube.frag.spv");
  const std::array stages{
    VkPipelineShaderStageCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_VERTEX_BIT, vertex, "main"},
    VkPipelineShaderStageCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_FRAGMENT_BIT, fragment, "main"}};
  VkPipelineVertexInputStateCreateInfo vertexInput{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
  VkPipelineInputAssemblyStateCreateInfo assembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
  assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  VkPipelineViewportStateCreateInfo viewport{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
  viewport.viewportCount = 1;
  viewport.scissorCount = 1;
  VkPipelineRasterizationStateCreateInfo rasterizer{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.cullMode = VK_CULL_MODE_NONE;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.lineWidth = 1.0f;
  VkPipelineMultisampleStateCreateInfo multisampling{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  VkPipelineDepthStencilStateCreateInfo depth{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
  depth.depthTestEnable = VK_TRUE;
  depth.depthWriteEnable = VK_TRUE;
  depth.depthCompareOp = VK_COMPARE_OP_LESS;
  VkPipelineColorBlendAttachmentState blendAttachment{};
  blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  VkPipelineColorBlendStateCreateInfo blending{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
  blending.attachmentCount = 1;
  blending.pAttachments = &blendAttachment;
  const std::array dynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamic{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
  dynamic.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamic.pDynamicStates = dynamicStates.data();
  VkPipelineLayoutCreateInfo layout{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
  layout.setLayoutCount = 1;
  layout.pSetLayouts = &engine.sceneSetLayout;
  check(vkCreatePipelineLayout(engine.device, &layout, nullptr, &engine.pipelineLayout), "create pipeline layout");
  VkGraphicsPipelineCreateInfo info{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
  info.stageCount = static_cast<uint32_t>(stages.size());
  info.pStages = stages.data();
  info.pVertexInputState = &vertexInput;
  info.pInputAssemblyState = &assembly;
  info.pViewportState = &viewport;
  info.pRasterizationState = &rasterizer;
  info.pMultisampleState = &multisampling;
  info.pDepthStencilState = &depth;
  info.pColorBlendState = &blending;
  info.pDynamicState = &dynamic;
  info.layout = engine.pipelineLayout;
  info.renderPass = engine.viewRenderPass;
  check(vkCreateGraphicsPipelines(engine.device, VK_NULL_HANDLE, 1, &info, nullptr, &engine.pipeline), "create graphics pipeline");
  vkDestroyShaderModule(engine.device, fragment, nullptr);
  vkDestroyShaderModule(engine.device, vertex, nullptr);
}

void createGridPipeline(Engine& engine) {
  const VkShaderModule vertex = createShaderModule(engine, std::string(SHADER_DIRECTORY) + "/grid.vert.spv");
  const VkShaderModule fragment = createShaderModule(engine, std::string(SHADER_DIRECTORY) + "/grid.frag.spv");
  const std::array stages{
      VkPipelineShaderStageCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_VERTEX_BIT, vertex, "main"},
      VkPipelineShaderStageCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_FRAGMENT_BIT, fragment, "main"}};
  VkPipelineVertexInputStateCreateInfo vertexInput{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
  VkPipelineInputAssemblyStateCreateInfo assembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
  assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
  VkPipelineViewportStateCreateInfo viewport{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
  viewport.viewportCount = 1;
  viewport.scissorCount = 1;
  VkPipelineRasterizationStateCreateInfo rasterizer{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.cullMode = VK_CULL_MODE_NONE;
  rasterizer.lineWidth = 1.0f;
  VkPipelineMultisampleStateCreateInfo multisampling{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  VkPipelineDepthStencilStateCreateInfo depth{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
  depth.depthTestEnable = VK_TRUE;
  depth.depthWriteEnable = VK_FALSE;
  depth.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
  VkPipelineColorBlendAttachmentState blendAttachment{};
  blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  VkPipelineColorBlendStateCreateInfo blending{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
  blending.attachmentCount = 1;
  blending.pAttachments = &blendAttachment;
  const std::array dynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamic{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
  dynamic.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamic.pDynamicStates = dynamicStates.data();
  VkGraphicsPipelineCreateInfo info{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
  info.stageCount = static_cast<uint32_t>(stages.size());
  info.pStages = stages.data();
  info.pVertexInputState = &vertexInput;
  info.pInputAssemblyState = &assembly;
  info.pViewportState = &viewport;
  info.pRasterizationState = &rasterizer;
  info.pMultisampleState = &multisampling;
  info.pDepthStencilState = &depth;
  info.pColorBlendState = &blending;
  info.pDynamicState = &dynamic;
  info.layout = engine.pipelineLayout;
  info.renderPass = engine.viewRenderPass;
  check(vkCreateGraphicsPipelines(engine.device, VK_NULL_HANDLE, 1, &info, nullptr, &engine.gridPipeline), "create grid pipeline");
  vkDestroyShaderModule(engine.device, fragment, nullptr);
  vkDestroyShaderModule(engine.device, vertex, nullptr);
}

void createDepthTargets(Engine& engine) {
  engine.depthFormat = findDepthFormat(engine);
  engine.depthTargets.resize(engine.swapchainImages.size());
  for (DepthTarget& target : engine.depthTargets) {
    createImage(engine, engine.swapchainExtent, engine.depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, target.image, target.memory);
    target.view = createImageView(engine, target.image, engine.depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
  }
}

void createViewTargets(Engine& engine) {
  for (uint32_t view = 0; view < viewCount; ++view) {
    ViewTarget& target = engine.views[view];
    target.extent = engine.viewExtents[view];
    createImage(engine, target.extent, engine.swapchainFormat,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                target.colorImage, target.colorMemory);
    target.colorView = createImageView(engine, target.colorImage, engine.swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    createImage(engine, target.extent, engine.depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                target.depthImage, target.depthMemory);
    target.depthView = createImageView(engine, target.depthImage, engine.depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    const std::array attachments{target.colorView, target.depthView};
    VkFramebufferCreateInfo framebuffer{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebuffer.renderPass = engine.viewRenderPass;
    framebuffer.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebuffer.pAttachments = attachments.data();
    framebuffer.width = target.extent.width;
    framebuffer.height = target.extent.height;
    framebuffer.layers = 1;
    check(vkCreateFramebuffer(engine.device, &framebuffer, nullptr, &target.framebuffer), "create view framebuffer");
  }
}

void destroyViewTargets(Engine& engine) {
  for (ViewTarget& target : engine.views) {
    vkDestroyFramebuffer(engine.device, target.framebuffer, nullptr);
    vkDestroyImageView(engine.device, target.depthView, nullptr);
    vkDestroyImage(engine.device, target.depthImage, nullptr);
    vkFreeMemory(engine.device, target.depthMemory, nullptr);
    vkDestroyImageView(engine.device, target.colorView, nullptr);
    vkDestroyImage(engine.device, target.colorImage, nullptr);
    vkFreeMemory(engine.device, target.colorMemory, nullptr);
    target = {};
  }
}

void resizeViewTargets(Engine& engine) {
  bool changed = false;
  for (uint32_t view = 0; view < viewCount; ++view)
    changed |= engine.viewExtents[view].width != engine.requestedViewExtents[view].width ||
               engine.viewExtents[view].height != engine.requestedViewExtents[view].height;
  if (!engine.imguiReady || !changed) return;
  check(vkDeviceWaitIdle(engine.device), "wait to resize views");
  for (ViewTarget& target : engine.views)
    if (target.texture) ImGui_ImplVulkan_RemoveTexture(target.texture);
  destroyViewTargets(engine);
  engine.viewExtents = engine.requestedViewExtents;
  createViewTargets(engine);
  for (ViewTarget& target : engine.views)
    target.texture = ImGui_ImplVulkan_AddTexture(target.colorView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void destroySwapchain(Engine& engine) {
  if (!engine.commandBuffers.empty()) vkFreeCommandBuffers(engine.device, engine.commandPool, static_cast<uint32_t>(engine.commandBuffers.size()), engine.commandBuffers.data());
  engine.commandBuffers.clear();
  destroyViewTargets(engine);
  for (VkFramebuffer framebuffer : engine.framebuffers) vkDestroyFramebuffer(engine.device, framebuffer, nullptr);
  engine.framebuffers.clear();
  vkDestroyPipeline(engine.device, engine.pipeline, nullptr);
  vkDestroyPipeline(engine.device, engine.gridPipeline, nullptr);
  vkDestroyPipelineLayout(engine.device, engine.pipelineLayout, nullptr);
  vkDestroyRenderPass(engine.device, engine.renderPass, nullptr);
  vkDestroyRenderPass(engine.device, engine.viewRenderPass, nullptr);
  engine.pipeline = VK_NULL_HANDLE;
  engine.gridPipeline = VK_NULL_HANDLE;
  engine.pipelineLayout = VK_NULL_HANDLE;
  engine.renderPass = VK_NULL_HANDLE;
  engine.viewRenderPass = VK_NULL_HANDLE;
  for (DepthTarget target : engine.depthTargets) {
    vkDestroyImageView(engine.device, target.view, nullptr);
    vkDestroyImage(engine.device, target.image, nullptr);
    vkFreeMemory(engine.device, target.memory, nullptr);
  }
  engine.depthTargets.clear();
  for (VkImageView view : engine.imageViews) vkDestroyImageView(engine.device, view, nullptr);
  engine.imageViews.clear();
  vkDestroySwapchainKHR(engine.device, engine.swapchain, nullptr);
  engine.swapchain = VK_NULL_HANDLE;
}

void createSwapchain(Engine& engine) {
  const SwapchainSupport support = getSwapchainSupport(engine.physicalDevice, engine.surface);
  const VkSurfaceFormatKHR format = chooseSurfaceFormat(support.formats);
  const VkExtent2D extent = chooseExtent(engine, support.capabilities);
  uint32_t imageCount = support.capabilities.minImageCount + 1;
  if (support.capabilities.maxImageCount > 0) imageCount = std::min(imageCount, support.capabilities.maxImageCount);
  const uint32_t families[] = {engine.queues.graphics, engine.queues.present};
  VkSwapchainCreateInfoKHR info{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  info.surface = engine.surface;
  info.minImageCount = imageCount;
  info.imageFormat = format.format;
  info.imageColorSpace = format.colorSpace;
  info.imageExtent = extent;
  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  info.imageSharingMode = engine.queues.graphics == engine.queues.present ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
  info.queueFamilyIndexCount = engine.queues.graphics == engine.queues.present ? 0 : 2;
  info.pQueueFamilyIndices = families;
  info.preTransform = support.capabilities.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.presentMode = choosePresentMode(support.presentModes);
  info.clipped = VK_TRUE;
  check(vkCreateSwapchainKHR(engine.device, &info, nullptr, &engine.swapchain), "create swapchain");
  engine.swapchainFormat = format.format;
  engine.swapchainExtent = extent;
  vkGetSwapchainImagesKHR(engine.device, engine.swapchain, &imageCount, nullptr);
  engine.swapchainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(engine.device, engine.swapchain, &imageCount, engine.swapchainImages.data());
  engine.imageViews.resize(imageCount);
  for (uint32_t i = 0; i < imageCount; ++i)
    engine.imageViews[i] = createImageView(engine, engine.swapchainImages[i], engine.swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT);
  createDepthTargets(engine);
  createRenderPass(engine);
  createViewRenderPass(engine);
  createViewTargets(engine);
  createPipeline(engine);
  createGridPipeline(engine);
  engine.framebuffers.resize(imageCount);
  for (uint32_t i = 0; i < imageCount; ++i) {
    const std::array attachments{engine.imageViews[i], engine.depthTargets[i].view};
    VkFramebufferCreateInfo framebuffer{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebuffer.renderPass = engine.renderPass;
    framebuffer.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebuffer.pAttachments = attachments.data();
    framebuffer.width = extent.width;
    framebuffer.height = extent.height;
    framebuffer.layers = 1;
    check(vkCreateFramebuffer(engine.device, &framebuffer, nullptr, &engine.framebuffers[i]), "create framebuffer");
  }
  engine.commandBuffers.resize(imageCount);
  VkCommandBufferAllocateInfo allocation{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  allocation.commandPool = engine.commandPool;
  allocation.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocation.commandBufferCount = imageCount;
  check(vkAllocateCommandBuffers(engine.device, &allocation, engine.commandBuffers.data()), "allocate command buffers");
}

void createImGuiPool(Engine& engine) {
  const std::array sizes{
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
  VkDescriptorPoolCreateInfo info{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
  info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  info.maxSets = 11000;
  info.poolSizeCount = static_cast<uint32_t>(sizes.size());
  info.pPoolSizes = sizes.data();
  check(vkCreateDescriptorPool(engine.device, &info, nullptr, &engine.imguiDescriptorPool), "create ImGui descriptor pool");
}

void initializeImGui(Engine& engine) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NoMouseCursorChange;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForVulkan(engine.window, true);
  ImGui_ImplVulkan_InitInfo init{};
  init.Instance = engine.instance;
  init.PhysicalDevice = engine.physicalDevice;
  init.Device = engine.device;
  init.QueueFamily = engine.queues.graphics;
  init.Queue = engine.graphicsQueue;
  init.DescriptorPool = engine.imguiDescriptorPool;
  init.MinImageCount = static_cast<uint32_t>(engine.swapchainImages.size());
  init.ImageCount = static_cast<uint32_t>(engine.swapchainImages.size());
  init.ApiVersion = VK_API_VERSION_1_0;
  init.PipelineInfoMain.RenderPass = engine.renderPass;
  init.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  ImGui_ImplVulkan_Init(&init);
  for (ViewTarget& target : engine.views)
    target.texture = ImGui_ImplVulkan_AddTexture(target.colorView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  engine.imguiReady = true;
}

void shutdownImGui(Engine& engine) {
  if (!engine.imguiReady) return;
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  engine.imguiReady = false;
}

void updateUniform(Engine& engine, uint32_t frame, ViewKind viewKind) {
  const uint32_t viewIndex = static_cast<uint32_t>(viewKind);
  const bool gameView = viewKind == ViewKind::Game;
  const math::Mat4 model = gameView
      ? math::multiply(math::rotation(math::axisAngle({0.3f, 1.0f, 0.15f}, engine.cube.angle)),
                       math::translation(engine.cube.position))
      : math::translation(engine.cube.position);
  const Camera& camera = gameView ? engine.camera : engine.sceneCamera;
  const math::Vec3 forward = math::rotate(camera.orientation, {0.0f, 0.0f, -1.0f});
  const math::Vec3 up = math::rotate(camera.orientation, {0.0f, 1.0f, 0.0f});
  const math::Mat4 view = math::lookAt(camera.position, forward, up);
  const VkExtent2D extent = engine.views[viewIndex].extent;
  const float aspect = static_cast<float>(extent.width) / static_cast<float>(extent.height);
  const float fovRadians = camera.fieldOfView * 0.0174532925f;
  const math::Mat4 projection = math::perspective(fovRadians, aspect, 0.1f, 100.0f);
  UniformData uniform{};
  uniform.mvp = math::multiply(math::multiply(model, view), projection);
  uniform.globalLight = engine.globalLight.enabled ? engine.globalLight.level : 0.0f;
  std::memcpy(engine.uniformMapped[frame][viewIndex], &uniform, sizeof(uniform));
}

void recordView(const Engine& engine, VkCommandBuffer command, ViewKind viewKind) {
  const uint32_t viewIndex = static_cast<uint32_t>(viewKind);
  std::array<VkClearValue, 2> clear{};
  clear[0].color = {{0.02f, 0.025f, 0.05f, 1.0f}};
  clear[1].depthStencil = {1.0f, 0};
  VkRenderPassBeginInfo render{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
  render.renderPass = engine.viewRenderPass;
  render.framebuffer = engine.views[viewIndex].framebuffer;
  render.renderArea.extent = engine.views[viewIndex].extent;
  render.clearValueCount = static_cast<uint32_t>(clear.size());
  render.pClearValues = clear.data();
  vkCmdBeginRenderPass(command, &render, VK_SUBPASS_CONTENTS_INLINE);
  const VkExtent2D extent = engine.views[viewIndex].extent;
  const VkViewport viewport{0.0f, static_cast<float>(extent.height),
                            static_cast<float>(extent.width), -static_cast<float>(extent.height), 0.0f, 1.0f};
  const VkRect2D scissor{{0, 0}, extent};
  vkCmdSetViewport(command, 0, 1, &viewport);
  vkCmdSetScissor(command, 0, 1, &scissor);
  if (viewKind == ViewKind::Scene) {
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, engine.gridPipeline);
    vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, engine.pipelineLayout, 0, 1,
                            &engine.sceneSets[engine.frameIndex][viewIndex], 0, nullptr);
    vkCmdDraw(command, gridVertexCount, 1, 0, 0);
  }
  vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, engine.pipeline);
  vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, engine.pipelineLayout, 0, 1,
                          &engine.sceneSets[engine.frameIndex][viewIndex], 0, nullptr);
  vkCmdDraw(command, cubeVertexCount, 1, 0, 0);
  vkCmdEndRenderPass(command);
}

void recordCommands(const Engine& engine, uint32_t imageIndex) {
  const VkCommandBuffer command = engine.commandBuffers[imageIndex];
  VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  check(vkBeginCommandBuffer(command, &begin), "begin command buffer");
  recordView(engine, command, ViewKind::Scene);
  recordView(engine, command, ViewKind::Game);
  std::array<VkClearValue, 2> clear{};
  clear[0].color = {{0.02f, 0.025f, 0.05f, 1.0f}};
  clear[1].depthStencil = {1.0f, 0};
  VkRenderPassBeginInfo render{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
  render.renderPass = engine.renderPass;
  render.framebuffer = engine.framebuffers[imageIndex];
  render.renderArea.extent = engine.swapchainExtent;
  render.clearValueCount = static_cast<uint32_t>(clear.size());
  render.pClearValues = clear.data();
  vkCmdBeginRenderPass(command, &render, VK_SUBPASS_CONTENTS_INLINE);
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command);
  vkCmdEndRenderPass(command);
  check(vkEndCommandBuffer(command), "end command buffer");
}

void recreateSwapchain(Engine& engine) {
  int width = 0, height = 0;
  glfwGetFramebufferSize(engine.window, &width, &height);
  while (width == 0 || height == 0) {
    glfwWaitEvents();
    glfwGetFramebufferSize(engine.window, &width, &height);
  }
  check(vkDeviceWaitIdle(engine.device), "wait for device");
  shutdownImGui(engine);
  destroySwapchain(engine);
  createSwapchain(engine);
  initializeImGui(engine);
  engine.framebufferResized = false;
}

void drawFrame(Engine& engine) {
  FrameSync& frame = engine.frames[engine.frameIndex];
  check(vkWaitForFences(engine.device, 1, &frame.finished, VK_TRUE, std::numeric_limits<uint64_t>::max()), "wait for frame");
  uint32_t imageIndex = 0;
  const VkResult acquire = vkAcquireNextImageKHR(engine.device, engine.swapchain, std::numeric_limits<uint64_t>::max(), frame.imageAvailable, VK_NULL_HANDLE, &imageIndex);
  if (acquire == VK_ERROR_OUT_OF_DATE_KHR) { recreateSwapchain(engine); return; }
  if (acquire != VK_SUCCESS && acquire != VK_SUBOPTIMAL_KHR) check(acquire, "acquire swapchain image");
  check(vkResetFences(engine.device, 1, &frame.finished), "reset frame fence");
  check(vkResetCommandBuffer(engine.commandBuffers[imageIndex], 0), "reset command buffer");
  updateUniform(engine, engine.frameIndex, ViewKind::Scene);
  updateUniform(engine, engine.frameIndex, ViewKind::Game);
  recordCommands(engine, imageIndex);
  const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submit.waitSemaphoreCount = 1;
  submit.pWaitSemaphores = &frame.imageAvailable;
  submit.pWaitDstStageMask = &waitStage;
  submit.commandBufferCount = 1;
  submit.pCommandBuffers = &engine.commandBuffers[imageIndex];
  submit.signalSemaphoreCount = 1;
  submit.pSignalSemaphores = &frame.renderFinished;
  check(vkQueueSubmit(engine.graphicsQueue, 1, &submit, frame.finished), "submit draw");
  VkPresentInfoKHR present{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  present.waitSemaphoreCount = 1;
  present.pWaitSemaphores = &frame.renderFinished;
  present.swapchainCount = 1;
  present.pSwapchains = &engine.swapchain;
  present.pImageIndices = &imageIndex;
  const VkResult result = vkQueuePresentKHR(engine.presentQueue, &present);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || engine.framebufferResized) recreateSwapchain(engine);
  else check(result, "present image");
  engine.frameIndex = (engine.frameIndex + 1) % framesInFlight;
}

void buildInterface(Engine& engine) {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  Camera& activeCamera = engine.focusedView == ViewKind::Scene ? engine.sceneCamera : engine.camera;
  ImGui::SetNextWindowPos({16.0f, 16.0f}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize({380.0f, 280.0f}, ImGuiCond_FirstUseEver);
  ImGui::Begin("Controls");
  ImGui::TextUnformatted("WASD: move camera   Space/Ctrl: vertical");
  ImGui::TextUnformatted("Arrow keys: move cube   Escape: exit");
  if (ImGui::Checkbox("Hide cursor and capture focused view (Tab)", &engine.cursorCaptured))
    setCursorCapture(engine, engine.cursorCaptured);
  ImGui::Text("Input target: %s", engine.focusedView == ViewKind::Scene ? "Scene" : "Game");
  ImGui::SliderFloat("Field of view", &activeCamera.fieldOfView, 30.0f, 120.0f, "%.0f degrees");
  ImGui::SliderFloat("Camera speed", &activeCamera.speed, 0.25f, 20.0f, "%.2f");
  ImGui::SliderFloat("Camera sensitivity", &activeCamera.sensitivity, 0.0005f, 0.01f, "%.4f");
  ImGui::Checkbox("Global ambient light", &engine.globalLight.enabled);
  ImGui::SliderFloat("Global light level", &engine.globalLight.level, 0.0f, 2.0f, "%.2f");
  ImGui::SeparatorText("Transform");
  ImGui::DragFloat3("Cube position", &engine.cube.position.x, 0.02f);
  ImGui::End();

  ImGui::SetNextWindowPos({16.0f, 312.0f}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize({300.0f, 125.0f}, ImGuiCond_FirstUseEver);
  ImGui::Begin("Camera");
  ImGui::Text("Active: %s", engine.focusedView == ViewKind::Scene ? "Scene" : "Game");
  ImGui::Text("Position: %.2f, %.2f, %.2f", activeCamera.position.x, activeCamera.position.y, activeCamera.position.z);
  ImGui::Text("Orientation: %.2f, %.2f, %.2f, %.2f", activeCamera.orientation.x,
              activeCamera.orientation.y, activeCamera.orientation.z, activeCamera.orientation.w);
  ImGui::TextUnformatted(engine.cursorCaptured ? "Mouse captured" : "Mouse released for UI");
  ImGui::End();

  ImGui::SetNextWindowPos({16.0f, 453.0f}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize({300.0f, 115.0f}, ImGuiCond_FirstUseEver);
  ImGui::Begin("Engine");
  ImGui::Text("Frame: %llu", static_cast<unsigned long long>(engine.renderedFrames));
  ImGui::Text("Frame time: %.2f ms", engine.frameTime * 1000.0f);
  ImGui::Text("Swapchain: %u x %u", engine.swapchainExtent.width, engine.swapchainExtent.height);
  ImGui::Text("Images: %zu", engine.swapchainImages.size());
  ImGui::End();

  const auto textureRef = [](VkDescriptorSet texture) {
    return ImTextureRef{static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(texture))};
  };
  const auto displayView = [&engine, &textureRef](ViewKind viewKind) {
    const uint32_t view = static_cast<uint32_t>(viewKind);
    const ImVec2 available = ImGui::GetContentRegionAvail();
    engine.requestedViewExtents[view] = {
        static_cast<uint32_t>(std::max(64.0f, available.x)),
        static_cast<uint32_t>(std::max(64.0f, available.y))};
    ImGui::Image(textureRef(engine.views[view].texture), available);
  };

  ImGui::SetNextWindowPos({420.0f, 16.0f}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize({680.0f, 430.0f}, ImGuiCond_FirstUseEver);
  ImGui::Begin("Scene");
  if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) engine.focusedView = ViewKind::Scene;
  ImGui::TextUnformatted("Editor view: runtime transformations are paused.");
  displayView(ViewKind::Scene);
  ImGui::End();

  ImGui::SetNextWindowPos({420.0f, 462.0f}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize({680.0f, 430.0f}, ImGuiCond_FirstUseEver);
  ImGui::Begin("Game");
  if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) engine.focusedView = ViewKind::Game;
  ImGui::TextUnformatted("Runtime view: camera controls and cube animation are active.");
  displayView(ViewKind::Game);
  ImGui::End();

  if (engine.exitConfirmationOpen) ImGui::OpenPopup("Exit Blossom?");
  if (ImGui::BeginPopupModal("Exit Blossom?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    if (!engine.exitConfirmationOpen) {
      ImGui::CloseCurrentPopup();
    } else {
      ImGui::TextUnformatted("Exit the application?");
      ImGui::TextUnformatted("Enter: Yes    Escape: No");
      if (ImGui::Button("Yes", {120.0f, 0.0f})) glfwSetWindowShouldClose(engine.window, GLFW_TRUE);
      ImGui::SameLine();
      if (ImGui::Button("No", {120.0f, 0.0f})) {
        engine.exitConfirmationOpen = false;
        if (engine.resumeInputAfterExitConfirmation) setCursorCapture(engine, true);
        engine.resumeInputAfterExitConfirmation = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::SetItemDefaultFocus();
    }
    ImGui::EndPopup();
  }
  ImGui::Render();
}

void initialize(Engine& engine) {
  if (!glfwInit()) throw std::runtime_error("glfwInit failed");
  engine.glfwStarted = true;
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  engine.window = glfwCreateWindow(windowWidth, windowHeight, "Blossom Vulkan", nullptr, nullptr);
  if (!engine.window) throw std::runtime_error("glfwCreateWindow failed");
  glfwSetWindowUserPointer(engine.window, &engine);
  glfwSetFramebufferSizeCallback(engine.window, onFramebufferResize);
  glfwSetInputMode(engine.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  uint32_t extensionCount = 0;
  const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
  VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO};
  app.pApplicationName = "Blossom";
  app.pEngineName = "Blossom";
  app.apiVersion = VK_API_VERSION_1_0;
  VkInstanceCreateInfo instanceInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  instanceInfo.pApplicationInfo = &app;
  instanceInfo.enabledExtensionCount = extensionCount;
  instanceInfo.ppEnabledExtensionNames = extensions;
  check(vkCreateInstance(&instanceInfo, nullptr, &engine.instance), "create Vulkan instance");
  check(glfwCreateWindowSurface(engine.instance, engine.window, nullptr, &engine.surface), "create GLFW surface");
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(engine.instance, &deviceCount, nullptr);
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(engine.instance, &deviceCount, devices.data());
  for (VkPhysicalDevice device : devices) {
    QueueFamilies queues{};
    if (supportsDevice(device, engine.surface, queues)) { engine.physicalDevice = device; engine.queues = queues; break; }
  }
  if (!engine.physicalDevice) throw std::runtime_error("No Vulkan device can present to this window");
  const float priority = 1.0f;
  std::vector<uint32_t> families{engine.queues.graphics};
  if (engine.queues.present != engine.queues.graphics) families.push_back(engine.queues.present);
  std::vector<VkDeviceQueueCreateInfo> queues;
  for (uint32_t family : families) {
    VkDeviceQueueCreateInfo info{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    info.queueFamilyIndex = family;
    info.queueCount = 1;
    info.pQueuePriorities = &priority;
    queues.push_back(info);
  }
  const char* extensionsForDevice[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  VkDeviceCreateInfo deviceInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queues.size());
  deviceInfo.pQueueCreateInfos = queues.data();
  deviceInfo.enabledExtensionCount = 1;
  deviceInfo.ppEnabledExtensionNames = extensionsForDevice;
  check(vkCreateDevice(engine.physicalDevice, &deviceInfo, nullptr, &engine.device), "create logical device");
  vkGetDeviceQueue(engine.device, engine.queues.graphics, 0, &engine.graphicsQueue);
  vkGetDeviceQueue(engine.device, engine.queues.present, 0, &engine.presentQueue);
  VkCommandPoolCreateInfo commandPool{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
  commandPool.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPool.queueFamilyIndex = engine.queues.graphics;
  check(vkCreateCommandPool(engine.device, &commandPool, nullptr, &engine.commandPool), "create command pool");
  createSceneResources(engine);
  createImGuiPool(engine);
  createSwapchain(engine);
  VkSemaphoreCreateInfo semaphore{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  VkFenceCreateInfo fence{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  fence.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  for (FrameSync& frame : engine.frames) {
    check(vkCreateSemaphore(engine.device, &semaphore, nullptr, &frame.imageAvailable), "create image semaphore");
    check(vkCreateSemaphore(engine.device, &semaphore, nullptr, &frame.renderFinished), "create render semaphore");
    check(vkCreateFence(engine.device, &fence, nullptr, &frame.finished), "create frame fence");
  }
  initializeImGui(engine);
  engine.previousTime = glfwGetTime();
}

void shutdown(Engine& engine) {
  if (engine.device) vkDeviceWaitIdle(engine.device);
  shutdownImGui(engine);
  if (engine.device) {
    for (const FrameSync& frame : engine.frames) {
      vkDestroyFence(engine.device, frame.finished, nullptr);
      vkDestroySemaphore(engine.device, frame.renderFinished, nullptr);
      vkDestroySemaphore(engine.device, frame.imageAvailable, nullptr);
    }
    destroySwapchain(engine);
    vkDestroyDescriptorPool(engine.device, engine.imguiDescriptorPool, nullptr);
    destroySceneResources(engine);
    vkDestroyCommandPool(engine.device, engine.commandPool, nullptr);
    vkDestroyDevice(engine.device, nullptr);
  }
  if (engine.surface) vkDestroySurfaceKHR(engine.instance, engine.surface, nullptr);
  if (engine.instance) vkDestroyInstance(engine.instance, nullptr);
  if (engine.window) glfwDestroyWindow(engine.window);
  if (engine.glfwStarted) glfwTerminate();
}
