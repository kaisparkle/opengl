# Add source to this project's executable.
add_executable(${CMAKE_PROJECT_NAME}
        main.cpp
        camera.cpp
        camera.h
        gl/check.cpp
        gl/check.h
        gl/shader.cpp
        gl/shader.h
        gl/vertex.h
        gl/texture.cpp
        gl/texture.h
        gl/mesh.cpp
        gl/mesh.h
        gl/model.cpp
        gl/model.h
        gl/renderer.cpp
        gl/renderer.h)

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>")

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${CMAKE_PROJECT_NAME} sdl2 glad glm stb assimp imgui implot ${CMAKE_DL_LIBS})

add_dependencies(${CMAKE_PROJECT_NAME} Shaders)