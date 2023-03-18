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
*/

/*
const int THREAD_COUNT = 4;
const int COMMAND_BUFFER_COUNT = 10;

VkCommandBuffer secondaryBuffers[THREAD_COUNT][COMMAND_BUFFER_COUNT];
VkCommandPool commandPools[THREAD_COUNT];
std::vector<std::thread> threads;

void submitSecondaryBuffers(int threadIndex) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    for (int i = 0; i < COMMAND_BUFFER_COUNT; i++) {
        VkCommandBuffer cmdBuffer = secondaryBuffers[threadIndex][i];

        vkBeginCommandBuffer(cmdBuffer, &beginInfo);

        // Record commands into the secondary command buffer...

        vkEndCommandBuffer(cmdBuffer);

        // Submit the secondary command buffer to the main thread...
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuffer;

        vkQueueSubmit(mainThreadQueue, 1, &submitInfo, VK_NULL_HANDLE);
    }
}

int main() {
    // Create the main thread's command pool and allocate primary command buffers...

    // Spawn the secondary buffer submission threads...
    for (int i = 0; i < THREAD_COUNT; i++) {
        commandPools[i] = createCommandPool(device, queueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        for (int j = 0; j < COMMAND_BUFFER_COUNT; j++) {
            secondaryBuffers[i][j] = allocateCommandBuffer(device, commandPools[i], VK_COMMAND_BUFFER_LEVEL_SECONDARY);
        }

        threads.emplace_back(submitSecondaryBuffers, i);
    }

    // Wait for all threads to finish...
    for (auto& thread : threads) {
        thread.join();
    }

    // Destroy the command pools...
    for (int i = 0; i < THREAD_COUNT; i++) {
        vkDestroyCommandPool(device, commandPools[i], nullptr);
    }

    return 0;
}
*/