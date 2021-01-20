//
// Created by Vladimir Shubarin on 14.01.2021.
//

#ifndef CGLABS__DIFFUSE_LIGHT_HPP_
#define CGLABS__DIFFUSE_LIGHT_HPP_
#include <utility>

#include "functions.hpp"
#include "light_source.hpp"
class DiffuseLight : public LightSource {
  struct Data {
	glm::vec3 position{};
	glm::vec3 color{};
	float intensity{1};
  };
  Data data;

 public:
  [[nodiscard]] const Data& getData() const {
	return data;
  }

 public:
  explicit DiffuseLight(std::string _name, Data _data) : LightSource(lightType::DIFFUSE, std::move(_name)) {
	data = _data;
  }
  void passDataToShader(Shader* shader, int id) override {
	  shader->setUniform3f("lights[" + std::to_string(id) + "].position", data.position);
	  shader->setUniform3f("lights[" + std::to_string(id) + "].color", data.color * (bEnabled ? data.intensity : 0));
  }
  void passDataToShader(Shader* shader){
	shader->setUniform3f("light.position", data.position);
	shader->setUniform3f("light.color", data.color * (bEnabled ? data.intensity : 0));
  }

  void moveTo(glm::vec3 newCoords) override {
	data.position = newCoords;
  }
  void setIntensity(float _intensity) override {
	data.intensity = _intensity;
  }
  void setColor(glm::vec3 _color) override {
	data.color = _color;
  }
  [[nodiscard]] glm::vec3 getColor() const{
	return data.color;
  }

  [[nodiscard]] glm::vec3 getPosition() const{
	return data.position;
  }
  [[nodiscard]] float getIntensity() const{
	return data.intensity;
  }
};

#endif//CGLABS__DIFFUSE_LIGHT_HPP_
