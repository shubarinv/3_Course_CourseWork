//
// Created by Vladimir Shubarin on 15.01.2021.
//

#ifndef CGLABS__LIGHTS_MANAGER_HPP_
#define CGLABS__LIGHTS_MANAGER_HPP_

#include <utility>
#include <variant>
#include <vector>
#include "shader.hpp"

class LightsManager {
public:
    struct DirectionalLight {
        DirectionalLight(std::string _name, glm::vec3 _direction, glm::vec3 _ambient, glm::vec3 _diffuse,
                         glm::vec3 _specular) {
            name = std::move(_name);
            direction = _direction;
            ambient = _ambient;
            diffuse = _diffuse;
            specular = _specular;
        }

        std::string name;
        glm::vec3 direction{}; // this is where light will look
        glm::vec3 ambient{};

        glm::vec3 diffuse{}; //colors ?
        glm::vec3 specular{}; //colors ?
    };

    struct PointLight {
        std::string name;
        glm::vec3 position;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };
    struct SpotLight {
        std::string name;
        glm::vec3 position;
        glm::vec3 direction;
        float cutOff;
    };
private:
    std::vector<PointLight> pointLights{};
    std::vector<DirectionalLight> dirLights{};
    std::vector<SpotLight> spotLights{};

public:
    void passDataToShader(Shader *shader) {
        shader->bind();
        shader->setUniform1i("NUM_POINT_LIGHTS", pointLights.size());
        shader->setUniform1i("NUM_SPOT_LIGHTS", spotLights.size());
        shader->setUniform1i("NUM_DIR_LIGHTS", dirLights.size());
        for (int i = 0; i < dirLights.size(); ++i) {
            shader->setUniform3f("dirLights["+std::to_string(i)+"].direction", dirLights[i].direction);
            shader->setUniform3f("dirLights["+std::to_string(i)+"].diffuse", dirLights[i].diffuse);
            shader->setUniform3f("dirLights["+std::to_string(i)+"].ambient", dirLights[i].ambient);
            shader->setUniform3f("dirLights["+std::to_string(i)+"].specular", dirLights[i].specular);
        }
    }

    DirectionalLight *getDirLightByName(std::string name) {

    }

    PointLight *getPointLightByName(std::string name) {}

    SpotLight *getSpotLightByName(std::string name) {}

public:
    void addLight(PointLight pointLight) {
        pointLights.push_back(pointLight);
    }

    void addLight(SpotLight spotLight) {
        spotLights.push_back(spotLight);
    }

    void addLight(DirectionalLight dirLight) {
        dirLights.push_back(dirLight);
    }
};

#endif//CGLABS__LIGHTS_MANAGER_HPP_
