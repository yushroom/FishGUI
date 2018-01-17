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
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()


if (NOT WIN32)
	# boost
	set(Boost_USE_STATIC_LIBS       ON)
	set(Boost_USE_MULTITHREADED     ON)
	set(Boost_USE_STATIC_RUNTIME    OFF)
	find_package(Boost 1.58 REQUIRED COMPONENTS system filesystem)
	include_directories(${Boost_INCLUDE_DIRS})
	MESSAGE("Found Boost: " ${Boost_INCLUDE_DIRS})
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

if (NOT APPLE)
	# glew
	set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
	ADD_SUBDIRECTORY( ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/glew/build/cmake )
	include_directories( ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/glew/include )
	SET_TARGET_PROPERTIES(glew PROPERTIES FOLDER "ThirdParty")
	SET_TARGET_PROPERTIES(glew_s PROPERTIES FOLDER "ThirdParty")
endif()

add_library(FishGUI STATIC ${SRCS} ${HEADERS} ${NANO_VG_SRCS} ${NANO_VG_HEADERS})


if (FISHGUI_BUILD_EXAMPLES)
	# glfw
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
	ADD_SUBDIRECTORY(./ThirdParty/glfw/)
	SET_TARGET_PROPERTIES(glfw PROPERTIES FOLDER "ThirdParty")

	if (APPLE)
		file( GLOB FONTS ${CMAKE_CURRENT_LIST_DIR}/resources/fonts/*.ttf)
		set( RESOURCES ${FONTS} )
		# set_source_files_properties(Demo PROPERTIES MACOSX_PACKAGE_LOCATION fonts)
	endif()

	FILE(GLOB_RECURSE EXAMPLES_COMMON ${CMAKE_CURRENT_LIST_DIR}/examples/common/*.hpp ${CMAKE_CURRENT_LIST_DIR}/examples/common/*.cpp)
	FILE(GLOB_RECURSE DEMO_SRC ${CMAKE_CURRENT_LIST_DIR}/examples/demo1/*)
	add_executable(Demo MACOSX_BUNDLE ${DEMO_SRC} ${EXAMPLES_COMMON} ${RESOURCES})
	set_target_properties(Demo PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/cmake/MacOSXBundleInfo.plist.in)

	source_group("common" FILES ${EXAMPLES_COMMON})
	source_group("Resources" FILES ${RESOURCES})

	target_link_libraries(Demo FishGUI glfw)
	# target_link_libraries(Demo ${Boost_LIBRARIES})

	if (WIN32)
		target_link_libraries(Demo opengl32.lib)
		target_link_libraries(Demo glew_s)
	endif()
	if (APPLE)
		target_link_libraries(Demo ${Boost_LIBRARIES})
		set_target_properties(Demo PROPERTIES LINK_FLAGS "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
		# file( GLOB FONTS ${CMAKE_CURRENT_LIST_DIR}/resources/fonts/*.ttf)
		set_source_files_properties(${RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/fonts)
	endif()
	if (LINUX)
		target_link_libraries(Demo ${Boost_LIBRARIES})
		target_link_libraries(Demo glew_s)
	endif()

	if (NOT APPLE)
		# copy fonts
		add_custom_command(TARGET Demo POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources/fonts $<TARGET_FILE_DIR:Demo>/fonts)
	endif()
endif() # FISHGUI_BUILD_EXAMPLES
