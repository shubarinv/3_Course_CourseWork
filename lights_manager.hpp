//
// Created by Vladimir Shubarin on 15.01.2021.
//

#ifndef CGLABS__LIGHTS_MANAGER_HPP_
#define CGLABS__LIGHTS_MANAGER_HPP_
#include <variant>
#include <vector>
class LightsManager {
  int numDirLights{0};
  int numPointLights{0};
  int numSpotLights{0};
  int numDiffLights{0};

  std::vector<LightSource> lights{};
  std::vector<DiffuseLight> diffLights{};

 public:
  void passDataToShader(Shader* shader) {
	shader->bind();
	int enabledLights{0};
	for (int i = 0; i < diffLights.size(); ++i) {
	  diffLights[i].passDataToShader(shader, i);
	}
	shader->setUniform1i("numDiffLights", diffLights.size());
  }

  DiffuseLight* getLightByNameDir(const std::string& name) {
	for (auto& light : diffLights) {
	  if (light.name == name) {
		return &light;
	  }
	}
	return nullptr;
  }

 public:
  void addLight(const DiffuseLight& light) {
	diffLights.push_back(light);
  }
  void addLight(const LightSource& light) {
	lights.push_back(light);
  }
};

#endif//CGLABS__LIGHTS_MANAGER_HPP_
