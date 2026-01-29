#pragma once

#include <vector>

#include "implot.h"

class Graph {
   public:
    void plot();

    void add_point(float t, float v);

   private:
    // time and voltage data
    // private to make sure the sizes are the same
    std::vector<float> t_data_ = {};
    std::vector<float> v_data_ = {};
};