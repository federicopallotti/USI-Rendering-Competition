//
//  Material.h
//  Raytracer
//
//  Created by Piotr Didyk on 14.07.21.
//

#ifndef Material_h
#define Material_h

#include "glm/glm.hpp"
#include "Textures.h"

/**
 Structure describing a material of an object
 */
struct Material {
  glm::vec3 ambient = glm::vec3(0.0);
  glm::vec3 diffuse = glm::vec3(1.0);
  glm::vec3 specular = glm::vec3(0.0);
  float shininess = 0.0;
  glm::vec3 (*texture)(glm::vec2 uv) = nullptr;
  float reflectivity = 0.0;
  bool refract = false;
  float refractiveIndex = 1.f;
};

Material yellow_specular{
    glm::vec3(0.1f, 0.10f, 0.0f),
    glm::vec3(0.4f, 0.4f, 0.0f),
    glm::vec3(1.0),
    100.0
};

Material green_diffuse{
    glm::vec3(0.03f, 0.1f, 0.03f),
    glm::vec3(0.3f, 1.0f, 0.3f)
};

Material red_specular{
    glm::vec3(0.01f, 0.02f, 0.02f),
    glm::vec3(1.0f, 0.2f, 0.2f),
    glm::vec3(0.5),
    10.0
};

Material blue_specular{
    glm::vec3(0.02f, 0.02f, 0.1f),
    glm::vec3(0.1f, 0.1f, 0.1f),
    glm::vec3(0.1),
    100.0,
    nullptr,
    0.5f
};

Material refractive{
    glm::vec3(0.02f, 0.02f, 0.1f),
    glm::vec3(0.1f, 0.1f, 0.1f),
    glm::vec3(0.1),
    100.0,
    nullptr,
    0.25f,
    true,
    2.0f
};

Material refractive_light{
    glm::vec3(0.02f, 0.02f, 0.1f),
    glm::vec3(0.1f, 0.1f, 0.1f),
    glm::vec3(0.1),
    30.0,
    nullptr,
    0.5f,
    true,
    5.0f
};

Material perlinTexture{
    glm::vec3(0.0),
    glm::vec3(0.0),
    glm::vec3(0.0),
    0.0f,
    &perlinNoise
};

Material textured{
    glm::vec3(0.0),
    glm::vec3(0.0),
    glm::vec3(0.0),
    0.0f,
    &rainbowTexture
};

Material red_diffuse{
    glm::vec3(0.09f, 0.06f, 0.06f),
    glm::vec3(0.9f, 0.6f, 0.6f)
};

Material blue_diffuse{
    glm::vec3(0.06f, 0.06f, 0.09f),
    glm::vec3(0.6f, 0.6f, 0.9f)
};

Material white_diffuse{
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(0.1f, 0.1f, 0.1f)
};

#endif /* Material_h */
