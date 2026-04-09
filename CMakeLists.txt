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

FetchContent_Declare(ImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.91.1
)

FetchContent_Declare(ImGui-SFML
  GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
  GIT_TAG master
)

FetchContent_Declare(ImPlot
  GIT_REPOSITORY https://github.com/epezent/implot.git
  GIT_TAG master
)
FetchContent_MakeAvailable(ImPlot)

set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

FetchContent_MakeAvailable(SFML)
FetchContent_MakeAvailable(ImGui)

set(IMGUI_DIR ${imgui_SOURCE_DIR} CACHE PATH "" FORCE)

FetchContent_MakeAvailable(ImGui-SFML)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(ProjectARIA ${APP_SOURCES} "src/Utils/UI/CrashLogger.h" "src/simulation/imgui/imgui_handling.cpp" "src/Protozoa/genetics/CellGenome.h"  "src/Protozoa/extension/mutations.cpp" "src/Utils/Graphics/spatial_grid/spatial_grid_renderer.h" "src/Food/food.h" "src/Food/FoodManager_spawn.cpp" "src/simulation/imgui/imgui_population.cpp")

target_link_libraries(ProjectARIA PRIVATE
  SFML::Graphics
  SFML::Window
  SFML::System
  ImGui-SFML::ImGui-SFML
)

target_include_directories(ProjectARIA PRIVATE ${implot_SOURCE_DIR})
target_sources(ProjectARIA PRIVATE
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)

if(MSVC)
    target_link_libraries(ProjectARIA PRIVATE dbghelp)
endif()

target_compile_definitions(ProjectARIA PRIVATE 
    PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)

if(MSVC)
    target_compile_options(ProjectARIA PRIVATE
        /std:c++23preview
        /Zi
        /EHa
        /Zc:__cplusplus
    )
    target_link_options(ProjectARIA PRIVATE
        /DEBUG
    )
    # Release: keep symbols, enable optimisation
    target_compile_options(ProjectARIA PRIVATE
        $<$<CONFIG:Release>:/O2>
    )
    target_link_options(ProjectARIA PRIVATE
        $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
    )
endif()

add_custom_command(TARGET ProjectARIA POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/media"
    "$<TARGET_FILE_DIR:ProjectARIA>/media"
)