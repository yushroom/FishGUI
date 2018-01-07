CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0 FATAL_ERROR)

PROJECT(FishGUI)

SET(FishGUI_VERSION_MAJOR 0)
SET(FishGUI_VERSION_MINOR 1)
SET(FishGUI_VERSION_PATCH 0)
SET(FishGUI_VERSION ${FishGUI_VERSION_MAJOR}.${FishGUI_VERSION_MINOR}.${FishGUI_VERSION_PATCH})

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

set(FishGUI_SCRIPT_DIR ${CMAKE_CURRENT_LIST_DIR}/../Script)

if (LINUX)
	SET(CMAKE_BUILD_TYPE "Debug")  
	SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
	SET(CMAKE_C_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
endif()



include_directories(./src)
include_directories(./ThirdParty/)
include_directories(./ThirdParty/nanovg)

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

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
include_directories(./ThirdParty/glfw/include)
SET_TARGET_PROPERTIES(glfw PROPERTIES FOLDER "ThirdParty")

aux_source_directory( ${CMAKE_CURRENT_LIST_DIR}/src SRCS )
FILE(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/*.hpp ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

FILE(GLOB_RECURSE NANO_VG_SRCS ./ThirdParty/nanovg/*.c)
FILE(GLOB_RECURSE NANO_VG_HEADERS ./ThirdParty/nanovg/*.h)

source_group("nanovg" FILES ${NANO_VG_SRCS} ${NANO_VG_HEADERS})

add_executable(Demo ${SRCS} ${HEADERS} ${NANO_VG_SRCS} ${NANO_VG_HEADERS})

target_link_libraries(Demo glfw)

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
