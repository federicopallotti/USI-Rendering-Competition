//
// Created by Volodymyr Karpenko on 12.11.21.
//

#ifndef USI_RENDERING_COMPETITION__LIGHT_H_
#define USI_RENDERING_COMPETITION__LIGHT_H_

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

class Light {
 public:
  glm::vec3 position; ///< Position of the light source
  glm::vec3 color{}; ///< Color/intentisty of the light source
  explicit Light(glm::vec3 position) : position(position) {
    color = glm::vec3(1.0);
  }

  Light(glm::vec3 position, glm::vec3 color) : position(position), color(color) {
  }
};

/**
 Function performing tonemapping of the intensities computed using the raytracer
 @param intensity Input intensity
 @return Tonemapped intensity in range (0,1)
 */
glm::vec3 toneMapping(glm::vec3 intensity) {
  float gamma = 1.0 / 2.0;
  float alpha = 12.0f;
  return glm::clamp(alpha * glm::pow(intensity, glm::vec3(gamma)), glm::vec3(0.0), glm::vec3(1.0));
}

vector<Light *> lights; ///< A list of lights in the scene
glm::vec3 ambient_light(0.001, 0.001, 0.001);


void position_lights() {
  lights.push_back(new Light(glm::vec3(0, 26, 5), glm::vec3(1.0, 1.0, 1.0)));
  lights.push_back(new Light(glm::vec3(0, 1, 12), glm::vec3(0.1)));
  lights.push_back(new Light(glm::vec3(0, 5, 1), glm::vec3(0.4)));
  lights.push_back(new Light(glm::vec3(0, 3, -11), glm::vec3(0.1)));
}

#endif //USI_RENDERING_COMPETITION__LIGHT_H_
