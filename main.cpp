/**
@file main.cpp
*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "thread-pool/thread_pool.hpp"

#include "object/Sphere.h"
#include "object/Plane.h"
#include "object/Cone.h"
#include "object/Triangle.h"
#include "Image.h"
#include "Ray.h"
#include "Light.h"
#include "kdtree.h"
#include <math.h>
#include <random>

using std::chrono::system_clock;

using namespace std;

mutex mtx;

vector<Object *> objects; ///< A list of all objects in the scene


/** Function for computing color of an object according to the Phong Model
 @param point A point belonging to the object for which the color is computer
 @param normal A normal vector the the point
 @param uv Texture coordinates
 @param view_direction A normalized direction from the point to the viewer/camera
 @param material A material structure representing the material of the object
*/
glm::vec3 PhongModel(glm::vec3 point, glm::vec3 normal, glm::vec2 uv, glm::vec3 view_direction, Material material) {

  glm::vec3 color(0.0);
  for (auto &light: lights) {

    glm::vec3 light_direction = glm::normalize(light->position - point);
    glm::vec3 reflected_direction = glm::reflect(-light_direction, normal);

    float NdotL = glm::clamp(glm::dot(normal, light_direction), 0.0f, 1.0f);
    float VdotR = glm::clamp(glm::dot(view_direction, reflected_direction), 0.0f, 1.0f);

    glm::vec3 diffuse_color = material.diffuse;
    if (material.texture) diffuse_color = material.texture(uv);

    glm::vec3 diffuse = diffuse_color * glm::vec3(NdotL);
    glm::vec3 specular = material.specular * glm::vec3(pow(VdotR, material.shininess));


    // distance to the light
    float r = glm::distance(point, light->position);
    r = max(r, 0.1f);

    float sj = 1.f;
    Ray ray = Ray(point, light_direction);
    for (auto &object: objects) {
      Hit hit = object->intersect(ray);
      if (hit.hit && hit.distance < glm::distance(light->position, point) &&
          hit.distance > 0.003) {
        sj = 0.f;
        break;
      }
    }
    if (sj != 0.f)
      color += light->color * (diffuse + specular) / r / r;
  }
  color += ambient_light * material.ambient;
  color = glm::clamp(color, glm::vec3(0.0), glm::vec3(1.0));
  return color;
}

/**
 Functions that computes a color along the ray
 @param ray Ray that should be traced through the scene
 @return Color at the intersection point
 */
glm::vec3 trace_ray(Ray ray, int depth, bool outside) {

  Hit closest_hit{};
  closest_hit.hit = false;
  closest_hit.distance = INFINITY;

  for (auto &object: objects) {
    Hit hit = object->intersect(ray);
    if (hit.hit && hit.distance < closest_hit.distance) closest_hit = hit;
  }

  glm::vec3 color(0.0);
  glm::vec3 reflect_color(0.0f);
  glm::vec3 refract_color(0.0f);

  if (depth > 0 && closest_hit.hit) {

    if (closest_hit.object->getMaterial().refract) {
      float Fr;
      float Ft;
      float delta1 = 1.0f;
      float delta2 = closest_hit.object->getMaterial().refractiveIndex;


      float beta = 1.0f / closest_hit.object->getMaterial().refractiveIndex;
      glm::vec3 refraction_direction = glm::normalize(glm::refract(ray.direction, closest_hit.normal, beta));
      if (!outside) {
        beta = 1.0f / beta;
        refraction_direction = glm::normalize(glm::refract(ray.direction, -closest_hit.normal, beta));
        delta1 = closest_hit.object->getMaterial().refractiveIndex;
        delta2 = 1.0f;
      }
      float cos_theta1 = glm::dot(-ray.direction, closest_hit.normal);
      float cos_theta2 = glm::dot(refraction_direction, -closest_hit.normal);

      float part1 = (delta1 * cos_theta1 - delta2 * cos_theta2) / (delta1 * cos_theta1 + delta2 * cos_theta2);
      float part2 = (delta1 * cos_theta2 - delta2 * cos_theta1) / (delta1 * cos_theta2 + delta2 * cos_theta1);
      Fr = (float) ((1.0f / 2.0f) * (glm::pow(part1, 2) + glm::pow(part2, 2)));
      Ft = 1.0f - Fr;

      Ray refractRay(closest_hit.intersection, refraction_direction);
      refract_color = glm::clamp(
          Ft * trace_ray(refractRay, depth - 1, !outside),
          glm::vec3(0.0f), glm::vec3(1.0f));
    }

    if (closest_hit.object->getMaterial().reflectivity != 0.f) {
      float reflection_alpha = closest_hit.object->getMaterial().reflectivity;
      glm::vec3 reflection_direction = glm::reflect(ray.direction, closest_hit.normal);
      Ray reflect_ray(closest_hit.intersection, reflection_direction);
      reflect_color = glm::clamp(
          reflection_alpha * trace_ray(reflect_ray, depth - 1, true),
          glm::vec3(0.0f), glm::vec3(1.0f));
    }


    color = glm::clamp(PhongModel(closest_hit.intersection, closest_hit.normal, closest_hit.uv,
                                  glm::normalize(-ray.direction), closest_hit.object->getMaterial()),
                       glm::vec3(0.0f), glm::vec3(1.0f));
  } else {
    color = glm::vec3(0.0, 0.0, 0.0);
  }

  return color + reflect_color + refract_color;
}

/**
 Function defining the scene
 */
void sceneDefinition() {

  objects.push_back(new Sphere(1.0, glm::vec3(1, -2, 8), blue_specular));
  objects.push_back(new Sphere(0.5, glm::vec3(-1, -2.5, 6), red_specular));
  //objects.push_back(new Sphere(1.0, glm::vec3(3,-2,6), green_diffuse));

  objects.push_back(new Sphere(2.0, glm::vec3(-3, -1, 8), refractive));

  //Textured sphere
  objects.push_back(new Sphere(7.0, glm::vec3(-6, 4, 23), textured));

  // Cone

  Cone *cone = new Cone(yellow_specular);
  glm::mat4 translationMatrix = glm::translate(glm::vec3(5, 9, 14));
  glm::mat4 scalingMatrix = glm::scale(glm::vec3(3.0f, 12.0f, 3.0f));
  glm::mat4 rotationMatrix = glm::rotate(glm::radians(180.0f), glm::vec3(1, 0, 0));
  cone->setTransformation(translationMatrix * scalingMatrix * rotationMatrix);
  objects.push_back(cone);

  Cone *cone2 = new Cone(green_diffuse);
  translationMatrix = glm::translate(glm::vec3(6, -3, 7));
  scalingMatrix = glm::scale(glm::vec3(1.0f, 3.0f, 1.0f));
  rotationMatrix = glm::rotate(glm::atan(3.0f), glm::vec3(0, 0, 1));
  cone2->setTransformation(translationMatrix * rotationMatrix * scalingMatrix);
  objects.push_back(cone2);

  //Triangle
  objects.push_back(new Triangle(glm::vec3(0.0f, 0.0f, 6.0f),
                                 glm::vec3(5.0f, 5.0f, 6.0f),
                                 glm::vec3(5.0f, 0.0f, 6.0f)));
}

void planes() {
  objects.push_back(new Plane(glm::vec3(0, -3, 0), glm::vec3(0.0, 1, 0)));
  objects.push_back(new Plane(glm::vec3(0, 1, 30), glm::vec3(0.0, 0.0, -1.0), green_diffuse));
  objects.push_back(new Plane(glm::vec3(-15, 1, 0), glm::vec3(1.0, 0.0, 0.0), red_diffuse));
  objects.push_back(new Plane(glm::vec3(15, 1, 0), glm::vec3(-1.0, 0.0, 0.0), blue_diffuse));
  objects.push_back(new Plane(glm::vec3(0, 27, 0), glm::vec3(0.0, -1, 0)));
  objects.push_back(new Plane(glm::vec3(0, 1, -0.01), glm::vec3(0.0, 0.0, 1.0), green_diffuse));
}

void parse_to_triangles(const vector<point> &points) {
  glm::mat4 translationMatrix = glm::translate(glm::vec3(0, 1.3, 3));
  for (int i = 0; i < points.size(); i += 3) {
    auto *triangle = new Triangle(points[i], points[i + 1], points[i + 2]);
    triangle->setTransformation(translationMatrix);
    objects.push_back(triangle);
  }
}

void threading_test(int start, int end, int height, float X, float Y, float s, Image image) {
    for (int i = start; i < end; i++)
        for (int j = 0; j < height; j++) {

            float dx = X + (float) i * s + s / 2;
            float dy = Y - (float) j * s - s / 2;
            float dz = 1;

            glm::vec3 origin(0, 0, 0);
            glm::vec3 direction(dx, dy, dz);
            direction = glm::normalize(direction);
            Ray ray(origin, direction);

            //code for DOF effect

            //DOF parameters
            float f = 8.0; //focal dist
            float r = 0.3f; //aperture
            float n = 30.f; //num of samples
            glm::vec3 focal_p = f * ray.direction / ray.direction.z;
            glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

            for (int i = 0; i < n; i++) {
                float offset_x = r * (((float)(rand()%RAND_MAX))/(float)RAND_MAX)*2.f-1.f;
                float offset_y = r * (((float)(rand()%RAND_MAX))/(float)RAND_MAX)*2.f-1.f;
                glm::vec3 new_o = ray.origin + glm::vec3(offset_x, offset_y, 0.0);
                glm::vec3 new_d = glm::normalize(focal_p - new_o);
                Ray new_ray(new_o, new_d);
                color += toneMapping(trace_ray(new_ray, 3, true));
            }
            glm::vec3 res = color/n;
            image.setPixel(i, j, res);
        }
}

int main(int argc, const char *argv[]) {
  clock_t t = clock(); // variable for keeping the time of the rendering

  int width = 2048; //width of the image
  int height = 1536; // height of the image
//  int width = 1024; //width of the image
//  int height = 768; // height of the image
//  int width = 512; //width of the image
//  int height = 384; // height of the image

  float fov = 90; // field of view
  if (argc == 2) {
    vector<point> points = parseFile(argv[1]);
    parse_to_triangles(points);
  }

  t = clock() - t;
  cout << "It took " << ((float) t) / CLOCKS_PER_SEC << " seconds to load the mesh." << endl;
  time_t timet = system_clock::to_time_t(system_clock::now());
  struct tm *time = localtime(&timet);
  cout << "Current time: " << put_time(time, "%X") << '\n';
  sceneDefinition(); // Let's define a scene
  planes();
  position_lights();

  Image image(width, height); // Create an image where we will store the result

  auto s = (float) (2 * tan(0.5 * fov / 180 * M_PI) / width);
  auto X = (float) (-s * (float) width / 2.0);
  auto Y = (float) (s * (float) height / 2.0);
  uint n = thread::hardware_concurrency()*2;
  uint slice = floor(width / n);
  int x = 0;
  thread_pool pool;
  for (uint i = 0; i < n; i++, x++) {
    pool.push_task(threading_test, slice * x, slice * (x + 1), height, X, Y, s, ref(image));
  }
  pool.push_task(threading_test, slice * x, width, height, X, Y, s, ref(image));
  pool.wait_for_tasks();
//    random_device rd;
//    mt19937 gen(rd());
//    uniform_int_distribution<float> uniform_dist(0.2,2.2);
//    float mean = uniform_dist(rd);
//    normal_distribution<> normal_dist(mean,2);
//    for (int i = 0; i < width; i++)
//        for (int j = 0; j < height; j++) {
//
//            float dx = X + (float) i * s + s / 2;
//            float dy = Y - (float) j * s - s / 2;
//            float dz = 1;
//
//            glm::vec3 origin(0, 0, 0);
//            glm::vec3 direction(dx, dy, dz);
//            direction = glm::normalize(direction);
//            Ray ray(origin, direction);
//
//            //code for DOF effect
//
//            //DOF parameters
//            float f = 3.0; //focal dist
//            float r = 0.2; //aperture
//            float n = 4.f; //num of samples
//            glm::vec3 focal_p = f * ray.direction / ray.direction.z;
//            glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
//
//            for (int i = 0; i < n; i++) {
//                r = r * sqrt(random());
//                //cout << "r value:" << r <<endl;
//                float theta = random() * 2 * M_PI;
//                float offset_x = r * cos(theta);
//                //cout << "x value:" << offset_x<< endl;
//                float offset_y = r * sin(theta);
//                //cout << "y value:" << offset_y<< endl;
//                glm::vec3 new_o = ray.origin + glm::vec3(offset_x, offset_y, 0.0);
//                glm::vec3 new_d = glm::normalize(focal_p - new_o);
////                cout << "origin x:" << new_o.x<< endl;
////                cout << "origin y" << new_o.y<< endl;
////                cout << "origin z:" << new_o.z<< endl;
//                cout << "dir x:" << new_d.x<< endl;
//                cout << "dir y:" << new_d.y<< endl;
//                cout << "dir z:" << new_d.z<< endl;
//                //cout << "direction:" << new_d<< endl;
//                Ray new_ray(new_o, new_d);
//                color += trace_ray(new_ray, 3, true);
//                //cout << "r value:" << color.r/n<< endl;
////                cout << "g value:" << color.g/n<< endl;
////                cout << "b value:" << color.b/n<< endl;
//                glm::vec3 res = toneMapping(color / n);
//                image.setPixel(i, j, res);
//            }
//        }


  timet = system_clock::to_time_t(system_clock::now());
  //convert it to tm struct
  time = std::localtime(&timet);
  cout << "Current time: " << put_time(time, "%X") << '\n';

  // Writing the final results of the rendering
  if (argc == 3) {
    image.writeImage(argv[2]);
  } else {
    image.writeImage("./result.ppm");
  }
//  test();
  return 0;
}
