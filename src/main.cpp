#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include "ktree.h"
#include "segment.h"

float gen_func(int ind){
    // if(ind <= 11) return ind * ind * ind;
    // ind -= 11;
    // return (11 * 11 * 11) + (ind * ind * ind);
    float v = 3 * ind + 2;
    if(ind < 7){
        return ind / v;
    }
    ind -= 6;
    return ((ind + 6) / v) + (ind * ind * ind);
}

float* gen_edgecase(int E, int N){
    float x1 = 10, half_e = E / 2.0, inv_e = 1.0 / E, x_curr;
    float* vals = (float*) malloc(sizeof(float) * (4 + (N * (E + 2))));
    int ind = 3;
    vals[0] = x1;
    vals[1] = x1 + half_e;
    vals[2] = x1 + E;
    x_curr = x1 + E + inv_e;
    for(int i = 0; i < N; i++, ind++){
        for(int j = 0; j <= E; j++, ind++){
            vals[ind] = x_curr;
        }
        vals[ind] = x_curr + inv_e;
        x_curr = vals[ind] + E;
    }
    vals[ind] = x_curr - half_e;
    return vals;
}

int main(int argc, char *argv[])
{
    const int K = 3;
    int NUM_VALS, MAX_ERR;
    KTree<K> tree;

    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <num_vals> <max_err>\n";
        return 1;
    }

    NUM_VALS = std::stoi(argv[1]);
    MAX_ERR = std::stoi(argv[2]);

    float* vals = (float*) malloc(sizeof(float) * NUM_VALS);
    for (int i = 0; i < NUM_VALS; i++)
    {
        vals[i] = gen_func(i);
    }

    // float* vals = (float*) malloc(sizeof(float) * NUM_VALS);
    // int rep_cnt = 0.7 * NUM_VALS;
    // for (int i = 0; i < rep_cnt; i++)
    // {
    //     vals[i] = 1;
    // }
    // for (int i = rep_cnt; i < NUM_VALS; i++)
    // {
    //     vals[i] = 1+ gen_func(i);
    // }

    // int N = 5;
    // NUM_VALS = 4 + (N * (MAX_ERR + 2));
    // float* vals = gen_edgecase(MAX_ERR, N);

    std::vector<std::pair<float, Segment *>> cone_data = gen_segments_cone(vals, NUM_VALS, MAX_ERR);
    std::vector<std::pair<float, Segment *>> dcone_data = gen_segments_dcone(vals, NUM_VALS, MAX_ERR);
    Segment *seg;

    std::ofstream cone_stream("seg_cone_out.txt");
    if (!cone_stream)
    {
        std::cerr << "Error opening file for writing!\n";
        return 1;
    }

    for (auto &[k, ptr] : cone_data)
    {
        print_segment(ptr, cone_stream);
    }

    cone_stream.close();

    std::ofstream dcone_stream("seg_dcone_out.txt");
    if (!dcone_stream)
    {
        std::cerr << "Error opening file for writing!\n";
        return 1;
    }

    for (auto &[k, ptr] : dcone_data)
    {
        print_segment(ptr, dcone_stream);
    }

    dcone_stream.close();

    // for (int i = 1; i <= 100; ++i) {

    //     vals = (int*)malloc(sizeof(int) * SEG_SIZE);
    //     for(int j = 0; j < SEG_SIZE; j++){
    //         vals[j] = (SEG_SIZE * i) + j;
    //     }
    //     seg = new Segment(vals, SEG_SIZE);
    //     data.emplace_back((SEG_SIZE * (i + 1)) - 1, seg);
    // }

    // tree.build(data);

    // int test_key = 34;
    // seg = tree.search_tree(test_key);
    // int *result = search_segment(seg, test_key);
    // if(result) std::cout << "Found " << test_key << "\n";
    // else std::cout << "Not found " << test_key << "\n";

    for (auto &[k, ptr] : cone_data)
    {
        delete ptr;
    }
    for (auto &[k, ptr] : dcone_data)
    {
        delete ptr;
    }
    free(vals);
    return 0;
}
