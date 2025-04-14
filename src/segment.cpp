#include <vector>
#include <limits>
#include <iostream>
#include <fstream>
#include "segment.h"

// const float FLT_INF = std::numeric_limits<float>::infinity();
// const float INT_INF = std::numeric_limits<int>::max();

float* search_segment(Segment* segment, float key){
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

std::vector<std::pair<float, Segment*>> gen_segments_cone(float* data, int size, int err){ // fix by diff
    std::vector<std::pair<float, Segment*>> res;
    if(size == 0) return res;
    Segment* seg;
    float *curr, st, dlt;
    int loc, up, dn, ind = 0;
    float slhigh, sllow;
    while(ind < size){
        curr = (data + ind);
        st = data[ind];
        ind += 1;
        if(size - ind > 0){
            dlt = data[ind] - st;
            if(dlt == 0){
                loc = 1;
                while(dlt == 0 && loc <= err && ind < size){
                    dlt = data[ind] - st;
                    ind += 1;
                    loc += 1;
                }
                if(ind < size){
                    seg = new Segment(curr, loc, data[ind - loc], 0);
                    res.emplace_back(data[ind - 1], seg);
                }
            }
            else{
                slhigh = (1.0 + err) / dlt;
                sllow = (1.0 - err) / dlt;
                ind += 1;
                loc = 2;
                // std::cout << "--------------------\n";
                if(size - ind == 1) break;
                while(ind < size){
                    dlt = data[ind] - st;
                    up = (int)(slhigh * dlt);
                    dn = (int)(sllow * dlt);
                    // std::cout << "y: " << loc << ", y_high: " << up << ", y_low: " << dn << ", slh: " << slhigh << ", sll: " << sllow << "\n";
                    if(loc > up || loc < dn){
                        seg = new Segment(curr, loc, data[ind - loc], (slhigh + sllow) / 2);
                        res.emplace_back(data[ind - 1], seg);
                        break;
                    }
                    if(loc + err < up) slhigh = (loc + err) / dlt;
                    if(loc - err > dn) sllow = (loc - err) / dlt;
                    ind += 1;
                    loc += 1;
                }
            }
        }
        else{
            loc = 1;
            slhigh = 0;
            sllow = 0;
            break;
        }
    }
    seg = new Segment(curr, loc, data[ind - loc], (slhigh + sllow) / 2);
    res.emplace_back(data[ind - 1], seg);
    return res;
}

std::vector<std::pair<float, Segment*>> gen_segments_irng(float* data, int size, int err){
    std::vector<std::pair<float, Segment*>> res;
    if(size == 0) return res;
    Segment* seg;
    float *curr, st, dlt, st_up, st_dn;
    int dup_cnt, loc, up, dn, ind = 0;
    float slup, sldn;
    Make_Segment:
    while(ind < size){
        curr = (data + ind);
        st = data[ind];
        ind += 1;
        if(size - ind > 0){
            dlt = data[ind] - st;
            // need to set st_up, st_dn, slup, sldn, loc
            dup_cnt = 0;
            while(dlt == 0 && ind < size){
                dlt = data[ind] - st;
                ind += 1;
                dup_cnt += 1;
            }
            if(dup_cnt > 0){
                // our start has duplicates, need custom ranging
                if((err << 1) < dup_cnt){
                    // valid range: ind - E + dup_cnt to ind + E
                    // not possible to include all, cut losses
                    ind -= (dup_cnt - (err << 1));
                    dup_cnt = err << 1;
                    seg = new Segment(curr, dup_cnt + 1, data[ind - dup_cnt - 1], 0);
                    res.emplace_back(data[ind - 1], seg);
                    goto Make_Segment;
                }
                else if(ind < size){
                    // got something left we can use to extend!
                    st_up = err;
                    st_dn = dup_cnt - err;
                    dlt = data[ind] - st;
                    loc = dup_cnt + 1;
                    slup = (err + 1 - st_up) / dlt;
                    sldn = (err - 1 - st_dn) / dlt;
                }
                else{
                    // got everything possible
                    loc = dup_cnt + 1;
                    slup = 0;
                    sldn = 0;
                    break;
                }
            }
            else{
                // no dups, easy start
                st_up = err;
                st_dn = -err;
                slup = 1.0 / dlt;
                sldn = slup;
                ind += 1;
                loc = 2;
            }
            std::cout << "--------------------\n";
            if(size - ind == 1) break;
            while(ind < size){
                dlt = data[ind] - st;
                up = (int)(slup * dlt) + st_up;
                dn = (int)(sldn * dlt) + st_dn;
                std::cout << "y: " << loc << ", y_high: " << up << ", y_low: " << dn << ", slup: " << slup << ", sldn: " << sldn << ", stup: " << st_up << ", stdn: " << st_dn << "\n";
                if(loc - err > up || loc + err < dn){
                    seg = new Segment(curr, loc, data[ind - loc], (slup + sldn) / 2);
                    res.emplace_back(data[ind - 1], seg);
                    break;
                }
                if(loc + err < up) slup = (loc + err - st_up) / dlt;
                if(loc - err > dn) sldn = (loc - err - st_dn) / dlt;
                ind += 1;
                loc += 1;
            }
        }
        else{
            loc = 1;
            slup = 0;
            sldn = 0;
            break;
        }
    }
    seg = new Segment(curr, loc, data[ind - loc], (slup + sldn) / 2);
    res.emplace_back(data[ind - 1], seg);
    return res;
}
