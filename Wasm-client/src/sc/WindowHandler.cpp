#include "WindowHandler.h"

#include <iostream>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#define	 STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#ifdef __EMSCRIPTEN__
#include "EmscriptenGlue.h"
#endif

void WindowHandler::initialize()
{
	glfwInit();

#if defined __EMSCRIPTEN__
	setup_broadcast_channel();
	initializeEmscriptenCanvas();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#endif
	window = glfwCreateWindow(screenWidth, screenHeight, reinterpret_cast<const char*>(u8"Shutter Controller - Margitai Péter Máté"), NULL, NULL);
	glfwSetWindowUserPointer(window, static_cast<void*>(this));
	
	//glfwSetWindowPosCallback(window, window_pos_callback);
	glfwSetWindowPos(window, 100, 100);

	//stbi_set_flip_vertically_on_load(true);
	int componentNr = 0;
	GLFWimage windowIcon;
	windowIcon.pixels = stbi_load("./assets/shutter.png", &windowIcon.width, &windowIcon.height, &componentNr, 4);

	glfwSetWindowIcon(window, 1, &windowIcon);

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		int a;
		std::cin >> a;
		//return -1;
		return;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		//std::cout << "SIZE CHANGED - W: " << width << ", H: " << height << std::endl;

		auto self = static_cast<WindowHandler*>(glfwGetWindowUserPointer(window));
		self->screenWidth = width;
		self->screenHeight = height;
		self->updateViewPort();

		for (auto it = self->displaySizeChangedReceivers.begin(); it != self->displaySizeChangedReceivers.end(); it++)
		{
			if (it->second != nullptr)
			{
				it->second(width, height);
			}			
		}
	});

#if !defined __EMSCRIPTEN__
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		int a;
		std::cin >> a;
		//return -1;
		return;
	}
#endif

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	
	ImGuiIO& io = ImGui::GetIO();

	ImVector<ImWchar> ranges;
	ImFontGlyphRangesBuilder builder;
	builder.AddText(reinterpret_cast<const char*>(u8"áéíóöőúüű°"));

	builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
	builder.BuildRanges(&ranges);

	io.Fonts->AddFontFromFileTTF("./assets/Roboto-Medium.ttf", 16, nullptr, ranges.Data);
	io.Fonts->Build();

	//std::cout<<"GL VERSION: "<<glGetString(GL_VERSION)<<std::endl;
	//int major, minor, rev;
	//glfwGetVersion(&major, &minor, &rev);
	//std::cout<<"GLFW - major: "<<major<<", minor: "<<minor<<", rev: "<<rev<<std::endl;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplOpenGL3_Init("#version 300 es");
#else 
	ImGui_ImplOpenGL3_Init("#version 460 core");
#endif // __EMSCRIPTEN__	

	ImGui::StyleColorsDark();

	glEnable(GL_DEPTH_TEST);
}

void WindowHandler::terminate()
{
	glfwTerminate();
	ImGui::DestroyContext();
}

void WindowHandler::startFrame()
{
	glClearColor(0.45, 0.55, 0.60, 1.00);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
}

void WindowHandler::endFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool WindowHandler::shouldStop()
{
	return glfwWindowShouldClose(window);
}

void WindowHandler::updateViewPort()
{
	
}

#ifdef __EMSCRIPTEN__
void WindowHandler::initializeEmscriptenCanvas()
{
	screenWidth = canvas_get_width();
	screenHeight = canvas_get_height();
	
	//auto result = emscripten_set_canvas_element_size("canvas", screenWidth, screenHeight);
}
#endif