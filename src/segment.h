#pragma once

struct Segment
{
    float *data, slp;
    int size;
    Segment(float *values, int num, float slope)
        : data(values), size(num), slp(slope)
    {}
};

float* search_segment(Segment *segment, float key);
void print_segment(Segment *segment, std::ofstream &stream);
std::vector<std::pair<float, Segment *>> gen_segments_cone(float *data, int size, int err);
std::vector<std::pair<float, Segment *>> gen_segments_dcone(float *data, int size, int err);
