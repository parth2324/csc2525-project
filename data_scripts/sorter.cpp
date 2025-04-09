// A script to quickly sort data in a csv

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <file path to input.csv> <file path to output.csv>\n";
        return 1;
    }
    std::string inputPath = argv[1];
    std::string outputPath = argv[2];

    std::ifstream inputStream(inputPath);
    if (!inputStream.is_open())
    {
        std::cerr << "Error opening file: " << inputPath << std::endl;
        return 1;
    }

    std::vector<std::vector<float>> data;
    std::string line;

    // Read the CSV file line by line
    std::getline(inputStream, line); // Skip the header line
    while (std::getline(inputStream, line))
    {
        std::stringstream ss(line);
        std::string value;
        std::vector<float> row;

        // Split the line by commas
        while (std::getline(ss, value, ','))
        {
            row.push_back(std::stof(value));
        }
        data.push_back(row);
    }
    inputStream.close();
    // Sort the data based on the first column
    std::sort(data.begin(), data.end(), [](const std::vector<float> &a, const std::vector<float> &b)
              { return a[0] < b[0]; });
    // Write the sorted data to the output CSV file
    std::ofstream outputStream(outputPath);
    if (!outputStream.is_open())
    {
        std::cerr << "Error opening file: " << outputPath << std::endl;
        return 1;
    }

    // Write the header line
    outputStream << "sample\n";
    for (const auto &row : data)
    {
        for (size_t i = 0; i < row.size(); ++i)
        {
            outputStream << row[i];
            if (i < row.size() - 1)
            {
                outputStream << ",";
            }
        }
        outputStream << "\n";
    }
    outputStream.close();
    std::cout << "Data sorted and written to " << outputPath << std::endl;
    // Clean up
    data.clear();

    return 0;
}