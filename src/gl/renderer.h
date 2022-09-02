#pragma once

#include <string>
#include <glad/glad.h>
#include <imgui.h>
#include <gl/shader.h>
#include <gl/model.h>
#include <camera.h>

namespace GLRenderer {
    struct ScrollingBuffer {
        int MaxSize;
        int Offset;
        ImVector<ImVec2> Data;

        ScrollingBuffer(int max_size = 10000) {
            MaxSize = max_size;
            Offset = 0;
            Data.reserve(MaxSize);
        }

        void AddPoint(float x, float y) {
            if (Data.size() < MaxSize)
                Data.push_back(ImVec2(x, y));
            else {
                Data[Offset] = ImVec2(x, y);
                Offset = (Offset + 1) % MaxSize;
            }
        }

        void Erase() {
            if (Data.size() > 0) {
                Data.shrink(0);
                Offset = 0;
            }
        }
    };

    class Renderer {
    public:
        void init(FlyCamera *camera, uint32_t windowWidth, uint32_t windowHeight);

        void update_window_size(uint32_t windowWidth, uint32_t windowHeight);

        void draw(double delta);

        void cleanup();

        bool isInitialized = false;

    private:
        double _delta = 0;
        uint32_t _windowWidth = 0;
        uint32_t _windowHeight = 0;
        float _lightColor[3] = {1.0f, 1.0f, 1.0f};
        float _lightPower = 64.0f;
        float _lightRadius = 64.0f;
        Shader *_pbrShader = nullptr;
        ModelManager *_modelManager = nullptr;
        FlyCamera *_flyCamera = nullptr;


        void init_shaders();

        void init_scene();

        void update_ui();
    };
}