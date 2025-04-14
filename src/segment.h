#pragma once

struct Segment
{
    float *data, x_icpt, slp;
    int size;
    Segment(float *values, int num, float x_intercpt, float slope)
        : data(values), size(num), x_icpt(x_intercpt), slp(slope)
    {}
};

float* search_segment(Segment *segment, float key);
void print_segment(Segment *segment, std::ofstream &stream);
std::vector<std::pair<float, Segment *>> gen_segments_cone(float *data, int size, int err);
std::vector<std::pair<float, Segment *>> gen_segments_irng(float *data, int size, int err);
