/* Generates a csv file with values that step by step_size
 * Given a segment_size and a step_size, the generator will create a csv file with
 * segment_size * num_steps values.
 * The values will range from 0 to step_size * num_steps - 1.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cerr << "Usage: " << argv[0] << " <segment_size> <step_size> <num_steps> <output_file>\n";
        return 1;
    }
    // Parse command line arguments
    size_t segment_size = std::stoull(argv[1]);
    size_t step_size = std::stoull(argv[2]);
    size_t num_steps = std::stoull(argv[3]);
    std::string output_file = argv[4];

    // Open the output file
    std::ofstream output_stream(output_file);
    if (!output_stream.is_open())
    {
        std::cerr << "Error opening file: " << output_file << std::endl;
        return 1;
    }

    // Write the header line
    output_stream << "sample\n";

    // Generate the values and write them to the file
    for (size_t i = 0; i < num_steps; ++i)
    {
        for (size_t j = 0; j < segment_size; ++j)
        {
            size_t value = i * step_size;
            output_stream << value;
            output_stream << "\n";
        }
    }
    // Close the output file
    output_stream.close();
    std::cout << "Generated " << num_steps * segment_size << " values in " << output_file << std::endl;
    return 0;
}