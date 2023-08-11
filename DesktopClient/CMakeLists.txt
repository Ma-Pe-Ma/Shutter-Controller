cmake_minimum_required (VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("ShutterClient")

# to build emscripten version: emcmake cmake -B./out/build/wasm -G "Ninja"
add_executable (ShutterClient
	"src/Main.cpp"
	"src/WindowHandler.h"
	"src/WindowHandler.cpp"
	"src/MainWindow.h"
	"src/MainWindow.cpp"
	"src/Timing.h"
	"src/Message.h"
	"src/RequestQueue.h"
	"src/RequestQueue.cpp"	
	"src/Request/Request.h"
	"src/Request/SimpleRequest.h"
	"src/Request/EmscriptenRequest.h"
	"src/EmscriptenGlue.h"
	"src/EmscriptenGlue.cpp"

	"src/submodules/imgui/backends/imgui_impl_opengl3.cpp"
	"src/submodules/imgui/backends/imgui_impl_glfw.cpp"
	"src/submodules/imgui/imgui.cpp"
	"src/submodules/imgui/imgui_draw.cpp"
	"src/submodules/imgui/imgui_tables.cpp"
	"src/submodules/imgui/imgui_widgets.cpp"
	"src/submodules/glad.c"
)

target_include_directories(ShutterClient PRIVATE
	"src/submodules/"
	"src/submodules/imgui"
	"src/submodules/glfw/include"	
	"src/submodules/json/single_include/nlohmann"
)

if(NOT EMSCRIPTEN)
	# specify open ssl installation path!
	set(OPEN_SSL_PATH "C:/Path/To/OpenSSL/")

	target_link_directories(ShutterClient PRIVATE
		"${OPEN_SSL_PATH}/lib/"
	)

	target_link_libraries(ShutterClient PRIVATE
		"libssl"	
		"libcrypto"
	)

	target_include_directories(ShutterClient PRIVATE
		"${OPEN_SSL_PATH}/include/"
	)

	# copy font to resources folder
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/submodules/imgui/misc/fonts/Roboto-Medium.ttf" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/resources/")

	# build glfw library
	add_subdirectory("src/submodules/glfw")

	# link libraries with the app
	target_link_libraries(ShutterClient PRIVATE
		glfw
	)
else()
	# generate html file
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
	
	# configure opengl context + pthreads link options
	set(CMAKE_EXE_LINKER_FLAGS 
		"${CMAKE_EXE_LINKER_FLAGS} -sLLD_REPORT_UNDEFINED\
		-sUSE_GLFW=3 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2 -sFETCH --profiling-funcs\
		-pthread -sPTHREAD_POOL_SIZE=1 -sALLOW_MEMORY_GROWTH \
		-sNO_DISABLE_EXCEPTION_CATCHING \
		"
	)

	# add resource files to build
	set_target_properties(ShutterClient	PROPERTIES LINK_FLAGS
		#"--preload-file ${CMAKE_SOURCE_DIR}/resources@/resources/"
		"--preload-file ${CMAKE_SOURCE_DIR}/resources/Roboto-Medium.ttf@/resources/Roboto-Medium.ttf \
		--preload-file ${CMAKE_SOURCE_DIR}/resources/translation_en.txt@/resources/translation_en.txt"
	)
	
	# add compile options
	target_compile_options(ShutterClient PUBLIC
		"-pthread;-sPTHREAD_POOL_SIZE=1;-sALLOW_MEMORY_GROWTH=1;-sNO_DISABLE_EXCEPTION_CATCHING;"	
	)
endif()

# windows specific settings
if (WIN32)
	# Release mode
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		# hide Console Window
		set_target_properties(ShutterClient
			PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
		)

		# runtime libraries, earlier they were necessary
		#target_link_libraries(ShutterClient PRIVATE
			#msvcmrt	
			#msvcrt			
		#)

	# Debug mode
	else()
		# show console window
		set_target_properties(ShutterClient
			PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
		)
	endif()
endif()