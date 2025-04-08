#include "DataLoader.h"
#include <algorithm>

size_t DataLoader::countSamples()
{
    std::ifstream fileBuffer(filePath);
    if (!fileBuffer.is_open())
    {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return 0;
    }
    const std::size_t bufferSize = 4096; // Adjust buffer size as needed
    std::vector<char> buffer(bufferSize);
    std::size_t rowCount = 0;

    while (fileBuffer.read(buffer.data(), bufferSize) || fileBuffer.gcount() > 0)
    {
        rowCount += std::count(buffer.begin(), buffer.begin() + fileBuffer.gcount(), '\n');
    }

    return rowCount;
}