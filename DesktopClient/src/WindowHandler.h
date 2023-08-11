#ifndef WINDOWING_H
#define WINDOWING_H

#include <memory>
#include <thread>
#include <iostream>
#include <map>
#include <functional>
#include <algorithm>

#if defined __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#else 
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>

class WindowHandler {

private:
	int screenWidth = 460;
	int screenHeight = 540;

	GLFWwindow* window;

	std::map<int, std::function<void(int, int)>> displaySizeChangedReceivers;
public:
	~WindowHandler() {}

	void initialize();
	void terminate();

	void startFrame();
	void endFrame();

	bool shouldStop();
	void updateViewPort();

	float getScreenWidth() { return this->screenWidth; }
	float getScreenHeight() { return this->screenHeight; }

	GLFWwindow* getGLFWWindow()
	{
		return this->window;
	}

	int subscribeToDisplaySizeChange(std::function<void(int, int)> receiver)
	{
		int currentSize = displaySizeChangedReceivers.size();
		displaySizeChangedReceivers[currentSize] = receiver;

		return currentSize;
	}

	void unsubscribeToDisplaySizeChange(int functionID)
	{
		displaySizeChangedReceivers[functionID] = nullptr;
	}

#ifdef __EMSCRIPTEN__
	void initializeEmscriptenCanvas();
#endif
};

#endif