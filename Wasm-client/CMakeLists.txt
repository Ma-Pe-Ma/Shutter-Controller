cmake_minimum_required (VERSION 3.20.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project("ShutterClient")

# specify OpenSSL installation path for desktop build
set(OPEN_SSL_PATH "c:/Program Files/OpenSSL/" CACHE STRING "Path to the OpenSSL installation")

# copy the used message proto file to this project
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../ESP8266_Server/src/app/Shutter.proto" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/src/sc/")

# function to collect all currently added targets in a directory's all subdirectories
function(get_all_targets result_list directory)
	get_property(sub_dirs DIRECTORY "${directory}" PROPERTY SUBDIRECTORIES)
	foreach(sub_dir IN LISTS sub_dirs)
		get_all_targets(${result_list} "${sub_dir}")
	endforeach()

	get_directory_property(sub_targets DIRECTORY "${directory}" BUILDSYSTEM_TARGETS)
	set(${result_list} ${${result_list}} ${sub_targets} PARENT_SCOPE)
endfunction()

# to build emscripten version: emcmake cmake -B./out/build/wasm-release -G "Ninja" -DCMAKE_BUILD_TYPE=Release
add_executable(ShutterClient
	"src/sc/Main.cpp"
	"src/sc/WindowHandler.h"
	"src/sc/WindowHandler.cpp"
	"src/sc/MainWindow.h"
	"src/sc/MainWindow.cpp"
	"src/sc/Timing.h"
	"src/sc/Message.h"
	"src/sc/RequestQueue.h"
	"src/sc/RequestQueue.cpp"	
	"src/sc/request/Request.h"
	"src/sc/request/SimpleRequest.h"
	"src/sc/request/EmscriptenRequest.h"
	"src/sc/request/DemoRequest.h"
	"src/sc/EmscriptenGlue.h"
	"src/sc/EmscriptenGlue.cpp"

	"src/submodules/imgui/backends/imgui_impl_opengl3.cpp"
	"src/submodules/imgui/backends/imgui_impl_glfw.cpp"
	"src/submodules/imgui/imgui.cpp"
	"src/submodules/imgui/imgui_draw.cpp"
	"src/submodules/imgui/imgui_tables.cpp"
	"src/submodules/imgui/imgui_widgets.cpp"

	"src/sc/Shutter.proto"
)

target_include_directories(ShutterClient PRIVATE
	"src/submodules/"
	"src/submodules/imgui"
	"src/submodules/glfw/include"
	"src/protobuf/src"
)

if (WIN32)
	file(WRITE "assets/icon.rc" "IDI_ICON1 ICON \"./shutter.ico\"")

	target_sources(ShutterClient PRIVATE
		"assets/icon.rc"
	)
endif()

set(BUILD_TESTING OFF)
set(protobuf_BUILD_TESTS OFF)
set(protobuf_BUILD_SHARED_LIBS OFF)
set(protobuf_BUILD_PROTOC_BINARIES ON)
set(ABSL_USE_GOOGLETEST_HEAD OFF)
set(ABSL_PROPAGATE_CXX_STD ON)
set(protobuf_MSVC_STATIC_RUNTIME ON)
#set(protobuf_BUILD_LIBUPB OFF)

set(GLFW_LIBRARY_TYPE "STATIC")

add_subdirectory("src/submodules/protobuf")

if(NOT EMSCRIPTEN)
	if(WIN32)
		if(EXISTS ${OPEN_SSL_PATH})
			target_compile_definitions(ShutterClient PUBLIC 
				CPPHTTPLIB_OPENSSL_SUPPORT
			)
			
			target_link_directories(ShutterClient PRIVATE
				"${OPEN_SSL_PATH}/lib/"
			)

			target_include_directories(ShutterClient PRIVATE
				"${OPEN_SSL_PATH}/include/"
			)

			target_link_libraries(ShutterClient PRIVATE
				"libssl"	
				"libcrypto"
			)
		endif()		

		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/connection.txt")
			file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/connection.txt" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/resources")
		endif()
	
		# configure runtime library for target
		if(MSVC)
			set_property(TARGET ShutterClient PROPERTY
				MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
			)

			# Release mode
			if (CMAKE_BUILD_TYPE STREQUAL "Release")
				# hide Console Window
				set_target_properties(ShutterClient PROPERTIES
					LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
				)

				target_compile_options(ShutterClient PRIVATE 
					"/MT"
				)   

				# link the static runtime library
				target_link_libraries(ShutterClient PRIVATE
					MSVCRT
				)
			# Debug mode
			else()
				# show console window
				set_target_properties(ShutterClient PROPERTIES
					LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
				)

				target_compile_options(ShutterClient PRIVATE 
					"/MTd"
				)   

				target_link_libraries(ShutterClient PRIVATE
					MSVCRTD
				)
			endif()
		endif()
	elseif(UNIX)
		target_compile_definitions(ShutterClient PUBLIC 
			CPPHTTPLIB_OPENSSL_SUPPORT
		)

		find_package(OpenSSL REQUIRED)

		target_link_libraries(ShutterClient PRIVATE
			OpenSSL::SSL
			OpenSSL::Crypto
		)
	endif()

	target_sources(ShutterClient PRIVATE
		"src/submodules/glad.c"
	)

	# copy font to resources folder
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/submodules/imgui/misc/fonts/Roboto-Medium.ttf" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/assets/")

	# build glfw library
	add_subdirectory("src/submodules/glfw")

	# link libraries with the app
	target_link_libraries(ShutterClient PRIVATE
		glfw
	)
else()
	file(GLOB ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/*")

	set_target_properties(ShutterClient	PROPERTIES 
		SUFFIX ".html" # generate html
		LINK_FLAGS "--preload-file ${CMAKE_SOURCE_DIR}/assets@/assets/ --bind" # add resource files to build
		LINK_DEPENDS "${ASSET_FILES}" # rebuild if an asset is changed
	)

	target_compile_options(ShutterClient PRIVATE	
		"-pthread;"
		"-sASSERTIONS=3;"
	)

	target_link_options(ShutterClient PUBLIC
		"-pthread;"
		"-sLLD_REPORT_UNDEFINED;"
		"-sMAX_WEBGL_VERSION=2;-sMIN_WEBGL_VERSION=2;-sFETCH;--profiling-funcs;"
		"-sPTHREAD_POOL_SIZE=1;-sALLOW_MEMORY_GROWTH;"
		"-sNO_DISABLE_EXCEPTION_CATCHING;"
		"-std=c++20;-fexperimental-library;"
		"-sUSE_GLFW=3;"
	)

	# set pthread support for protobuf/absl targets
	get_all_targets(proto_targets "${CMAKE_CURRENT_SOURCE_DIR}/src/submodules/protobuf")

	foreach(proto_target ${proto_targets})
		target_compile_options(${proto_target} INTERFACE
			"-pthread;"
		)
	endforeach()

	target_compile_options(time_zone PUBLIC
		"-pthread;"
	)

	# protoc is used to generate the protobuf sources, while using emscripten it's get called with node
	# it needs to be compiled with these flags to use the compiling system's paths (to generate the sources) instead of Emscripten's virtual FS
	target_compile_options(protoc PRIVATE
		"-sENVIRONMENT=node"
		"-sNODERAWFS=1"
	)

	target_link_options(protoc PRIVATE
		"-sENVIRONMENT=node"
		"-sNODERAWFS=1"
	)
endif()

add_dependencies(ShutterClient protobuf::libprotobuf)

target_link_libraries(ShutterClient PUBLIC 
	"protobuf::libprotobuf"
)

include(src/submodules/protobuf/cmake/protobuf-generate.cmake)
PROTOBUF_GENERATE(
	TARGET ShutterClient
	LANGUAGE cpp
	GENERATE_EXTENSIONS .pb.h .pb.cc
	IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/sc/"
	PROTOC_OUT_DIR "${CMAKE_CURRENT_LIST_DIR}/src/sc/"
)

# cmake --install /path/to/build --prefix /path/to/install --component shutter
if(EMSCRIPTEN)
	set(em_target "ShutterClient")
	install(TARGETS ShutterClient COMPONENT shutter RUNTIME DESTINATION ".")
	install(FILES
        "$<TARGET_FILE_DIR:${em_target}>/${em_target}.js"
        "$<TARGET_FILE_DIR:${em_target}>/${em_target}.wasm"
		"$<TARGET_FILE_DIR:${em_target}>/${em_target}.data"
		COMPONENT shutter
        DESTINATION ".")
elseif(WIN32 OR UNIX)
	install(TARGETS ShutterClient COMPONENT shutter RUNTIME DESTINATION ".")
	install(DIRECTORY "assets" COMPONENT shutter DESTINATION "." PATTERN "*.rc" EXCLUDE PATTERN "connection.txt" EXCLUDE PATTERN "*.ico" EXCLUDE)
	install(FILES "../ReadMe.md" "../LICENSE.md" COMPONENT shutter DESTINATION ".")
	
	install(FILES "assets/connection.txt" COMPONENT shutter DESTINATION "./resources/")
endif()
