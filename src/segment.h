#pragma once

struct Segment {
    int low, high, *data, size;
    float slp;
    Segment(int* values, int num, float slope)
        : data(values), size(num), slp(slope){
            low = data[0];
            high = data[num - 1];
        }
};

int* search_segment(Segment* segment, int key);
void print_segment(Segment* segment, std::ofstream& stream);
std::vector<std::pair<int, Segment*>> gen_segments_cone(int* data, int size, int err);
std::vector<std::pair<int, Segment*>> gen_segments_wmpt(int* data, int size, int err);
