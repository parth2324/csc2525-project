#include <vector>
#include <limits>
#include <iostream>
#include <fstream>
#include <cmath>
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
                    seg = new Segment(curr, loc, 0);
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
                    up = round(slhigh * dlt);
                    dn = round(sllow * dlt);
                    // std::cout << "y: " << loc << ", y_high: " << up << ", y_low: " << dn << ", slh: " << slhigh << ", sll: " << sllow << "\n";
                    if(loc > up || loc < dn){
                        seg = new Segment(curr, loc, (slhigh + sllow) / 2);
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
    seg = new Segment(curr, loc, (slhigh + sllow) / 2);
    res.emplace_back(data[ind - 1], seg);
    return res;
}

std::vector<std::pair<float, Segment*>> gen_segments_dcone(float* data, int size, int err){
    std::vector<std::pair<float, Segment*>> res;
    if(size == 0) return res;
    Segment* seg;
    float *curr, st, dlt, st_up, st_dn, m;
    int dup_cnt, loc, up, dn, ind = 0;
    float slup, sldn;
    bool m_inc, m_dec;
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
                if(err < dup_cnt){
                    // not possible to include all, cut losses
                    ind -= (dup_cnt - err);
                    dup_cnt = err;
                    seg = new Segment(curr, dup_cnt + 1, 0);
                    res.emplace_back(data[ind - 1], seg);
                    goto Make_Segment;
                }
                else if(ind < size){
                    // got something left we can use to extend!
                    st_up = err;
                    st_dn = dup_cnt - err;
                    dlt = data[ind] - st;
                    loc = dup_cnt + 1;
                    slup = (err + 1 - st_dn) / dlt;
                    sldn = (err - 1 - st_up) / dlt;
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
                slup = (err + 1 - st_dn) / dlt;
                sldn = (err - 1 - st_up) / dlt;
                ind += 1;
                loc = 2;
            }
            // std::cout << "--------------------\n";
            while(ind < size){
                dlt = data[ind] - st;
                up = round(slup * dlt) + st_dn;
                dn = round(sldn * dlt) + st_up;
                // std::cout << "y: " << loc << ", y_high: " << up << ", y_low: " << dn << ", slup: " << slup << ", sldn: " << sldn << ", stup: " << st_up << ", stdn: " << st_dn << "\n";
                if(loc - err > up || loc + err < dn){
                    m = (slup + sldn) / 2;
                    int i = ind - loc, j, prd;
                    m_inc = false;
                    m_dec = false;
                    for(j = 0; i < ind; j++, i++){
                        dlt = data[i] - st;
                        prd = round(m*dlt);
                        if(j - err > prd){
                            if(m_dec) break;
                            m = (j - err) / dlt;
                            m_inc = true;
                        }
                        if(j + err < prd){
                            if(m_inc) break;
                            m = (j + err) / dlt;
                            m_dec = true;
                        }
                    }
                    seg = new Segment(curr, j, m);
                    res.emplace_back(data[i - 1], seg);
                    if(i < ind){
                        // break computed segment :(
                        std::cerr << "Lost " << (ind - i) << " iterations (" << (float)(ind - i) / size << " of total)\n";
                        ind = i;
                    }
                    break;
                }
                if(loc + err < up) slup = (loc + err - st_dn) / dlt;
                if(loc - err > dn) sldn = (loc - err - st_up) / dlt;
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
    if(loc > 2){
        m = (slup + sldn) / 2;
        int i = ind - loc, j, prd;
        m_inc = false;
        m_dec = false;
        for(j = 0; i < ind; j++, i++){
            dlt = data[i] - st;
            prd = round(m*dlt);
            if(j - err > prd){
                if(m_dec) break;
                m = (j - err) / dlt;
                m_inc = true;
            }
            if(j + err < prd){
                if(m_inc) break;
                m = (j + err) / dlt;
                m_dec = true;
            }
        }
        seg = new Segment(curr, j, m);
        res.emplace_back(data[i - 1], seg);
        if(i < ind){
            // break computed segment :(
            std::cerr << "Lost " << (ind - i) << " iterations (" << (float)(ind - i) / size << " of total)\n";
            ind = i;
            goto Make_Segment;
        }
    }
    else{
        if(loc == 2) m = 1 / ((*(curr + 1)) - (*curr));
        else m = 0;
        seg = new Segment(curr, loc, m);
        res.emplace_back(data[ind - 1], seg);
    }
    return res;
}
