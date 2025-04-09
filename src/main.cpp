#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include "ktree.h"
#include "segment.h"

int gen_func(int ind){
    return ind * ind;
}

int main(int argc, char* argv[]) {
    const int K = 3;
    int NUM_VALS, MAX_ERR;
    KTree<K> tree;

    if(argc != 3){
        std::cerr << "Usage: " << argv[0] << " <num_vals> <max_err>\n";
        return 1;
    }

    NUM_VALS = std::stoi(argv[1]);
    MAX_ERR = std::stoi(argv[2]);

    int *vals = (int*)malloc(sizeof(int) * NUM_VALS);
    for(int i = 0; i < NUM_VALS; i++){
        vals[i] = gen_func(i);
    }

    std::vector<std::pair<int, Segment*>> data = gen_segments_cone(vals, NUM_VALS, MAX_ERR);
    Segment* seg;

    std::ofstream stream("seg_out.txt");
    if (!stream) {
        std::cerr << "Error opening file for writing!\n";
        return 1;
    }

    for (auto& [k, ptr] : data){
        print_segment(ptr, stream);
    }

    stream.close();

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
    // seg = tree.lookup(test_key);
    // int *result = search_segment(seg, test_key);
    // if(result) std::cout << "Found " << test_key << "\n";
    // else std::cout << "Not found " << test_key << "\n";

    for (auto& [k, ptr] : data){
        delete ptr;
    }
    free(vals);
    return 0;
}
