//
// Created by Volodymyr Karpenko on 16.11.21.
//

#ifndef USI_RENDERING_COMPETITION__KDTREE_H_
#define USI_RENDERING_COMPETITION__KDTREE_H_
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
  point p;
  Node *left;
  Node *right;
  Node(point p, Node *left, Node *right) : p(std::move(p)), left(left), right(right) {

  }
  ~Node() {
    delete left;
    delete right;
  }
};

Node *kdtree_h(vector<point> points, int begin, int end, int depth = 0) {
  if (end <= begin)
    return nullptr;
  unsigned int k = points[0].size();
  unsigned int axis = depth % k;
  sort(&points[begin], &points[end], [axis](const point &a, const point &b) -> bool {
    return a[axis] < b[axis];
  });
  int median = begin + (end - begin) / 2;
  return new Node(
      points[median],
      kdtree_h(points, begin, median, depth + 1),
      kdtree_h(points, median + 1, end, depth + 1));
}

Node *kdtree(const vector<point> &points) {
  return kdtree_h(points, 0, (int) points.size());
}

int extract_index(string x) {
  replace(x.begin(),x.end(),'/',' ');
  std::stringstream ss(x);
  int p;
  ss >> p;
  return p-1;
}

vector<point> parseFile(const string &name) {
  ifstream myfile;
  myfile.open(name);
  char v;
  float x, y, z;
  string f1, f2, f3, f4;
  vector<point> points;
  vector<vector<int>> faces;
  vector<point> ret_points;
  string str;
  bool flag = false;
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
  return ret_points;
}
#endif //USI_RENDERING_COMPETITION__KDTREE_H_
