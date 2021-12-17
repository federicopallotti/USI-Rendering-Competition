#include "Object.h"

class Triangle : public Object {
 protected:
  const float EPSILON = 0.0000001;
 public:
  glm::vec3 v1{};
  glm::vec3 v2{};
  glm::vec3 v3{};
  glm::vec3 normal{};
  glm::vec3 e1{};
  glm::vec3 e2{};
  Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) : v1(v1), v2(v2), v3(v3) {
    this->e1 = this->v2 - this->v1;
    this->e2 = this->v3 - this->v1;
    this->normal = glm::normalize(glm::cross(this->e1, this->e2));
  }

  Triangle(std::vector<float> v1, std::vector<float> v2, std::vector<float> v3) {
    this->v1 = glm::vec3(v1[0], v1[1], v1[2]);
    this->v2 = glm::vec3(v2[0], v2[1], v2[2]);
    this->v3 = glm::vec3(v3[0], v3[1], v3[2]);
    this->material = white_diffuse;
    this->e1 = this->v2 - this->v1;
    this->e2 = this->v3 - this->v1;
    this->normal = glm::normalize(glm::cross(this->e1, this->e2));
  }

  //compute ray plane intersection to find P
  Hit intersect(Ray ray) override {
    Hit hit{};
    hit.hit = false;

    glm::vec3 d = inverseTransformationMatrix * glm::vec4(ray.direction, 0.0);
    d = glm::normalize(d);
    glm::vec3 o = inverseTransformationMatrix * glm::vec4(ray.origin, 1.0);

    // Calculate determinant
    glm::vec3 p = glm::cross(d, e2);

    //Calculate determinat
    float det = glm::dot(e1, p);

    //if determinant is near zero, ray lies in plane of triangle otherwise not
    if (det > -EPSILON && det < EPSILON) { return hit; }
    float invDet = 1.0f / det;

    //calculate distance from p1 to ray origin
    glm::vec3 t = o - v1;

    //Calculate u parameter
    float u = glm::dot(t, p) * invDet;

    //Check for ray hit
    if (u < 0 || u > 1) { return hit; }

    //Prepare to test v parameter
    glm::vec3 q = glm::cross(t, e1);

    //Calculate v parameter
    float v = glm::dot(d, q) * invDet;

    //Check for ray hit
    if (v < 0 || u + v > 1) { return hit; }

    if ((glm::dot(e2, q) * invDet) > EPSILON) {
      //ray does intersect
      hit.intersection = transformationMatrix * glm::vec4(v1 + u*e1 + v*e2,1.0);
      hit.normal = normal;
      hit.distance = glm::dot(e2, q) * invDet;
      hit.object = this;
      hit.hit = true;
      return hit;
    }

    // No hit at all
    return hit;
  }
};