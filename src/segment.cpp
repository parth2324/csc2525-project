#include <vector>
#include <limits>
#include <iostream>
#include <fstream>
#include "segment.h"

const float FLT_INF = std::numeric_limits<float>::infinity();
const float INT_INF = std::numeric_limits<int>::max();

int* search_segment(Segment* segment, int key){
    for(int i = 0; i < segment->size; i++){
        if(segment->data[i] == key) return (segment->data + i);
    }
    return nullptr;
}

void print_segment(Segment* segment, std::ofstream& stream){
    stream << segment->size << "\n";
    stream << segment->slp << "\n";
    for(int i = 0; i < segment->size; i++){
        stream << segment->data[i] << "\n";
        //" at loc: " << i << ", slope-prediction: " << (int)(segment->slp * (segment->data[i] - segment->low)) << "\n";
    }
}

std::vector<std::pair<int, Segment*>> gen_segments_cone(int* data, int size, int err){
    int *curr, st, loc, up, dn, dlt, ind = 0;
    float slhigh, sllow;
    std::vector<std::pair<int, Segment*>> res;
    Segment* seg;
    while(ind < size){
        curr = (data + ind);
        st = data[ind];
        slhigh = FLT_INF;
        sllow = 0;
        loc = 1;
        ind += 1;
        while(ind < size){
            dlt = data[ind] - st;
            up = slhigh == FLT_INF ? INT_INF : (int)(slhigh * dlt);
            dn = sllow == FLT_INF ? INT_INF : (int)(sllow * dlt);
            // std::cout << up << " " << dn << " " << slhigh << " " << sllow << "\n";
            if(loc > up || loc < dn){
                seg = new Segment(curr, loc, (slhigh + sllow) / 2);
                res.emplace_back(data[ind - 1], seg);
                if(ind == size) ind += 1;
                break;
            }
            if(dlt > 0){
                if(slhigh == FLT_INF || up - loc > err) slhigh = (float)(loc + err) / dlt;
                if(loc - dn > err) sllow = (float)(loc - err) / dlt;
            }
            else sllow = FLT_INF;
            ind += 1;
            loc += 1;
        }
    }
    if(ind == size){
        seg = new Segment(curr, loc, (slhigh + sllow) / 2);
        res.emplace_back(data[ind - 1], seg);
    }
    return res;
}

std::vector<std::pair<int, Segment*>> gen_segments_wmpt(int* data, int size, int err){
    std::vector<std::pair<int, Segment*>> res;
    return res;
}
