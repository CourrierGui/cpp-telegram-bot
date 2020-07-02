cmake_minimum_required(VERSION 3.13)
project(cppbot)

set(Boost_USE_MULTITHREADED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
find_package(yaml-cpp REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Curlpp REQUIRED IMPORTED_TARGET curlpp)

if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  add_definitions(-DHAVE_CURL)
endif()

add_subdirectory(extern/json)
add_subdirectory(extern/tgbot-cpp)

add_library(target-flags INTERFACE)
target_compile_options(target-flags
	INTERFACE -Wall -Wextra -Wpedantic
)
target_compile_features(target-flags INTERFACE cxx_std_17)

add_library(cppbot-utils STATIC
	src/bot.cpp
	src/buffer.cpp
	src/unsplash.cpp
	src/arg-parser.cpp
)
target_include_directories(cppbot-utils
	PUBLIC
		include
		extern/tgbot-cpp/include
	PRIVATE
		extern/json/include
)
target_link_libraries( cppbot-utils
	PUBLIC
		TgBot
	PRIVATE
		yaml-cpp
		nlohmann_json::nlohmann_json
		target-flags
)

add_executable(cppbot app/main.cpp)
target_link_libraries(cppbot PRIVATE target-flags cppbot-utils)

option(BUILD_TESTING "Build the tests" ON)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  enable_testing()
  include(GoogleTest)
endif()

add_subdirectory(tests)
