#include <iostream>
#include <vector>
#include <numeric>
#include <chrono>
#include <random>

class Timer
{
public:
    Timer()
    {
        timeStart_ = std::chrono::steady_clock::now();
    };
    ~Timer(){};

    double timerStop()
    {
        return std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(std::chrono::steady_clock::now() - timeStart_).count();
    }

    double timerReset()
    {
        timeStart_ = std::chrono::steady_clock::now();
    }
private:
    std::chrono::time_point<std::chrono::steady_clock> timeStart_;
};

template<typename T> requires std::is_same_v<T, double> || std::is_same_v<T, float>
std::vector<T> generateData(unsigned int dataSize = 1000000)
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::vector<T> data(dataSize);

    std::uniform_real_distribution<T> num{0, 1000};
    auto gen = [&](){return num(g);};
    std::generate(data.begin(), data.end(), gen);

    return data;
}

template<typename T> requires std::is_same_v<T, double> || std::is_same_v<T, float>
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

    return result;
}


int main()
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<int> dataSize(1000000,2000000);
    dataSize(rd);
    std::vector<float> data = generateData<float>(dataSize(rd));
    unsigned int k = 14;
    if(data.empty() || (data.size() < k)){return 0;}

    std::cout << "dataSize = " << data.size() << std::endl;
    Timer timer;
    std::vector<float> result = getSMA(data, k);
    std::cout << timer.timerStop() << std::endl;
    //for(auto &el: result)
    //{
    //    std::cout << el << " ";
    //}

    return 0;
}
