//
// Created by Volodymyr Karpenko on 16.12.21.
//

#ifndef USI_RENDERING_COMPETITION_OBJECT_FIGURE_H_
#define USI_RENDERING_COMPETITION_OBJECT_FIGURE_H_
#include "Object.h"
#include "Triangle.h"
#include "PerlinNoise.h"
#include <utility>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

typedef vector<float> point;

class Node {
 public:
  int split_plane;
  Triangle *p;
  Node *left;
  Node *right;

  Node(int split_plane, Triangle *p, Node *left, Node *right)
      : split_plane(split_plane), p(p), left(left), right(right) {
  }
  ~Node() {
    delete p;
    delete left;
    delete right;
  }
};

class Figure : public Object {
 private:
  Node *tree;
  vector<Triangle *> parse_to_triangles(const vector<point> &points, bool flag) {
    PerlinNoise np;
    vector<Triangle *> triangles;
    if (flag) {
      glm::mat4 translationMatrix = glm::translate(glm::vec3(0, 0, 1));
      for (int i = 0; i < points.size(); i += 3) {
        point v1 = points[i];
        point v2 = points[i + 1];
        point v3 = points[i + 2];
        v1[1] += (float) np.noise(v1[0], v1[1], v1[2]);
        v2[1] += (float) np.noise(v2[0], v2[1], v2[2]);
        v3[1] += (float) np.noise(v3[0], v3[1], v3[2]);
        auto *triangle = new Triangle(v1, v2, v3);
        triangle->setTransformation(translationMatrix);
        triangle->setMaterial(blue_specular);
        triangles.push_back(triangle);
      }
    } else {
      glm::mat4 translationMatrix = glm::translate(glm::vec3(0, 1.3, 3));
      for (int i = 0; i < points.size(); i += 3) {
        auto *triangle = new Triangle(points[i], points[i + 1], points[i + 2]);
        triangle->setTransformation(translationMatrix);
        triangles.push_back(triangle);
      }
    }
    return triangles;
  }
  static int extract_index(string x) {
    replace(x.begin(), x.end(), '/', ' ');
    std::stringstream ss(x);
    int p;
    ss >> p;
    return p - 1;
  }
  Node *node_tree(vector<Triangle *> points, int begin, int end, int depth = 0) {
    if (end <= begin)
      return nullptr;
    unsigned int k = 3;
    unsigned int axis = depth % k;
    sort(&points[begin], &points[end], [axis](const Triangle *a, const Triangle *b) -> bool {
      if (axis == 0) {
        return a->v1.x < b->v1.x && a->v2.x < b->v2.x && a->v3.x < b->v3.x;
      } else if (axis == 1) {
        return a->v1.y < b->v1.y && a->v2.y < b->v2.y && a->v3.y < b->v3.y;
      } else {
        return a->v1.z < b->v1.z && a->v2.z < b->v2.z && a->v3.z < b->v3.z;
      }
    });
    int median = begin + (end - begin) / 2;
    return new Node(
        median,
        points[median],
        node_tree(points, begin, median, depth + 1),
        node_tree(points, median + 1, end, depth + 1));
  }

  void kdtree(const vector<Triangle *> &triangles) {
    tree = node_tree(triangles, 0, (int) triangles.size());
  }
 public:
  Figure(const string &name, bool flag) {
    ifstream myfile;
    myfile.open(name);
    char v;
    float x, y, z;
    string f1, f2, f3, f4;
    vector<point> points;
    vector<vector<int>> faces;
    vector<point> ret_points;
    string str;
    if (myfile.is_open()) {
      while (getline(myfile, str)) {
        std::stringstream ss(str);
        ss >> v;
        if (v == 'v') {
          ss >> x >> y >> z;
          points.push_back({x, y, z});
        }
        if (v == 'f') {
          ss >> f1 >> f2 >> f3;
          ret_points.push_back(points[extract_index(f1)]);
          ret_points.push_back(points[extract_index(f2)]);
          ret_points.push_back(points[extract_index(f3)]);
        }
      }
      myfile.close();
    }
    vector<Triangle *> triangles = parse_to_triangles(ret_points, flag);
    kdtree(triangles);
  }

  Hit intersect(Ray ray) {
    return intersect_local(ray, tree, 0);
  }

  Hit intersect_local(Ray ray, Node *node, int depth) {
    unsigned int k = 3;
    unsigned int axis = depth % k;
    if (node == nullptr) {
      Hit hit{};
      hit.hit = false;
      return hit;
    }
    Hit hit = node->p->intersect(ray);
    if (hit.hit)
      return hit;
    if (axis == 0) {
      if (ray.direction.x <= node->split_plane) {
        hit = intersect_local(ray, node->left, depth + 1);
        if (!hit.hit) {
          hit = intersect_local(ray, node->right, depth + 1);
        }
      } else {
        hit = intersect_local(ray, node->right, depth + 1);
        if (!hit.hit) {
          hit = intersect_local(ray, node->left, depth + 1);
        }
      }
    } else if (axis == 1) {
      if (ray.direction.y <= node->split_plane) {
        hit = intersect_local(ray, node->left, depth + 1);
        if (!hit.hit) {
          hit = intersect_local(ray, node->right, depth + 1);
        }
      } else {
        hit = intersect_local(ray, node->right, depth + 1);
        if (!hit.hit) {
          hit = intersect_local(ray, node->left, depth + 1);
        }
      }
    } else {
      if (ray.direction.z <= node->split_plane) {
        hit = intersect_local(ray, node->left, depth + 1);
        if (!hit.hit) {
          hit = intersect_local(ray, node->right, depth + 1);
        }
      } else {
        hit = intersect_local(ray, node->right, depth + 1);
        if (!hit.hit) {
          hit = intersect_local(ray, node->left, depth + 1);
        }
      }
    }
    return hit;
  }
};
#endif //USI_RENDERING_COMPETITION_OBJECT_FIGURE_H_
