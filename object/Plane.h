//
// Created by Volodymyr Karpenko on 12.11.21.
//

#ifndef USI_RENDERING_COMPETITION_OBJECT_PLANE_H_
#define USI_RENDERING_COMPETITION_OBJECT_PLANE_H_
#include "Object.h"

class Plane : public Object {

 private:
  glm::vec3 normal;
  glm::vec3 point;

 public:
  Plane(glm::vec3 point, glm::vec3 normal) : point(point), normal(normal) {
  }

  Plane(glm::vec3 point, glm::vec3 normal, Material material) : point(point), normal(normal) {
    this->material = material;
  }

  Hit intersect(Ray ray) override {

    Hit hit{};
    hit.hit = false;
    float DdotN = glm::dot(ray.direction, normal);
    if (DdotN < 0) {

      float PdotN = glm::dot(point - ray.origin, normal);
      float t = PdotN / DdotN;

      if (t > 0) {
        hit.hit = true;
        hit.normal = normal;
        hit.distance = t;
        hit.object = this;
        hit.intersection = t * ray.direction + ray.origin;
      }
    }
    return hit;
  }
};
#endif //USI_RENDERING_COMPETITION_OBJECT_PLANE_H_
