cmake_minimum_required(VERSION 3.14.0)
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
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
else(CMAKE_COMPILER_IS_GNUCXX)
  message(STATUS "MSVC detected, adding compile flags")
  set(CMAKE_CXX_FLAGS "/EHsc")
  set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2")
endif(CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")

### Project headers ###
include_directories(include)

### Dependency declaration ###

FetchContent_Declare(
    sfml
    GIT_REPOSITORY "https://github.com/SFML/SFML.git"
    GIT_TAG        2.5.1
)

FetchContent_Declare(
    json
    GIT_REPOSITORY "https://github.com/nlohmann/json.git"
    GIT_TAG        v3.10.4
)


### Dependency population ###

# sfml
set(BUILD_SHARED_LIBS OFF)
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC OFF)

set(SFML_BUILD_NETWORK OFF)

set(SFML_BUILD_AUDIO ON)
set(SFML_BUILD_GRAPHICS ON)
set(SFML_BUILD_WINDOW ON)

FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
    FetchContent_Populate(sfml)
    add_subdirectory(${sfml_SOURCE_DIR})
endif()
# !sfml

# sfml audio cannot build since c++17 due to removed auto_ptr
set_target_properties(sfml-audio PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)

# nlohman json
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
# !nlohman json

### Project sources ###
file(GLOB_RECURSE SRCS ./src/*)

add_executable(FNES_emulator ${SRCS})

target_link_libraries(
    FNES_emulator
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    nlohmann_json::nlohmann_json
)