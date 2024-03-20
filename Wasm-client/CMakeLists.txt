project("ShutterClient")

cmake_minimum_required (VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#[[
First clone the protobuf with its submodules and add these lines to its CMakeLists.txt to build it properly:

if(MSVC)
    add_compile_options(/bigobj)
elseif(EMSCRIPTEN)
    add_compile_options(-pthread -sPTHREAD_POOL_SIZE=1 -sALLOW_MEMORY_GROWTH=1 -sNO_DISABLE_EXCEPTION_CATCHING)
endif()

For emscripten use this command to run cmake, for Windows turn set manually the speicified arguments:
emcmake cmake -B./out/build/wasm-release -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_PROTOC_BINARIES=OFF -DABSL_USE_GOOGLETEST_HEAD=OFF -DABSL_PROPAGATE_CXX_STD=ON

The MSVC static runtime option should be turned on. After building, install the protobuf library (in VS build->install protobuf, with emscripten run: "cmake --install ." at the build directory.

After installing set the installation path for the "shutter_wasm_proto_dir" variable below.
]]

set(shutter_wasm_proto_dir "" CACHE STRING "Path to protobuf installation directory")

# to build emscripten version: emcmake cmake -B./out/build/wasm-release -G "Ninja" -DCMAKE_BUILD_TYPE=Release
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
	"src/Request/DemoRequest.h"
	"src/EmscriptenGlue.h"
	"src/EmscriptenGlue.cpp"
	"src/Shutter.pb.h"
	"src/Shutter.pb.cc"

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
	set_property(TARGET ShutterClient PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
	)

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

	# set the application to be multithreaded for MSVC
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
	# generate html file
	set(CMAKE_EXECUTABLE_SUFFIX ".html")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fexperimental-library")
	
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
		--preload-file ${CMAKE_SOURCE_DIR}/resources/translation_en.txt@/resources/translation_en.txt \ 
		--bind"
	)
	
	# add compile options
	target_compile_options(ShutterClient PUBLIC
		"-pthread;-sPTHREAD_POOL_SIZE=1;-sALLOW_MEMORY_GROWTH=1;-sNO_DISABLE_EXCEPTION_CATCHING;"	
	)
endif()

list(APPEND CMAKE_PREFIX_PATH "${shutter_wasm_proto_dir}") 
find_package(Protobuf CONFIG REQUIRED)

target_link_libraries(ShutterClient PUBLIC 
	"protobuf::libprotobuf"
)

# windows specific settings
if (WIN32)
	# Release mode
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		# hide Console Window
		set_target_properties(ShutterClient
			PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
		)

		# runtime libraries, earlier they are necessary when the app have to be build with them statically
		target_link_libraries(ShutterClient PRIVATE
			#msvcmrt
			msvcrt
		)

	# Debug mode
	else()
		# show console window
		set_target_properties(ShutterClient
			PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
		)
	endif()
endif()
