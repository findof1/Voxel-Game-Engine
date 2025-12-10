#pragma once
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

GLFWwindow *initWindow(const int w, const int h, std::string windowName);
void closeWindow(GLFWwindow *window);

VkInstance createInstance(GLFWwindow *window);
void destroyInstance(VkInstance instance);
