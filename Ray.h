//
// Created by Volodymyr Karpenko on 12.11.21.
//

#ifndef TEMPLATE__RAY_H_
#define TEMPLATE__RAY_H_
/**
 Class representing a single ray.
 */
class Ray {
 public:
  glm::vec3 origin; ///< Origin of the ray
  glm::vec3 direction; ///< Direction of the ray
  glm::vec3 position;
  /**
   Contructor of the ray
   @param origin Origin of the ray
   @param direction Direction of the ray
   */
  Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {
    this->position = origin+position;
  }
};

#endif //TEMPLATE__RAY_H_
