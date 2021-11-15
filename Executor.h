//
// Created by Volodymyr Karpenko on 12.11.21.
//

#ifndef USI_RENDERING_COMPETITION__EXECUTOR_H_
#define USI_RENDERING_COMPETITION__EXECUTOR_H_

#include <queue>
#include "Image.h"


typedef struct {
  float s;
  float X;
  float Y;
  int i;
  int j;
} arguments;

std::mutex mu;

class Executor {
 private:
  std::vector<arguments> jobs;

 public:
  void add_job(float s, float X, float Y, int start, int end,int height) {
//    cout <<"start"+to_string(start)+"/"<< endl;
//    cout <<"end"+ to_string(end)+"/"<< endl;
    for (int i = start; i < end; i++) {
      for (int j = 0; j < height; j++) {
        jobs.emplace_back(arguments{s, X, Y, i, j});
      }
    }
  }

  void execute_jobs(glm::vec3(*fun)(float, float, float, int, int),Image image) {
    while (!jobs.empty()) {
      arguments c = jobs.back();
      glm::vec3 res = fun(c.s, c.X, c.Y, c.i, c.j);
//      mu.lock();
      image.setPixel(c.i,c.j,res);
//      mu.unlock();
      jobs.pop_back();
    }
  }
};
#endif //USI_RENDERING_COMPETITION__EXECUTOR_H_
