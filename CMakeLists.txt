cmake_minimum_required(VERSION 4.2.1)

project(DaktLibDecrypt VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB_RECURSE DAKT_SOURCES CONFIGURE_DEPENDS
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_library(daktdecrypt STATIC ${DAKT_SOURCES})

target_include_directories(daktdecrypt
	PUBLIC
		"${PROJECT_SOURCE_DIR}/include"
)

if(MSVC)
	target_compile_options(daktdecrypt PRIVATE /W4 /permissive- /Zc:__cplusplus)
else()
	target_compile_options(daktdecrypt PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	target_compile_definitions(daktdecrypt PRIVATE DAKT_PLATFORM_MACOS)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_compile_definitions(daktdecrypt PRIVATE DAKT_PLATFORM_LINUX)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	target_compile_definitions(daktdecrypt PRIVATE DAKT_PLATFORM_WINDOWS)
endif()
