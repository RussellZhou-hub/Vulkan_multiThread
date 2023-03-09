/*
// 创建UBO的uniform buffer
VkBuffer uniformBuffer;
VkDeviceMemory uniformBufferMemory;
createBuffer(device, physicalDevice, sizeof(UBO),
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory);

// 更新uniform buffer的数据
void updateUniformBuffer() {
  UBO ubo = {};
  ubo.model = glm::mat4(1.0f);
  ubo.view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  ubo.projection = glm::perspective(glm::radians(45.0f),
      swapchainExtent.width / (float) swapchainExtent.height, 0.1f, 10.0f);
  ubo.projection[1][1] *= -1; // 翻转Y轴坐标系以匹配Vulkan
  void* data;
  vkMapMemory(device, uniformBufferMemory, 0, sizeof(ubo), 0, &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(device, uniformBufferMemory);
}

// 在渲染立方体之前绑定uniform buffer
void renderCube(VkCommandBuffer commandBuffer) {
  // 绑定uniform buffer
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, offsets);
  vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

  // 渲染立方体
  vkCmdDrawIndexed(commandBuffer, 36, 1, 0, 0, 0);
}

// 定义鼠标移动回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // 翻转Y轴
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f; // 鼠标灵敏度
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  // 限制俯仰角的范围在[-89, 89]度之间
  if (pitch > 89.0f) {
    pitch = 89.0f;
  }
  if (pitch < -89.0f) {
    pitch = -89.0f;
  }

  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(front);

  updateUniformBuffer();
}

// 定义滚轮滚动回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  float sensitivity = 0.1f; // 鼠标灵敏度
  if (fov >= 1.0f && fov <= 45.0f) {
    fov -= yoffset * sensitivity;
  }
  if (fov <= 1.0f) {
    fov = 1.0f;
  }
  if (fov >= 45.0f) {
    fov = 45.0f;
  }

  updateUniformBuffer();
}

const std::vector<Vertex> vertices = {
  // 前面
  {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
  {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
  {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
  {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},

  // 后面
  {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
  {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
  {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
  {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},

  // 上面
  {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
  {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
  {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
  {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},

  // 底面
  {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
  {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
  {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
  {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},

  // 左面
  {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
  {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
  {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0


const std::vector<uint32_t> indices = {
  // 前面
  0, 1, 2,
  2, 3, 0,

  // 后面
  4, 5, 6,
  6, 7, 4,

  // 上面
  8, 9, 10,
  10, 11, 8,

  // 底面
  12, 13, 14,
  14, 15, 12,

  // 左面
  16, 17, 18,
  18, 19, 16,

  // 右面
  20, 21, 22,
  22, 23, 20
};

*/