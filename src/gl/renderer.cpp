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
        init_shadow_map();

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
        _depthShader = new Shader("../shaders/depth.vert.spv", "../shaders/depth.frag.spv",
                                  "../shaders/depth.geom.spv");
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

    void Renderer::init_shadow_map() {
        // create framebuffer and cubemap
        glGenFramebuffers(1, &depthMapFBO);
        glGenTextures(1, &depthCubemap);

        // create textures for cubemap
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        for (unsigned int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, (GLsizei) SHADOW_MAP_RES,
                         (GLsizei) SHADOW_MAP_RES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // attach cubemap to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        ImGui::DragFloat3("Light Position", _lightPos, 1.0f, 0.0f, 0.0f, "%.1f");
        ImGui::ColorEdit3("Light Color", _lightColor);
        ImGui::DragFloat("Gamma", &_gamma, 0.1f, 0.0f, 10.0f, "%.1f");
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

        draw_shadow_map();
        draw_scene();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Renderer::draw_shadow_map() {
        // set viewport to map size and clear buffers
        glViewport(0, 0, (GLsizei) SHADOW_MAP_RES, (GLsizei) SHADOW_MAP_RES);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // create projection
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float) SHADOW_MAP_RES / (float) SHADOW_MAP_RES,
                                                _shadowNear, _shadowFar);

        // convert light pos to glm vec3 for cleanliness
        glm::vec3 glmLightPos = glm::vec3(_lightPos[0], _lightPos[1], _lightPos[2]);

        // generate transform matrices for each part of cubemap
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(glmLightPos, glmLightPos + glm::vec3(1.0f, 0.0f, 0.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(glmLightPos, glmLightPos + glm::vec3(-1.0f, 0.0f, 0.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(glmLightPos, glmLightPos + glm::vec3(0.0f, 1.0f, 0.0f),
                                                            glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(glmLightPos, glmLightPos + glm::vec3(0.0f, -1.0f, 0.0f),
                                                            glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(glmLightPos, glmLightPos + glm::vec3(0.0f, 0.0f, 1.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(glmLightPos, glmLightPos + glm::vec3(0.0f, 0.0f, -1.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));

        // clear framebuffer's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // send matrices and uniforms to depth shader
        _depthShader->bind();
        for (unsigned int i = 0; i < 6; i++) {
            _depthShader->set_mat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        }
        _depthShader->set_float("far_plane", _shadowFar);
        _depthShader->set_float_vec3("lightPos", _lightPos[0], _lightPos[1], _lightPos[2]);

        // draw shadows
        glCullFace(GL_FRONT);
        for (auto &it: _modelManager->models) {
            it.second.update_transform();
            it.second.draw_model_untextured(_depthShader);
        }

        // unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::draw_scene() {
        // set viewport to window size and clear buffers
        glViewport(0, 0, (GLsizei) _windowWidth, (GLsizei) _windowHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set uniforms for each model and draw
        glCullFace(GL_BACK);
        for (auto &it: _modelManager->models) {
            it.second._modelShader->bind();
            it.second._modelShader->set_mat4("matrix_viewproj", _flyCamera->projection * _flyCamera->get_view_matrix());
            it.second._modelShader->set_glm_vec3("camPos", _flyCamera->position);
            it.second._modelShader->set_float("lightRadius", _lightRadius);
            it.second._modelShader->set_float("lightPower", _lightPower);
            it.second._modelShader->set_float_vec3("lightColor", _lightColor[0], _lightColor[1], _lightColor[2]);
            it.second._modelShader->set_float("far_plane", _shadowFar);
            it.second._modelShader->set_float_vec3("lightPos", _lightPos[0], _lightPos[1], _lightPos[2]);
            it.second._modelShader->set_float("gamma", _gamma);
            it.second.update_transform();
            it.second.draw_model(depthCubemap);
        }
    }

    void Renderer::cleanup() {
    }
}