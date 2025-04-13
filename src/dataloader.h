// DataLoader class that loads data from a CSV file and provides methods to access the data
// and labels. It also provides a method to get the number of samples in the dataset.

#ifndef DATALOADER_H
#define DATALOADER_H
#endif

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

/* This class is designed to load data from a CSV file.
 * Since our application is meant to test main memory databases, we won't batch the data under
   the assumption that the dataset fits in memory. Thi decision is made to reduce the impact of
   the data loading process on the performance of the database.
 * The class also provides a method to get the number of samples in the dataset.
*/

class DataLoader
{
    size_t num_samples;

    // This method should only be called by the constructor
    size_t countSamples();

public:
    std::string filePath;
    std::ifstream fileStream;

    DataLoader(const std::string &filePath, size_t batchSize)
        : filePath(filePath)
    {
        num_samples = countSamples();
        fileStream = std::ifstream(filePath);
        if (!fileStream.is_open())
        {
            std::cerr << "Error opening file: " << filePath << std::endl;
            return;
        }
    }

    ~DataLoader()
    {
        if (fileStream.is_open())
        {
            fileStream.close();
        }
    }

    // Method to get the number of samples in the dataset
    int getNumSamples() const
    {
        return num_samples;
    }
};