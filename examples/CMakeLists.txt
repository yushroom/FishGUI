if (NOT WIN32)
	# boost
	set(Boost_USE_STATIC_LIBS       ON)
	set(Boost_USE_MULTITHREADED     ON)
	set(Boost_USE_STATIC_RUNTIME    OFF)
	find_package(Boost 1.58 REQUIRED COMPONENTS system filesystem)
	include_directories(${Boost_INCLUDE_DIRS})
	MESSAGE("Found Boost: " ${Boost_INCLUDE_DIRS})
endif()


if (APPLE)
	file( GLOB FONTS ${CMAKE_CURRENT_LIST_DIR}/../resources/fonts/*.ttf)
	file( GLOB MAINMENU_XIB ${CMAKE_CURRENT_LIST_DIR}/../resources/MainMenu.xib)
	# file( GLOB MAINMENU_NIB ${CMAKE_CURRENT_LIST_DIR}/../resources/MainMenu.nib)
	set( RESOURCES ${FONTS} ${MAINMENU_XIB})
	# set_source_files_properties(Demo PROPERTIES MACOSX_PACKAGE_LOCATION fonts)
endif()

FILE(GLOB_RECURSE EXAMPLES_COMMON ${CMAKE_CURRENT_LIST_DIR}/common/*.hpp ${CMAKE_CURRENT_LIST_DIR}/common/*.cpp)
FILE(GLOB_RECURSE DEMO_SRC ${CMAKE_CURRENT_LIST_DIR}/demo1/*)
add_executable(Demo MACOSX_BUNDLE ${DEMO_SRC} ${EXAMPLES_COMMON} ${RESOURCES})
set_target_properties(Demo PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/../cmake/MacOSXBundleInfo.plist.in)

source_group("common" FILES ${EXAMPLES_COMMON})
source_group("Resources" FILES ${RESOURCES})

target_link_libraries(Demo FishGUI glfw)
# target_link_libraries(Demo ${Boost_LIBRARIES})
set_target_properties(Demo PROPERTIES RESOURCE "${MAINMENU_XIB}")

if (WIN32)
	target_link_libraries(Demo opengl32.lib)
	target_link_libraries(Demo glew_s)
elseif (APPLE)
	target_link_libraries(Demo ${Boost_LIBRARIES})
	set_target_properties(Demo PROPERTIES LINK_FLAGS "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
	# file( GLOB FONTS ${CMAKE_CURRENT_LIST_DIR}/resources/fonts/*.ttf)
	set_source_files_properties(${FONTS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/fonts)
	# set_source_files_properties(${MAINMENU_NIB} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
elseif (LINUX)
	target_link_libraries(Demo ${Boost_LIBRARIES})
	target_link_libraries(Demo glew_s)
endif()

if (NOT APPLE)
	# copy fonts
	add_custom_command(TARGET Demo POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/../resources/fonts $<TARGET_FILE_DIR:Demo>/fonts)
endif()