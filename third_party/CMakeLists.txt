add_library(sdl2 INTERFACE)
if(WIN32)
    if(NOT DEFINED ENV{SDL2_PATH})
        message(FATAL_ERROR "Please set the SDL2_PATH environment variable.")
    endif()

    set(sdl2_DIR $ENV{SDL2_PATH} CACHE PATH "Path to SDL2")
    target_include_directories(sdl2 INTERFACE ${sdl2_DIR}/include ${sdl2_image_DIR}/include)
    target_link_libraries(sdl2 INTERFACE ${sdl2_DIR}/lib/x64/SDL2.lib ${sdl2_DIR}/lib/x64/SDL2main.lib)
elseif(UNIX)
    find_package(SDL2 REQUIRED)
    target_include_directories(sdl2 INTERFACE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(sdl2 INTERFACE ${SDL2_LIBRARIES})
endif()

add_library(glad STATIC)
target_sources(glad PRIVATE glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

add_subdirectory(assimp)

add_library(imgui STATIC)
target_sources(imgui PRIVATE
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/backends/imgui_impl_sdl.cpp
        )
target_include_directories(imgui PUBLIC imgui PUBLIC imgui/backends)
target_link_libraries(imgui PUBLIC sdl2)

add_library(implot STATIC)
target_sources(implot PRIVATE
        implot/implot.cpp
        implot/implot_demo.cpp
        implot/implot_items.cpp)
target_include_directories(implot PUBLIC implot)
target_link_libraries(implot PUBLIC imgui)