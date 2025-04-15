#pragma once

#include <vector>
#include <iostream>
#include <fstream>
struct Segment
{
    float *data, *last, slp;
    int size;
    Segment(float *values, int num, float slope)
        : data(values), size(num), slp(slope)
    {
        last = data + size - 1;
    }
};

float *search_segment(Segment *segment, float key, int err);
void print_segment(Segment *segment, std::ofstream &stream);
std::vector<std::pair<float, Segment *>> gen_segments_cone(float *data, int size, int err);
std::vector<std::pair<float, Segment *>> gen_segments_dcone(float *data, int size, int err);
