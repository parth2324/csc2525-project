#include "dataloader.h"
#include <algorithm>

void DataLoader::loadSamples(std::ifstream &fileStream)
{

    // First pass: count total rows using a fixed-size buffer.
    const std::size_t bufferSize = 4096; // Adjust buffer size as needed.
    std::vector<char> buffer(bufferSize);
    std::size_t rowCount = 0;

    while (fileStream.read(buffer.data(), bufferSize) || fileStream.gcount() > 0)
    {
        // Count newline characters in the current buffer block.
        rowCount += std::count(buffer.begin(), buffer.begin() + fileStream.gcount(), '\n');
    }

    this->num_samples = rowCount;

    // Reset the file stream for the second pass.
    fileStream.clear();
    fileStream.seekg(0, std::ios::beg);

    // Reserve space for our data vector.
    std::vector<float> data;
    this->data.reserve(num_samples);

    // Second pass: read the file line by line.
    std::string line;
    // Skip the header line.
    std::getline(fileStream, line); // Assuming the first line is a header.
    while (std::getline(fileStream, line))
    {
        if (line.empty())
            continue; // Skip empty lines.
        // Since there's only one field per line, convert the whole line.
        std::string field(line);
        this->data.push_back(std::stof(field));
    }

    // At this point, 'data' holds all the float values from the file.
    std::cout << "Loaded " << data.size() << " float samples.\n";
}