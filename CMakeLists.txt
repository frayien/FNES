cmake_minimum_required(VERSION 3.21.0)
include(FetchContent)

project(FNES_emulator VERSION 0.1.0)

### CMake config ###
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  message(STATUS "GCC detected, adding compile flags")
  set(CMAKE_CXX_FLAGS "-Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "-Og -g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g1")
else(CMAKE_COMPILER_IS_GNUCXX)
  message(STATUS "MSVC detected, adding compile flags")
  set(CMAKE_CXX_FLAGS "/EHsc")
  set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2")
endif(CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

### Dependency declaration ###

FetchContent_Declare(
  sfml
  GIT_REPOSITORY "https://github.com/SFML/SFML.git"
  GIT_TAG        2.6.1
)

FetchContent_Declare(
  json
  GIT_REPOSITORY "https://github.com/nlohmann/json.git"
  GIT_TAG        v3.11.3
)

### Dependency population ###

set(BUILD_SHARED_LIBS OFF)

# sfml
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC OFF)

set(SFML_BUILD_NETWORK OFF)

set(SFML_BUILD_AUDIO ON)
set(SFML_BUILD_GRAPHICS ON)
set(SFML_BUILD_WINDOW ON)
# !sfml

FetchContent_MakeAvailable(sfml json)

if(1)

### Project headers ###
include_directories(include)

### Project sources ###
file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS LIST_DIRECTORIES OFF ./src/*)

else()

set(SRCS ${CMAKE_CURRENT_LIST_DIR}/test.cpp)

endif()

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(
  ${PROJECT_NAME}
  sfml-graphics
  sfml-window
  sfml-system
  sfml-audio
  nlohmann_json::nlohmann_json
)

if (WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
  )
endif()

if (WIN32 AND SFML_BUILD_AUDIO)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${FETCHCONTENT_BASE_DIR}/sfml-src/extlibs/bin/x64/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
  )
endif()