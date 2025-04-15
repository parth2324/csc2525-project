#include "ktree.h"
#include "segment.h"
#include "dataloader.h"

#include <chrono>

int ERROR = 4; // Example error value

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <csv_file_path>" << std::endl;
        return 1;
    }

    // Load the dataset from the CSV file
    // The CSV file should contain one float value per line
    std::string filePath = argv[1];
    DataLoader dataLoader(filePath);

    if (dataLoader.getNumSamples() == 0)
    {
        std::cerr << "No samples found in the dataset." << std::endl;
        return 1;
    }

    std::vector<float> data = dataLoader.getData();

    // We will time the segment generation for our two algorithms
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<float, Segment *>> cone_entries = gen_segments_cone(data.data(), data.size(), ERROR);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Time taken to generate " << cone_entries.size() << " segments (cone): " << elapsed.count() << " seconds." << std::endl;

    start = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<float, Segment *>> dcone_entries = gen_segments_dcone(data.data(), data.size(), ERROR);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Time taken to generate " << dcone_entries.size() << " segments (dcone): " << elapsed.count() << " seconds." << std::endl;

    // Build the KTree with the generated segments
    KTree<4> cone_kTree; // Example with K=4
    cone_kTree.build(cone_entries);
    std::cout << "KTree with cone segments built successfully." << std::endl;

    KTree<4> dcone_kTree; // Example with K=4
    dcone_kTree.build(dcone_entries);
    std::cout << "KTree with dcone segments built successfully." << std::endl;

    // Now we will time the search operation
    float search_key = 0.5f; // Example search key
    start = std::chrono::high_resolution_clock::now();
    float *value = cone_kTree.lookup(search_key, ERROR);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Time taken to search in KTree (cone): " << elapsed.count() << " seconds." << std::endl;
    if (value)
    {
        std::cout << "Value found in cone KTree." << std::endl;
    }
    else
    {
        std::cout << "Value not found in cone KTree." << std::endl;
    }

    start = std::chrono::high_resolution_clock::now();
    value = dcone_kTree.lookup(search_key, ERROR);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Time taken to search in KTree (dcone): " << elapsed.count() << " seconds." << std::endl;
    if (value)
    {
        std::cout << "Value found in dcone KTree." << std::endl;
    }
    else
    {
        std::cout << "Value not found in dcone KTree." << std::endl;
    }

    // Now we will print the segments to a file
    std::ofstream cone_stream("seg_cone_out.txt");

    std::ofstream dcone_stream("seg_dcone_out.txt");

    for (auto &[k, ptr] : cone_entries)
    {
        print_segment(ptr, cone_stream);
    }
    cone_stream.close();

    for (auto &[k, ptr] : dcone_entries)
    {
        print_segment(ptr, dcone_stream);
    }
    dcone_stream.close();

    std::cout << "Segments printed to files." << std::endl;

    // Clean up
    for (auto &entry : cone_entries)
    {
        delete entry.second;
    }
    for (auto &entry : dcone_entries)
    {
        delete entry.second;
    }
    cone_entries.clear();
    dcone_entries.clear();
    data.clear();

    return 0;
}