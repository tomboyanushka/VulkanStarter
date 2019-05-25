#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>


class TriangleApp
{

public:
	void Run();


private:
	GLFWwindow* window;
	VkInstance instance;
	const int WIDTH = 800;
	const int HEIGHT = 600;

	void InitVulkan();
	void MainLoop();
	void Cleanup();
	void InitWindow();
	void CreateInstance();

};

