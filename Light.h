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
vector<vector<Light *>> soft_lights;

vector<Light *> build_light(float r, Light *light_g, int steps) {
  float tmpX, tmpY, lastX, lastY;
  lastX = lastY = 0;
  vector<Light *> local_light;
  for (int angle = steps; angle < 90; angle += steps)//Bigger circle, Smaller angle step
  {
    tmpX = r * cos((float) angle * M_PI);
    tmpY = r * sin((float) angle * M_PI);
    if (tmpX == lastX && tmpY == lastY) {
      continue;
    }
    auto *l1 = new Light(light_g->position, light_g->color);
    l1->position.x += tmpX;
    l1->position.x += tmpY;
    local_light.push_back(l1);
    l1 = new Light(light_g->position, light_g->color);
    l1->position.x -= tmpX;
    l1->position.x += tmpY;
    local_light.push_back(l1);
    l1 = new Light(light_g->position, light_g->color);
    l1->position.x += tmpX;
    l1->position.x -= tmpY;
    local_light.push_back(l1);
    l1 = new Light(light_g->position, light_g->color);
    l1->position.x -= tmpX;
    l1->position.x -= tmpY;
    local_light.push_back(l1);
  }
  return local_light;
}

void position_lights() {
//  lights.push_back(new Light(glm::vec3(0, 26, 5), glm::vec3(1.0, 1.0, 1.0)));
//  lights.push_back(new Light(glm::vec3(0, 1, 12), glm::vec3(0.1)));
  lights.push_back(new Light(glm::vec3(0, 20, 23), glm::vec3(0.4)));
//  lights.push_back(new Light(glm::vec3(0, 3, -16), glm::vec3(0.6)));

//  auto light_g = new Light(glm::vec3(0, 25, 23), glm::vec3(0.2));
  auto light_g = new Light(glm::vec3(0, 5, 0), glm::vec3(0.3));
  float x = light_g->position.x;
  for (int i = 0; i < 1; i++) {
    light_g->position.x = x+(float)(i+1)/10.f;
    soft_lights.push_back(build_light(0.05f, light_g, 18));
    light_g->position.x = x-(float)(i-1)/10.f;
    soft_lights.push_back(build_light(0.05f, light_g, 18));
  }
//  light_g->color = glm::vec3(1.f);
//  soft_lights.push_back(build_light(2.f,light_g, 8));
//  soft_lights.push_back(build_light(3.f,light_g, 10));
}

#endif //USI_RENDERING_COMPETITION__LIGHT_H_
