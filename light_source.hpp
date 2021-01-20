//
// Created by Vladimir Shubarin on 14.01.2021.
//

#ifndef CGLABS__LIGHT_SOURCE_HPP_
#define CGLABS__LIGHT_SOURCE_HPP_
#include <utility>

#include "shader.hpp"
class LightSource {
 public:
  enum lightType {
	DIFFUSE,
	AMBIENT,
	DIRECTIONAL,
	POINT,
	SPECULAR
  };

  void enable() { bEnabled=true; }
   void disable() {bEnabled=false; }
   lightType type;
  bool bEnabled=true;

  protected:
  explicit LightSource(lightType _type, std::string _name) {
   type = _type;
   name = std::move(_name);
 }
 public:
  virtual void setIntensity(float _intensity){}
  virtual void setColor(glm::vec3 color) {}
  virtual void moveTo(glm::vec3 newCoords) {}
  std::string name;
  virtual void passDataToShader(Shader* shader,int id) {}
};

#endif//CGLABS__LIGHT_SOURCE_HPP_
