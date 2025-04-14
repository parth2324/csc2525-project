#include <vector>
#include <limits>
#include <iostream>
#include <fstream>
#include <cmath>
#include "segment.h"

// const float FLT_INF = std::numeric_limits<float>::infinity();
// const float INT_INF = std::numeric_limits<int>::max();

float* search_segment(Segment* segment, float key, int E){
    float val = segment->data[0];
    // quick return if key doesn't exist
    if(key < val || key > *(segment->last)) return nullptr;
    int m = round(segment->slp * (key - val)), l, r;
    val = segment->data[m];
    // checking original prediction
    if (val == key) return segment->data + m;
    else if(val < key){
        l = m + 1;
        r = m + E;
    }
    else{
        l = m - E;
        r = m - 1;
    }
    // binary search in reduced range
    while(l <= r){
        m = (l + r) >> 1;
        val = segment->data[m];
        if (val == key) return segment->data + m;
        else if (val < key) l = m + 1;
        else r = m - 1;
    }
    // key not found
    return nullptr;
}

void print_segment(Segment* segment, std::ofstream& stream){
    stream << segment->size << "\n";
    stream << segment->slp << "\n";
    for(int i = 0; i < segment->size; i++){
        stream << segment->data[i] << "\n";
    }
}

std::vector<std::pair<float, Segment*>> gen_segments_cone(float* data, int size, int E){
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
            // atleast two points to process
            dlt = data[ind] - st;
            if(dlt == 0){
                /*
                 * Duplicates at the start force slhigh = sllow = inifnity.
                 * 
                 * We qucikly aquire all duplicates, upto E, and push them into a segment, then
                 * continue processing if there are keys remaining.
                 * 
                 * If we reach the end, the we don't need to explictly add a segment, it will
                 * be covered by final segment addition.
                 */
                loc = 1;
                while(dlt == 0 && loc <= E && ind < size){
                    dlt = data[ind] - st;
                    ind += 1;
                    loc += 1;
                }
                if(ind < size){
                    seg = new Segment(curr, loc, 0);
                    res.emplace_back(data[ind - 1], seg);
                }
                else{
                    /*
                     * The case when all we have is duplicates in our current segment and there
                     * are no more keys to process. Just need to setup values for final segment.
                     */
                    slhigh = 0;
                    sllow = 0;
                    break;
                }
            }
            else{
                /*
                 * Initializng cone via slhigh & sllow.
                 * 
                 * This is done by using the second point, and defining the upper limit
                 * line between (first_key, 0) and (second_key, 1 + E), and the lower
                 * limit line between (first_key, 0) and (second_key, 1 - E).
                 */
                slhigh = (1.0 + E) / dlt;
                sllow = (1.0 - E) / dlt;
                ind += 1;
                loc = 2;
                while(ind < size){
                    /*
                     * Adding as many keys as possible to the cone.
                     * 
                     * If the new key falls in the cone, we add to current segment and continue.
                     * Otherwise we add current segment and break out. Note that we jump out to 
                     * final segment if we run out of keys to add.
                     */
                    dlt = data[ind] - st;
                    // Recognizing decimal value when predicting position via rounding.
                    up = round(slhigh * dlt);
                    dn = round(sllow * dlt);
                    if(loc > up || loc < dn){
                        seg = new Segment(curr, loc, (slhigh + sllow) / 2);
                        res.emplace_back(data[ind - 1], seg);
                        break;
                    }
                    /*
                     * changing upper and lower lines to not include regions outside current key's
                     * error tolerance range.
                     */
                    if(loc + E < up) slhigh = (loc + E) / dlt;
                    if(loc - E > dn) sllow = (loc - E) / dlt;
                    ind += 1;
                    loc += 1;
                }
            }
        }
        else{
            // single point to process, setting up values for final segment.
            loc = 1;
            slhigh = 0;
            sllow = 0;
            break;
        }
    }
    // final segment addition
    seg = new Segment(curr, loc, (slhigh + sllow) / 2);
    res.emplace_back(data[ind - 1], seg);
    return res;
}

std::vector<std::pair<float, Segment*>> gen_segments_dcone(float* data, int size, int E){
    std::vector<std::pair<float, Segment*>> res;
    if(size == 0) return res;
    Segment* seg;
    float *curr, st, dlt, st_up, st_dn, m;
    int loc, up, dn, lost_iters = 0, ind = 0;
    float slup, sldn;
    bool m_inc, m_dec;
    Make_Segment:
    while(ind < size){
        curr = (data + ind);
        st = data[ind];
        ind += 1;
        if(size - ind > 0){
            // atleast two points to process
            dlt = data[ind] - st;
            loc = 1;
            // counting duplicates at the start, upto E.
            while(dlt == 0 && loc <= E && ind < size){
                dlt = data[ind] - st;
                ind += 1;
                loc += 1;
            }
            if(loc > 1){
                /*
                 * Duplicates at the start force smaller tolerance range around first key.
                 * 
                 * We process the duplicates acquired earlier, and then continue processing
                 * if there are keys remaining.
                 * 
                 * If we reach the end, the we don't need to explictly add a segment, it will
                 * be covered by final segment addition.
                 */
                if(ind < size){
                    /*
                     * If there are keys remaining which need to be processed, we compute the new
                     * tolerance range: intersection of [-E, E], [-E+1, E+1] ... [-E+loc-1,E+loc-1],
                     * which is [-E+loc-1,E] (The reason why we allow upto E duplicates in a segment
                     * as well, can be modified based on policy). Call it [st_dn, st_up].
                     * 
                     * If the next key is still a duplicate, we jump to the start of Make_Segment
                     * iteration after adding current segment.
                     * 
                     * Using the first non-duplicate, we then initialize our double cone to look like 
                     * an hour glass via lines in between (first_key, st_dn) and (second_key, E+1), and 
                     * in between (first_key, st_up) and (second_key, -E+1).
                     */
                    dlt = data[ind] - st;
                    if(dlt == 0){
                        seg = new Segment(curr, loc, 0);
                        res.emplace_back(data[ind - 1], seg);
                        goto Make_Segment;
                    }
                    st_up = E;
                    st_dn = loc - 1 - E;
                    slup = (E + 1 - st_dn) / dlt;
                    sldn = (E - 1 - st_up) / dlt;
                }
                else{
                    /*
                     * The case when all we have is duplicates in our current segment and there
                     * are no more keys to process. Just need to setup values for final segment.
                     */
                    slup = 0;
                    sldn = 0;
                    break;
                }
            }
            else{
                /*
                 * Using the first two keys, we then initialize our double cone to look like an hour 
                 * glass via lines in between (first_key, -E) and (second_key, E+1), and in 
                 * between (first_key, E) and (second_key, -E+1).
                 * 
                 * Note this scenario uses the standard tolerance range thus proceeding computation
                 * involving st_up & st_dn can be simplified. We keep fixed control flow and still
                 * use st_up = E & st_dn = -E.
                 */
                st_up = E;
                st_dn = -E;
                slup = (E + 1 - st_dn) / dlt;
                sldn = (E - 1 - st_up) / dlt;
                ind += 1;
                loc = 2;
            }
            while(ind < size){
                /*
                 * Adding as many keys as possible to the double cone / hour glass.
                 * 
                 * If the tolerance range for the (new_key, loc) intersects with our double 
                 * cone, we simply add it, and update the double cone to be in between the 
                 * lines between (first_key, st_dn) and (new_key, loc+E), and in between 
                 * (first_key, st_up) and (new_key, loc-E).
                 * 
                 * Otherwise, we enter the predictor gradient build pass.
                 * 
                 * Note that we jump out to final segment if we run out of keys to add.
                 */
                dlt = data[ind] - st;
                up = round(slup * dlt) + st_dn;
                dn = round(sldn * dlt) + st_up;
                if(loc - E > up || loc + E < dn){
                    /*
                     * Predictor gradient build pass.
                     * 
                     * The double cone algorithm cooks up a potential segment for us to add,
                     * but it doesn't check that error bounds are satisfied at all points
                     * explicitly, and doesn't have such a guarantee by design. See report
                     * for details on "curvature leaks", the cause of "false positive" 
                     * additions.
                     * 
                     * We thus, check all error bounds are satified, and further allow mutating 
                     * the gradient for our linear predictor only in one direction, supporting
                     * correction for curvature leaks only on one side. We informally argue the
                     * correctness in the report.
                     * 
                     * After this pass, we finally add the segment.
                     */
                    m = (slup + sldn) / 2;
                    int i = ind - loc, j, prd;
                    m_inc = false;
                    m_dec = false;
                    for(j = 0; i < ind; j++, i++){
                        dlt = data[i] - st;
                        prd = round(m*dlt);
                        if(j - E > prd){
                            if(m_dec) break;
                            m = (j - E) / dlt;
                            m_inc = true;
                        }
                        if(j + E < prd){
                            if(m_inc) break;
                            m = (j + E) / dlt;
                            m_dec = true;
                        }
                    }

                    // TO REMOVE: TOLERANCE CHECK
                    // int ii = ind - loc;
                    // for(int k = 0; k < j; k++, ii++){
                    //     dlt = data[ii] - st;
                    //     prd = round(m*dlt);
                    //     if(k - E > prd || k + E < prd){
                    //         std::cout << "err detected\n";
                    //     }
                    // }

                    seg = new Segment(curr, j, m);
                    res.emplace_back(data[i - 1], seg);
                    if(i < ind){
                        /*
                         * Our double cone algorithm had "false positives" equivalent.
                         * 
                         * We have points which cannot be fit using a singular linear line
                         * starting from (first_key, 0), while satisfying the positional error
                         * bound E. Thus, we simply break our segment maximally, and give away 
                         * the rest for reprocessing.
                         * 
                         * NOTE: Potential work is to be done here, exploring ways to use
                         * information computed in first pass, or reduce false positive rates 
                         * without sacrificing the optimality double cone algorithm brings.
                         */
                        lost_iters += ind - i;
                        ind = i;
                    }
                    break;
                }
                if(loc + E < up) slup = (loc + E - st_dn) / dlt;
                if(loc - E > dn) sldn = (loc - E - st_up) / dlt;
                ind += 1;
                loc += 1;
            }
        }
        else{
            // single point to process, setting up values for final segment.
            loc = 1;
            slup = 0;
            sldn = 0;
            break;
        }
    }
    if(loc > 2){
        /*
         * Predictor gradient build pass.
         *
         * See above for details. The only difference is that, upon hitting false
         * positives, we need to re-enter the loop we just exited, thus a "goto"
         * statement instead of simple break.
         */
        m = (slup + sldn) / 2;
        int i = ind - loc, j, prd;
        m_inc = false;
        m_dec = false;
        for(j = 0; i < ind; j++, i++){
            dlt = data[i] - st;
            prd = round(m*dlt);
            if(j - E > prd){
                if(m_dec) break;
                m = (j - E) / dlt;
                m_inc = true;
            }
            if(j + E < prd){
                if(m_inc) break;
                m = (j + E) / dlt;
                m_dec = true;
            }
        }
        seg = new Segment(curr, j, m);
        res.emplace_back(data[i - 1], seg);
        if(i < ind){
            std::cerr << "Lost " << (ind - i) << " iterations (" << (float)(ind - i) / size << " of total)\n";
            ind = i;
            goto Make_Segment;
        }
    }
    else{
        /*
         * Final segment of 1 or 2 points.
         * 
         * If single point, fitting line slope = 0.
         * 
         * Otherwise, for two points fitting line slope is that the line between
         * (first_key, 0) and (second_key, 1).
         */
        if(loc == 2) m = 1 / ((*(curr + 1)) - (*curr));
        else m = 0;
        seg = new Segment(curr, loc, m);
        res.emplace_back(data[ind - 1], seg);
    }
    std::cerr << "Lost " << lost_iters << " iterations (" << (float)lost_iters / size << " of total size)\n";
    return res;
}
