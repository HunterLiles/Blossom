#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Math.hpp"
#include "Objects.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

inline constexpr uint32_t windowWidth = 1280;
inline constexpr uint32_t windowHeight = 720;
inline constexpr uint32_t framesInFlight = 2;
inline constexpr uint32_t viewCount = 2;
inline constexpr VkExtent2D editorViewExtent{640, 360};
inline constexpr uint32_t gridVertexCount = 90;
inline constexpr uint32_t shaderFileCount = 4;
inline constexpr size_t projectNameCapacity = 64;

enum class ViewKind : uint32_t { Scene = 0, Game = 1 };

struct QueueFamilies { uint32_t graphics = UINT32_MAX; uint32_t present = UINT32_MAX; };
struct SwapchainSupport {
  VkSurfaceCapabilitiesKHR capabilities{};
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};
struct FrameSync {
  VkSemaphore imageAvailable = VK_NULL_HANDLE;
  VkSemaphore renderFinished = VK_NULL_HANDLE;
  VkFence finished = VK_NULL_HANDLE;
};
struct DepthTarget {
  VkImage image = VK_NULL_HANDLE;
  VkDeviceMemory memory = VK_NULL_HANDLE;
  VkImageView view = VK_NULL_HANDLE;
};
struct ViewTarget {
  VkImage colorImage = VK_NULL_HANDLE;
  VkDeviceMemory colorMemory = VK_NULL_HANDLE;
  VkImageView colorView = VK_NULL_HANDLE;
  VkImage depthImage = VK_NULL_HANDLE;
  VkDeviceMemory depthMemory = VK_NULL_HANDLE;
  VkImageView depthView = VK_NULL_HANDLE;
  VkFramebuffer framebuffer = VK_NULL_HANDLE;
  VkDescriptorSet texture = VK_NULL_HANDLE;
  VkExtent2D extent{};
};
struct Camera {
  math::Vec3 position{0.0f, 0.0f, 3.0f};
  math::Quaternion orientation{};
  float fieldOfView = 90.0f;
  float speed = 3.0f;
  float sensitivity = 0.0025f;
};
struct GlobalLight {
  bool enabled = true;
  float level = 1.0f;
};
struct ShaderFileState {
  std::filesystem::file_time_type modified{};
  bool exists = false;
};
struct UniformData {
  math::Mat4 mvp{};
  float globalLight = 1.0f;
  float padding[3]{};
};

struct Engine {
  GLFWwindow* window = nullptr;
  bool glfwStarted = false;
  bool framebufferResized = false;
  bool cursorCaptured = false;
  bool firstMouse = true;
  bool tabWasDown = false;
  bool shaderReloadWasDown = false;
  bool escapeWasDown = false;
  bool enterWasDown = false;
  bool exitConfirmationOpen = false;
  bool resumeInputAfterExitConfirmation = false;
  double mouseX = 0.0;
  double mouseY = 0.0;
  double previousTime = 0.0;
  float frameTime = 0.0f;
  uint64_t renderedFrames = 0;
  double lastShaderPollTime = 0.0;
  bool shaderReloadRequested = false;
  std::array<ShaderFileState, shaderFileCount> shaderFiles{};
  std::string shaderReloadStatus = "Waiting for shader changes";
  std::filesystem::path projectsRoot;
  std::filesystem::path activeProjectPath;
  std::filesystem::path selectedProjectFile;
  std::array<char, projectNameCapacity> newProjectName{};
  std::string projectWorkspaceStatus;
  Camera camera{};
  Camera sceneCamera{{0.0f, 1.5f, 5.0f}};
  ViewKind focusedView = ViewKind::Game;
  Cube cube{};
  GlobalLight globalLight{};

  VkInstance instance = VK_NULL_HANDLE;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  QueueFamilies queues{};
  VkDevice device = VK_NULL_HANDLE;
  VkQueue graphicsQueue = VK_NULL_HANDLE;
  VkQueue presentQueue = VK_NULL_HANDLE;
  VkSwapchainKHR swapchain = VK_NULL_HANDLE;
  VkFormat swapchainFormat = VK_FORMAT_UNDEFINED;
  VkExtent2D swapchainExtent{};
  std::vector<VkImage> swapchainImages;
  std::vector<VkImageView> imageViews;
  std::vector<DepthTarget> depthTargets;
  VkFormat depthFormat = VK_FORMAT_UNDEFINED;
  VkRenderPass renderPass = VK_NULL_HANDLE;
  VkRenderPass viewRenderPass = VK_NULL_HANDLE;
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;
  VkPipeline gridPipeline = VK_NULL_HANDLE;
  std::vector<VkFramebuffer> framebuffers;
  std::array<ViewTarget, viewCount> views{};
  std::array<VkExtent2D, viewCount> viewExtents{editorViewExtent, editorViewExtent};
  std::array<VkExtent2D, viewCount> requestedViewExtents{editorViewExtent, editorViewExtent};
  VkCommandPool commandPool = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> commandBuffers;
  std::array<FrameSync, framesInFlight> frames{};
  uint32_t frameIndex = 0;
  VkDescriptorSetLayout sceneSetLayout = VK_NULL_HANDLE;
  VkDescriptorPool sceneDescriptorPool = VK_NULL_HANDLE;
  std::array<std::array<VkDescriptorSet, viewCount>, framesInFlight> sceneSets{};
  std::array<std::array<VkBuffer, viewCount>, framesInFlight> uniformBuffers{};
  std::array<std::array<VkDeviceMemory, viewCount>, framesInFlight> uniformMemory{};
  std::array<std::array<void*, viewCount>, framesInFlight> uniformMapped{};
  VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;
  bool imguiReady = false;
};
