#include <SDL.h>
#include <iostream>
#include <chrono>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <gl/renderer.h>
#include <gl/check.h>

constexpr uint32_t DEFAULT_WINDOW_WIDTH = 1366;
constexpr uint32_t DEFAULT_WINDOW_HEIGHT = 768;
constexpr float DEFAULT_FOV_DEG = 110.0f;

int main(int argc, char *argv[]) {
    uint32_t windowWidth = DEFAULT_WINDOW_WIDTH;
    uint32_t windowHeight = DEFAULT_WINDOW_HEIGHT;

    // create window
    auto windowFlags = (SDL_WindowFlags) SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    SDL_Window *window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          (int) windowWidth, (int) windowHeight, windowFlags);

    // create OpenGL 4.6 context, debug, vsync
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetSwapInterval(1);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    // use relative mouse coordinates
    SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // get GL function pointers
    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // set up debug context if enabled
    int flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLRenderer::glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    // set the default viewport size
    glViewport(0, 0, (GLsizei) windowWidth, (GLsizei) windowHeight);

    // enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // set up imgui
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 460");

    // create camera
    auto camera = FlyCamera(DEFAULT_FOV_DEG, (float) windowWidth / (float) windowHeight, 0.1f, 2000.0f);

    // init renderer
    GLRenderer::Renderer renderer;
    renderer.init(&camera, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    if (!renderer.isInitialized) {
        std::cout << "Failed to initialize renderer" << std::endl;
        return -1;
    }

    double previousFrameTime = 0;
    SDL_Event e;
    bool quit = false;
    bool toggleUI = true;
    while (!quit) {
        auto frameTimerStart = std::chrono::high_resolution_clock::now();

        // disable relative mouse if using UI currently
        if (toggleUI) {
            SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "0", SDL_HINT_OVERRIDE);
            SDL_SetRelativeMouseMode(SDL_FALSE);
        } else {
            SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
            SDL_SetRelativeMouseMode(SDL_TRUE);
        }

        while (SDL_PollEvent(&e) != 0) {
            if (toggleUI) ImGui_ImplSDL2_ProcessEvent(&e);

            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_MOUSEMOTION && !toggleUI) {
                camera.process_mouse((float) e.motion.xrel, (float) e.motion.yrel);
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_TAB:
                        toggleUI = !toggleUI;
                }
            }
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
                renderer.update_window_size(e.window.data1, e.window.data2);
                camera.update_projection_matrix(DEFAULT_FOV_DEG, (float) windowWidth / (float) windowHeight,
                                                0.1f, 2000.0f);
            }
        }
        if (!toggleUI) {
            camera.process_keyboard(previousFrameTime, const_cast<uint8_t *>(SDL_GetKeyboardState(nullptr)));
        }

        // render frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        renderer.draw(previousFrameTime);

        SDL_GL_SwapWindow(window);

        auto frameTimerEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> frameDuration = frameTimerEnd - frameTimerStart;
        previousFrameTime = frameDuration.count();
    }

    renderer.cleanup();
    return 0;
}