//
// Created by Volodymyr Karpenko on 12.11.21.
//

#ifndef USI_RENDERING_COMPETITION_OBJECT_SPHERE_H_
#define USI_RENDERING_COMPETITION_OBJECT_SPHERE_H_

#include "Object.h"

class Sphere : public Object {
 private:
  float radius; ///< Radius of the sphere
  glm::vec3 center; ///< Center of the sphere

 public:
  /**
   The constructor of the sphere
   @param radius Radius of the sphere
   @param center Center of the sphere
   @param color Color of the sphere
   */
  Sphere(float radius, glm::vec3 center, glm::vec3 color) : radius(radius), center(center) {
    this->color = color;
  }

  Sphere(float radius, glm::vec3 center, Material material) : radius(radius), center(center) {
    this->material = material;
  }

  /** Implementation of the intersection function*/
  Hit intersect(Ray ray) override {

    glm::vec3 c = center - ray.origin;

    float cdotc = glm::dot(c, c);
    float cdotd = glm::dot(c, ray.direction);

    Hit hit{};

    float D = 0;
    if (cdotc > cdotd * cdotd) {
      D = sqrt(cdotc - cdotd * cdotd);
    }
    if (D <= radius) {
      hit.hit = true;
      float t1 = cdotd - sqrt(radius * radius - D * D);
      float t2 = cdotd + sqrt(radius * radius - D * D);

      float t = t1;
      if (t < 0.1) t = t2;
      if (t < 0.1) {
        hit.hit = false;
        return hit;
      }

      hit.intersection = ray.origin + t * ray.direction;
      hit.normal = glm::normalize(hit.intersection - center);
      hit.distance = glm::distance(ray.origin, hit.intersection);
      hit.object = this;

      hit.uv.s = (float) ((asin(hit.normal.y) + M_PI / 2) / M_PI);
      hit.uv.t = (float)((atan2(hit.normal.z, hit.normal.x) + M_PI) / (2 * M_PI));
    } else {
      hit.hit = false;
    }
    return hit;
  }
};

#endif //USI_RENDERING_COMPETITION_OBJECT_SPHERE_H_
