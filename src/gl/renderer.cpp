#include "renderer.h"
#include <iostream>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <gl/check.h>

namespace GLRenderer {
    void Renderer::init(FlyCamera *camera, uint32_t windowWidth, uint32_t windowHeight) {
        _flyCamera = camera;
        _windowWidth = windowWidth;
        _windowHeight = windowHeight;
        _modelManager = new ModelManager;

        init_shaders();
        init_scene();

        isInitialized = true;
    }

    void Renderer::update_window_size(uint32_t windowWidth, uint32_t windowHeight) {
        // update the viewport
        _windowWidth = windowWidth;
        _windowHeight = windowHeight;
        glViewport(0, 0, (GLsizei) _windowWidth, (GLsizei) _windowHeight);
    }

    void Renderer::init_shaders() {
        _pbrShader = new Shader("../shaders/pbr.vert.spv", "../shaders/pbr.frag.spv");
    }

    void Renderer::init_scene() {
        _modelManager->create_model("../assets/sponza-gltf-pbr/sponza.glb", "sponza", _pbrShader);
        _modelManager->models["sponza"].scale[0] = 0.1f;
        _modelManager->models["sponza"].scale[1] = 0.1f;
        _modelManager->models["sponza"].scale[2] = 0.1f;
        _modelManager->create_model("../assets/SciFiHelmet.gltf", "helmet", _pbrShader);
        _modelManager->models["helmet"].translation[1] = 10.0f;
        _modelManager->models["helmet"].scale[0] = 3.0f;
        _modelManager->models["helmet"].scale[1] = 3.0f;
        _modelManager->models["helmet"].scale[2] = 3.0f;
    }

    void Renderer::update_ui() {
        // frametime plot
        static ScrollingBuffer sdata;
        static float t = 0;
        t += ImGui::GetIO().DeltaTime;
        sdata.AddPoint(t, (float) _delta);
        static float history = 5.0f;

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                       ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                                       ImGuiWindowFlags_NoNav;
        ImVec2 frametimeWindowPos = {0.0f + 10.0f, 0.0f + 10.0f};
        ImGui::SetNextWindowPos(frametimeWindowPos);
        ImGui::Begin("Frametime Plot", nullptr, windowFlags);
        ImGui::PushItemWidth(500);
        ImGui::SliderFloat("##History", &history, 1, 15, "%.1f s");

        if (ImPlot::BeginPlot("##Frametime Plot", ImVec2(500, 150))) {
            ImPlot::SetupAxes(nullptr, nullptr);
            ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 5);
            ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
            ImPlot::PlotLine("Frametime (ms)", &sdata.Data[0].x, &sdata.Data[0].y, sdata.Data.size(), 0, sdata.Offset,
                             2 * sizeof(float));
            ImPlot::EndPlot();
        }
        ImGui::End();

        // scene editor
        ImVec2 sceneWindowPos = {static_cast<float>(_windowWidth), 0.0f};
        ImVec2 sceneWindowPivot = {1.0f, 0.0f};
        ImVec2 sceneWindowSize = {-1, ImGui::GetIO().DisplaySize.y};
        ImGui::SetNextWindowPos(sceneWindowPos, 0, sceneWindowPivot);
        ImGui::SetNextWindowSize(sceneWindowSize);
        ImGui::Begin("Scene", nullptr);
        ImGui::DragFloat("Light Power", &_lightPower, 1.0f, 0.0f, 0.0f, "%.1f");
        ImGui::DragFloat("Light Radius", &_lightRadius, 1.0f, 0.0f, 0.0f, "%.1f");
        ImGui::ColorEdit3("Light Color", _lightColor);
        for (auto &it: _modelManager->models) {
            if (ImGui::TreeNode(it.first.c_str())) {
                ImGui::DragFloat3("Translation", it.second.translation, 1.0f, 0.0f, 0.0f, "%.1f");
                ImGui::DragFloat3("Rotation", it.second.rotation, 1.0f, -360.0f, 360.0f, "%.1f deg");
                ImGui::DragFloat3("Scale", it.second.scale, 1.0f, 0.0f, 0.0f, "%.1f");
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    void Renderer::draw(double delta) {
        _delta = delta;
        update_ui();
        ImGui::Render();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto &it: _modelManager->models) {
            it.second._modelShader->bind();
            it.second._modelShader->set_mat4("matrix_viewproj", _flyCamera->projection * _flyCamera->get_view_matrix());
            it.second._modelShader->set_vec3("camPos", _flyCamera->position);
            it.second._modelShader->set_float("lightRadius", _lightRadius);
            it.second._modelShader->set_float("lightPower", _lightPower);
            it.second._modelShader->set_vec3("lightColor", glm::vec3(_lightColor[0], _lightColor[1], _lightColor[2]));
            it.second.update_transform();
            it.second.draw_model();
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Renderer::cleanup() {
    }
}