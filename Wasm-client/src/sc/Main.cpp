#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>

#include <imgui/imgui.h>

#include "WindowHandler.h"
#include "MainWindow.h"

std::shared_ptr<WindowHandler> windowHandler;
std::shared_ptr<MainWindow> mainWindow;

std::shared_ptr<std::map<std::string, std::string>> loadMapFile(std::string path);

inline void update() {
	windowHandler->startFrame();

	mainWindow->update();

	windowHandler->endFrame();
}

int main() {
	windowHandler = std::make_shared<WindowHandler>();
	windowHandler->initialize();

	std::shared_ptr<std::map<std::string, std::string>> translation = loadMapFile("./assets/translation_en.txt");

	mainWindow = std::make_shared<MainWindow>();
	mainWindow->setTranslation(translation);
#ifndef __EMSCRIPTEN__
	mainWindow->initializeOfflineRequests(loadMapFile("./resources/connection.txt"));
#endif
	mainWindow->initializeRequests();	
	mainWindow->setWindowSize(windowHandler->getScreenWidth(), windowHandler->getScreenHeight());

	windowHandler->subscribeToDisplaySizeChange([&](int width, int height) -> void {
		//mainWindow->setWindowSize(width, height);
	});

	std::chrono::system_clock::time_point prevFrameStart = std::chrono::system_clock::now();

#if defined __EMSCRIPTEN__
	emscripten_set_main_loop_arg((em_arg_callback_func)update, windowHandler->getGLFWWindow(), 60, 1);
#else
	while (!windowHandler->shouldStop()) {
		std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - prevFrameStart;

		if (work_time.count() < 1000.0f / 30) {
			std::chrono::duration<double, std::milli> delta_ms(1000.0f / 30 - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		prevFrameStart = std::chrono::system_clock::now();

		update();
	}
#endif
	windowHandler->terminate();
	mainWindow->terminate();

	return 0;
}

std::shared_ptr<std::map<std::string, std::string>> loadMapFile(std::string path) {
	std::shared_ptr<std::map<std::string, std::string>> map = std::make_shared<std::map<std::string, std::string>>();

	std::string row;
	std::fstream mapFile;
	mapFile.open(path);
	while (std::getline(mapFile, row, '\n')) {
		if (row.length() < 3) {
			continue;
		}

		for (int i = 0; i < row.length(); i++)
		{
			if (row[i] == ' ')
			{
				std::string key = row.substr(0, i);
				std::string value = row.substr(i + 1);

				(*map)[key] = value;
				break;
			}
		}
	}

	return map;
}