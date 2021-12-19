//
//  Textures.h
//  Raytracer
//
//  Created by Piotr Didyk on 19.08.21.
//

#ifndef Textures_h
#define Textures_h

#include "glm/glm.hpp"
#include "PerlinNoise.h"

PerlinNoise pn;
PerlinNoise pn1(2016, 1024);
PerlinNoise pn2(310, 2048);
PerlinNoise pn3(120412, 4096);

glm::vec3 checkerboardTexture(glm::vec2 uv) {
  float n = 20;
  float value = int(floor(n * uv.s) + floor(2 * n * uv.t)) % 2;
  return glm::vec3(value);
}
glm::vec3 rainbowTexture(glm::vec2 uv) {
  float n = 40;
  int value = int(floor(n * uv.t + 0.5 * n * uv.s)) % 3;
  switch (value) {
    case 0: return {1.0, 0.0, 0.0};
      break;
    case 1: return {0.0, 1.0, 0.0};
      break;
    default: return {0.0, 0.0, 1.0};
  }
}

glm::vec3 perlinNoise(glm::vec2 uv) {
  float n1 = (float) (pn.noise(uv.x, uv.y, 1.0) +
      pn1.noise(uv.x, uv.y, 1.0) +
      pn2.noise(uv.x, uv.y, 1.0) +
      pn3.noise(uv.x, uv.y, 1.0)) / 4.f;
  float n2 = (float) (pn.noise(uv.x, uv.y, 1.0) +
      pn1.noise(uv.x, uv.y, 1.0)) / 4.f;
  float n3 = (float) (pn.noise(uv.x, uv.y, 1.0)) / 4.f;
  return glm::normalize(glm::vec3((float) pn3.noise(uv.x, uv.y, 1.0)));
}

#endif /* Textures_h */
