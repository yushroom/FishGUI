CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0 FATAL_ERROR)

PROJECT(FishGUI)

SET(FishGUI_VERSION_MAJOR 0)
SET(FishGUI_VERSION_MINOR 1)
SET(FishGUI_VERSION_PATCH 0)
SET(FishGUI_VERSION ${FishGUI_VERSION_MAJOR}.${FishGUI_VERSION_MINOR}.${FishGUI_VERSION_PATCH})

option(FISHGUI_BUILD_EXAMPLES "Build the FishGUI example programs" ON)
# option(FISHGUI_BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Global compile & linker flags

# Target at least C++14
set(CMAKE_CXX_STANDARD 14)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# Output
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/Binary)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

if (LINUX)
	SET(CMAKE_BUILD_TYPE "Debug")  
	SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
	SET(CMAKE_C_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
# include_directories(./src)
include_directories( ${CMAKE_CURRENT_LIST_DIR}/ThirdParty )
include_directories( ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/nanovg )
include_directories( ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/stb )
include_directories( ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/glfw/include )
aux_source_directory( ${CMAKE_CURRENT_LIST_DIR}/src SRCS )
FILE(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

# nanovg
FILE(GLOB_RECURSE NANO_VG_SRCS 		${CMAKE_CURRENT_LIST_DIR}/ThirdParty/nanovg/*.c)
FILE(GLOB_RECURSE NANO_VG_HEADERS 	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/nanovg/*.h)
source_group("nanovg" FILES ${NANO_VG_SRCS} ${NANO_VG_HEADERS})


add_library(FishGUI STATIC ${SRCS} ${HEADERS} ${NANO_VG_SRCS} ${NANO_VG_HEADERS})


if (FISHGUI_BUILD_EXAMPLES)

	if (NOT APPLE)
		# glew
		set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
		ADD_SUBDIRECTORY(./ThirdParty/glew/build/cmake)
		include_directories(./ThirdParty/glew/include)
		SET_TARGET_PROPERTIES(glew PROPERTIES FOLDER "ThirdParty")
		SET_TARGET_PROPERTIES(glew_s PROPERTIES FOLDER "ThirdParty")
	endif()

	# glfw
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
	ADD_SUBDIRECTORY(./ThirdParty/glfw/)
	SET_TARGET_PROPERTIES(glfw PROPERTIES FOLDER "ThirdParty")


	FILE(GLOB_RECURSE DEMO_SRC ./examples/demo1/*)
	add_executable(Demo MACOSX_BUNDLE ${DEMO_SRC})
	set_target_properties(Demo PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/cmake/MacOSXBundleInfo.plist.in)

	target_link_libraries(Demo glfw FishGUI)

	if (WIN32)
		target_link_libraries(Demo opengl32.lib)
		target_link_libraries(Demo glew_s)
	endif()
	if (APPLE)
		set_target_properties(Demo PROPERTIES LINK_FLAGS "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
	endif()
	if (LINUX)
		SET(CMAKE_BUILD_TYPE "Debug")  
		SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
		SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
		target_link_libraries(Demo glew_s)
	endif()
endif() # FISHGUI_BUILD_EXAMPLES
