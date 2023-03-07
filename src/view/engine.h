#pragma once
#include "../config.h"
#include "logging.h"

const uint32_t NUM_THREADS = 4;

class Engine{
public:
    Engine(int width, int height);

	~Engine();

    GLFWwindow* window;
    void build_glfw_window(int width, int height);
    void run();
};