cmake_minimum_required(VERSION 3.5)

project(Sokoban)

# Create an option to switch between a system sdl library and a vendored sdl library
option(USE_VENDORED_SDL "Use vendored libraries" OFF)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# prevent installing to system directories. 
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

if(USE_VENDORED_SDL)
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	
	#SDL_3
	set(SDL_STATIC_PIC ON CACHE BOOL "" FORCE)
	set(SDL_SHARED OFF)	
	add_subdirectory("${PROJECT_SOURCE_DIR}/Vendored/SDL3" EXCLUDE_FROM_ALL)
	
	# SDL_TTF
	set(SDLTTF_VENDORED ON) 
	add_subdirectory("${PROJECT_SOURCE_DIR}/Vendored/SDL_ttf" EXCLUDE_FROM_ALL)
	
	# SDL_mixer (used for playing audio)
	set(SDLMIXER_MIDI_NATIVE OFF)
	set(SDLMIXER_MIDI OFF)
	set(SDLMIXER_GME OFF)
	set(SDLMIXER_WAVPACK ON)
	set(SDLMIXER_MOD ON)
	set(SDLMIXER_MOD_XMP ON)
	set(SDLMIXER_MOD_XMP_SHARED OFF)
	set(SDLMIXER_MOD_XMP_LITE OFF)
	set(SDLMIXER_MOD_MODPLUG OFF)
	set(SDLMIXER_MOD_MODPLUG_SHARED OFF)
	set(SDLMIXER_OPUS OFF)
	set(SDLMIXER_SNDFILE OFF)
	set(SDLMIXER_FLAC OFF)
	set(SDLMIXER_MP3 OFF)
	set(SDLMIXER_VORBIS "STB")
	set(SDLMIXER_BUILD_SHARED_LIBS OFF)
	set(SDLMIXER_VENDORED ON)
	
	add_subdirectory("${PROJECT_SOURCE_DIR}/Vendored/SDL_mixer" EXCLUDE_FROM_ALL)
	
	# SDL_image (used for loading various image formats)
	set(SDLIMAGE_VENDORED ON)
	set(SDLIMAGE_AVIF OFF)
	set(SDLIMAGE_BMP OFF)
	set(SDLIMAGE_GIF OFF)
	set(SDLIMAGE_JPG OFF)
	set(SDLIMAGE_JXL OFF)
	set(SDLIMAGE_LBM OFF)
	set(SDLIMAGE_PCX OFF)
	set(SDLIMAGE_PNG ON)
	set(SDLIMAGE_PNM OFF)
	set(SDLIMAGE_QOI OFF)
	set(SDLIMAGE_SVG OFF)
	set(SDLIMAGE_TGA OFF)
	set(SDLIMAGE_TIF OFF)
	set(SDLIMAGE_WEBP OFF)
	set(SDLIMAGE_XCF OFF)
	set(SDLIMAGE_XPM OFF)
	set(SDLIMAGE_XV OFF)
	add_subdirectory("${PROJECT_SOURCE_DIR}/Vendored/SDL_image" EXCLUDE_FROM_ALL)
	
else()
	set(SDL_SHARED ON)
    # 1. Look for a SDL3 package, 2. look for the SDL3 component and 3. fail if none can be found
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
	find_package(SDL3_image REQUIRED CONFIG REQUIRED COMPONENTS SDL3_image)
	find_package(SDL3_mixer REQUIRED CONFIG REQUIRED COMPONENTS SDL3_mixer)
	find_package(SDL3_ttf REQUIRED CONFIG REQUIRED COMPONENTS SDL3_ttf)
endif()


file(GLOB SRC CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.h"
)

if (WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "-static -mwindows")
endif()

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${SRC} )

# Set C++ version
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# on Visual Studio, set our app as the default project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${EXECUTABLE_NAME}")

if(USE_VENDORED_SDL)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
		SDL3_ttf::SDL3_ttf-static
		SDL3_mixer::SDL3_mixer-static
		SDL3_image::SDL3_image-static
		SDL3::SDL3-static
    )

	# SDL_Image bug: https://github.com/libsdl-org/SDL_image/issues/506
	if (APPLE AND NOT BUILD_SHARED_LIBS)
		find_library(IO_LIB ImageIO REQUIRED)
		find_library(CS_LIB CoreServices REQUIRED)
		find_library(CT_LIB CoreText REQUIRED)
		find_library(CG_LIB CoreGraphics REQUIRED)
		find_library(CF_LIB CoreFoundation REQUIRED)
		target_link_libraries(${PROJECT_NAME} PUBLIC ${CF_LIB} ${CT_LIB} ${IO_LIB} ${CS_LIB} ${CG_LIB})
	endif()
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE 
		SDL3_ttf::SDL3_ttf
		SDL3_mixer::SDL3_mixer
		SDL3_image::SDL3_image
		SDL3::SDL3
    )
endif()