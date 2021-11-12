//
// Created by Volodymyr Karpenko on 12.11.21.
//

#ifndef TEMPLATE_OBJECT_OBJECT_H_
#define TEMPLATE_OBJECT_OBJECT_H_

#include "../glm/glm.hpp"
#include "../glm/gtx/transform.hpp"
#include "../Material.h"
#include "../Ray.h"

class Object;

struct Hit {
  bool hit; ///< Boolean indicating whether there was or there was no intersection with an object
  glm::vec3 normal; ///< Normal vector of the intersected object at the intersection point
  glm::vec3 intersection; ///< Point of Intersection
  float distance; ///< Distance from the origin of the ray to the intersection point
  Object *object; ///< A pointer to the intersected object
  glm::vec2 uv; ///< Coordinates for computing the texture (texture coordinates)
};

class Object {
 protected:
  glm::mat4 transformationMatrix;
  glm::mat4 inverseTransformationMatrix;
  glm::mat4 normalMatrix;
 public:
  glm::vec3 color;
  Material material;
  virtual Hit intersect(Ray ray) = 0;

  void setMaterial(Material material){
    this->material = material;
  }
  Material getMaterial() const{
    return material;
  };
  void setTransformation(glm::mat4 matrix){
    transformationMatrix = matrix;
    inverseTransformationMatrix = glm::inverse(matrix);
    normalMatrix = glm::transpose(inverseTransformationMatrix);
  };
};

#endif //TEMPLATE_OBJECT_OBJECT_H_
