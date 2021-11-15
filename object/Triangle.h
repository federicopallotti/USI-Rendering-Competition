#include "Object.h"

class Triangle : public Object
{
private:
    glm::vec3 v1;
    glm::vec3 v2;
    glm::vec3 v3;
    glm::vec3 normal;

public:
    Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) : v1(v1), v2(v2), v3(v3)
    {
        //compute plane passing by the 3 points
        //compute the normal to that plane
        this->normal = glm::normalize(glm::cross(v1 - v2, v3 - v2));
    }

    //compute ray plane intersection to find P
    Hit intersect(Ray ray) override
    {
        Hit hit{};
        hit.hit = false;


        float DdotN = glm::dot(ray.direction, this->normal);
        if (DdotN >= 0)
            return hit;
        float PdotN = glm::dot(v2 - ray.origin, this->normal);
        float t = PdotN / DdotN;
        if (t<0.1)
            return hit;
        glm::vec3 p = t * ray.direction + ray.origin;
        //compute areas of the 3 triangles with dot prod
        float a1 = glm::length(glm::cross(v1 - p, v2 - p))/2.f;
        float a2 = glm::length(glm::cross(v2 - p, v3 - p))/2.f;
        float a3 = glm::length(glm::cross(v1 - p, v3 - p))/2.f;
        float a = glm::length(glm::cross(v2 - v1, v3 - v1))/2.f;

        //compute lambdas

        float lambda1 = a1/a;
        float lambda2 = a2/a;
        float lambda3 = a3/a;


        //check if all lambdas are positive

        if (lambda1 > 0 && lambda2 > 0 && lambda3 > 0 && (lambda1+lambda2+lambda3)<=1.001 && (lambda1+lambda2+lambda3)>=0.99){
            //set intersection to true
            hit.hit = true;
            hit.object = this;
            hit.intersection = p;
            hit.distance = t;
            hit.normal = normal;
            return hit;
        }
        


        return hit;

        
    }
};