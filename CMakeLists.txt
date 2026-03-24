cmake_minimum_required(VERSION 3.21)
project(ProjectARIA LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 3.0.0
)

set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

FetchContent_MakeAvailable(SFML)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(ProjectARIA ${APP_SOURCES} "src/Utils/UI/CrashLogger.h")

target_link_libraries(ProjectARIA PRIVATE SFML::Graphics SFML::Window SFML::System)

# ADDED: dbghelp required for std::stacktrace on Windows
if(MSVC)
    target_link_libraries(ProjectARIA PRIVATE dbghelp)
endif()

target_compile_definitions(ProjectARIA PRIVATE 
    PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)

# ADDED: MSVC compiler flags for C++23, debug info, and hardware exception catching
if(MSVC)
    target_compile_options(ProjectARIA PRIVATE
        /std:c++23preview
        /Zi
        /EHa
        /Zc:__cplusplus     # ADDED: make __cplusplus report the actual standard version
    )
    target_link_options(ProjectARIA PRIVATE
        /DEBUG
    )
endif()

add_custom_command(TARGET ProjectARIA POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/media"
    "$<TARGET_FILE_DIR:ProjectARIA>/media"
)