/**
@file main.cpp
*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "object/Sphere.h"
#include "object/Plane.h"
#include "object/Cone.h"
#include "Image.h"
#include "Ray.h"

using namespace std;

/**
 Light class
 */
class Light {
public:
    glm::vec3 position; ///< Position of the light source
    glm::vec3 color {}; ///< Color/intentisty of the light source
    explicit Light(glm::vec3 position) : position(position) {
        color = glm::vec3(1.0);
    }

    Light(glm::vec3 position, glm::vec3 color) : position(position), color(color) {
    }
};

vector<Light *> lights; ///< A list of lights in the scene
glm::vec3 ambient_light(0.001, 0.001, 0.001);
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
    for (auto & light : lights){

        glm::vec3 light_direction = glm::normalize(light->position - point);
        glm::vec3 reflected_direction = glm::reflect(-light_direction, normal);

        float NdotL = glm::clamp(glm::dot(normal, light_direction), 0.0f, 1.0f);
        float VdotR = glm::clamp(glm::dot(view_direction, reflected_direction), 0.0f, 1.0f);

        glm::vec3 diffuse_color = material.diffuse;
        if (material.texture) {
            diffuse_color = material.texture(uv);
        }

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
//    }
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

    for (auto & object : objects)
    {
        Hit hit = object->intersect(ray);
        if (hit.hit && hit.distance < closest_hit.distance)
            closest_hit = hit;
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

    Material green_diffuse;
    green_diffuse.ambient = glm::vec3(0.03f, 0.1f, 0.03f);
    green_diffuse.diffuse = glm::vec3(0.3f, 1.0f, 0.3f);

    Material red_specular;
    red_specular.diffuse = glm::vec3(1.0f, 0.2f, 0.2f);
    red_specular.ambient = glm::vec3(0.01f, 0.02f, 0.02f);
    red_specular.specular = glm::vec3(0.5);
    red_specular.shininess = 10.0;

    Material blue_specular;
    blue_specular.ambient = glm::vec3(0.02f, 0.02f, 0.1f);
    blue_specular.diffuse = glm::vec3(0.1f, 0.1f, 0.1f);
    blue_specular.specular = glm::vec3(0.1);
    blue_specular.shininess = 100.0;
    blue_specular.reflectivity = 0.5f;

    objects.push_back(new Sphere(1.0, glm::vec3(1, -2, 8), blue_specular));
    objects.push_back(new Sphere(0.5, glm::vec3(-1, -2.5, 6), red_specular));
    //objects.push_back(new Sphere(1.0, glm::vec3(3,-2,6), green_diffuse));


    //Refractive sphere
    Material refractive;
    refractive.refract = true;
    refractive.refractiveIndex = 2.0f;
    refractive.ambient = glm::vec3(0.02f, 0.02f, 0.1f);
    refractive.diffuse = glm::vec3(0.1f, 0.1f, 0.1f);
    refractive.specular = glm::vec3(0.1);
    refractive.shininess = 100.0;
    refractive.reflectivity = 0.25f;
    objects.push_back(new Sphere(2.0, glm::vec3(-3, -1, 8), refractive));

    //Textured sphere

    Material textured;
    textured.texture = &rainbowTexture;
    objects.push_back(new Sphere(7.0, glm::vec3(-6, 4, 23), textured));


    //Planes
    Material red_diffuse;
    red_diffuse.ambient = glm::vec3(0.09f, 0.06f, 0.06f);
    red_diffuse.diffuse = glm::vec3(0.9f, 0.6f, 0.6f);

    Material blue_diffuse;
    blue_diffuse.ambient = glm::vec3(0.06f, 0.06f, 0.09f);
    blue_diffuse.diffuse = glm::vec3(0.6f, 0.6f, 0.9f);
    objects.push_back(new Plane(glm::vec3(0, -3, 0), glm::vec3(0.0, 1, 0)));
    objects.push_back(new Plane(glm::vec3(0, 1, 30), glm::vec3(0.0, 0.0, -1.0), green_diffuse));
    objects.push_back(new Plane(glm::vec3(-15, 1, 0), glm::vec3(1.0, 0.0, 0.0), red_diffuse));
    objects.push_back(new Plane(glm::vec3(15, 1, 0), glm::vec3(-1.0, 0.0, 0.0), blue_diffuse));
    objects.push_back(new Plane(glm::vec3(0, 27, 0), glm::vec3(0.0, -1, 0)));
    objects.push_back(new Plane(glm::vec3(0, 1, -0.01), glm::vec3(0.0, 0.0, 1.0), green_diffuse));


    // Cone

    Material yellow_specular;
    yellow_specular.ambient = glm::vec3(0.1f, 0.10f, 0.0f);
    yellow_specular.diffuse = glm::vec3(0.4f, 0.4f, 0.0f);
    yellow_specular.specular = glm::vec3(1.0);
    yellow_specular.shininess = 100.0;

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

    lights.push_back(new Light(glm::vec3(0, 26, 5), glm::vec3(1.0, 1.0, 1.0)));
    lights.push_back(new Light(glm::vec3(0, 1, 12), glm::vec3(0.1)));
    lights.push_back(new Light(glm::vec3(0, 5, 1), glm::vec3(0.4)));
}

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

int main(int argc, const char *argv[]) {

    clock_t t = clock(); // variable for keeping the time of the rendering

    int width = 1024; //width of the image
    int height = 768; // height of the image
    float fov = 90; // field of view

    sceneDefinition(); // Let's define a scene

    Image image(width, height); // Create an image where we will store the result

    auto s = (float) (2 * tan(0.5 * fov / 180 * M_PI) / width);
    auto X = (float) (-s * (float) width / 2.0);
    auto Y = (float) (s * (float) height / 2.0);

    for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++) {

            float dx = X + (float) i * s + s / 2;
            float dy = Y - (float) j * s - s / 2;
            float dz = 1;

            glm::vec3 origin(0, 0, 0);
            glm::vec3 direction(dx, dy, dz);
            direction = glm::normalize(direction);

            Ray ray(origin, direction);

            image.setPixel(i, j, toneMapping(trace_ray(ray, 3, true)));

        }

    t = clock() - t;
    cout << "It took " << ((float) t) / CLOCKS_PER_SEC << " seconds to render the image." << endl;
    cout << "I could render at " << (float) CLOCKS_PER_SEC / ((float) t) << " frames per second." << endl;

    // Writing the final results of the rendering
    if (argc == 2) {
        image.writeImage(argv[2]);
    } else {
        image.writeImage("./result.ppm");
    }

    return 0;
}
