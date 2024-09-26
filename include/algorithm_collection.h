#ifndef ALGORITHM_COLLECTION_H
#define ALGORITHM_COLLECTION_H
#include <iostream>
#include <vector>
#include <numeric>

namespace AlgorithmCollection {
template<typename T>
std::vector<T> getSMA(const std::vector<T> &data, unsigned int k)
{
    if(k == 0){return data;}

    std::vector<T> result;
    T currentSum = 0;
    T currentAverage = 0;

    result.reserve(data.size() - k + 1);
    currentSum = std::accumulate(data.begin(), data.begin() + k, 0);
    currentAverage = currentSum / k;
    result.push_back(currentAverage);
    for(unsigned int i = 0; i < data.size() - k; i++)
    {
        currentSum -= data[i];
        currentSum += data[i+k];
        currentAverage = currentSum / k;
        result.push_back(currentAverage);
    }
    return std::move(result);
}
};

#endif /* ALGORITHM_COLLECTION_H */
