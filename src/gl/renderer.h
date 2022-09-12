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

        void draw_shadow_map();

        void draw_scene();

        void cleanup();

        bool isInitialized = false;

    private:
        double _delta = 0;

        uint32_t _windowWidth = 0;
        uint32_t _windowHeight = 0;

        float _lightColor[3] = {1.0f, 1.0f, 1.0f};
        float _lightPower = 8192.0f;
        float _lightRadius = 8192.0f;
        float _lightPos[3] = {0.0f, 50.0f, 0.0f};
        float _prevLightPos[3];
        float _gamma = 2.2f;

        Shader *_pbrShader = nullptr;
        Shader *_depthShader = nullptr;

        ModelManager *_modelManager = nullptr;
        FlyCamera *_flyCamera = nullptr;

        const unsigned int SHADOW_MAP_RES = 4096;
        unsigned int depthMapFBO = 0;
        unsigned int depthCubemap = 0;
        float _shadowNear = 0.1f;
        float _shadowFar = 2000.0f;
        float _shadowBias = 0.15f;

        void init_shaders();

        void init_scene();

        void init_shadow_map();

        void update_ui();
    };
}