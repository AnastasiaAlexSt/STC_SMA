#ifndef ALGORITHM_EXECUTOR_H
#define ALGORITHM_EXECUTOR_H
#include <iostream>
#include <vector>
#include <numeric>
#include <unordered_map>
#include <random>
#include <memory>
#include "data_writer_xlsx.h"
#include "timer.h"
#include "algorithm_collection.h"

#define DEFAULT_DATASIZE 1000000
#define NUM_TEST 30
namespace  AlgoExecutor{

const std::string FILEPATH  = ".";
const std::string FILENAME = "result.xlsx";
const std::string COLUMNNAME_TYPE =  "dataType";
const std::string COLUMNNAME_WINDOW = "window";
const std::string COLUMNNAME_TIME = "time";
const std::string NAME_SHEET = "ResultData";
const std::string NAME_COUNT_TIME = "count/ms";
const std::string NAME_CHART = "Result";

static unsigned inline int current_row_ = 1;
enum class dataType
{
    doubleType = 0,
    floatType,
};

const std::unordered_map<dataType, std::string> dataTypeStr
{
    {dataType::doubleType, "double"},
    {dataType::floatType, "float"},
};

std::shared_ptr<DataWriterXLXS> CreateXlsxFileSMA();
lxw_chart* CreateChart(std::shared_ptr<DataWriterXLXS> dwXLXS);
void DrawSMAChart(std::shared_ptr<DataWriterXLXS> dwXLXS, lxw_chart *chart);
};

template <typename  T>
concept isDoubleOrFloat = std::is_same_v<T, double> || std::is_same_v<T, float>;

template <isDoubleOrFloat  V>
class AlgorithmExecutor
{
public:
     void SetXlsx(std::shared_ptr<DataWriterXLXS> dwXLXS);
     void SetChart(lxw_chart *chart);
     void SetParam(unsigned int data_size = DEFAULT_DATASIZE, bool en_seed = true, unsigned int seed = 10);
     void StartSMA(const std::vector<unsigned int> &windows);
private:
     void GenerateData();
     void AddSMADataToXlsx(unsigned int window, double mean_time, unsigned int current_row);
     void AddSMAChart(unsigned start_row, unsigned int end_row);

private:
    Timer timer_;
    std::vector<std::string> title_columns_;
    std::shared_ptr<DataWriterXLXS> dwXLXS_;
    lxw_chart *chart_;
    std::vector<V> data_;
    unsigned int data_size_;
    bool en_seed_;
    unsigned int seed_;
};

template<isDoubleOrFloat V>
inline void AlgorithmExecutor<V>::SetXlsx(std::shared_ptr<DataWriterXLXS> dwXLXS)
{
    dwXLXS_ = dwXLXS;
}

template<isDoubleOrFloat V>
inline void AlgorithmExecutor<V>::SetChart(lxw_chart *chart)
{
    chart_ = chart;
}

template<isDoubleOrFloat V>
inline void AlgorithmExecutor<V>::SetParam(unsigned int data_size, bool en_seed, unsigned int seed)
{
    data_size_ = data_size;
    en_seed_ = en_seed;
    seed_ = seed;
    GenerateData();
}

template<isDoubleOrFloat V>
inline void AlgorithmExecutor<V>::StartSMA(const std::vector<unsigned int> &windows)
{
    long long int timeSum = 0;
    unsigned int start_row = AlgoExecutor::current_row_;
    for(auto &window: windows)
    {
        if(data_.empty() || (data_.size() < window)){return;}

        for(int i = 0; i < NUM_TEST; i++)
        {
            timer_.Start();
            std::vector<V> result = AlgorithmCollection::getSMA(data_, window);
            std::chrono::milliseconds time = timer_.Stop();
            timeSum += data_size_ / time.count();
        }
        double mean_time = timeSum / NUM_TEST;

        AddSMADataToXlsx(window, mean_time, AlgoExecutor::current_row_);
        AlgoExecutor::current_row_++;
    }
    AddSMAChart(start_row, AlgoExecutor::current_row_ - 1);
}

template<isDoubleOrFloat V>
inline void AlgorithmExecutor<V>::GenerateData()
{
    data_.clear();
    std::random_device rd;
    std::mt19937 g;
    if(en_seed_){g.seed(seed_);}
    else{g.seed(rd());}

    data_.resize(data_size_);

    std::uniform_real_distribution<V> num{0, 1000};
    auto gen = [&](){return num(g);};
    std::generate(data_.begin(), data_.end(), gen);
}

template<isDoubleOrFloat V>
inline void AlgorithmExecutor<V>::AddSMADataToXlsx(unsigned int window, double mean_time, unsigned int current_row)
{
    AlgoExecutor::dataType type;
    if constexpr(std::is_same_v<V, float>)
    {
        type = AlgoExecutor::dataType::floatType;
    }
    else
    {
        type = AlgoExecutor::dataType::doubleType;
    }
    dwXLXS_->AddData(AlgoExecutor::NAME_SHEET, AlgoExecutor::dataTypeStr.at(type), XLSX::CoordCell{.row = current_row, .column = 0});
    dwXLXS_->AddData(AlgoExecutor::NAME_SHEET, window, XLSX::CoordCell{.row = current_row, .column = 1});
    dwXLXS_->AddData(AlgoExecutor::NAME_SHEET, mean_time, XLSX::CoordCell{.row = current_row, .column = 2});
}

template<isDoubleOrFloat V>
inline void AlgorithmExecutor<V>::AddSMAChart(unsigned start_row, unsigned int end_row)
{
    AlgoExecutor::dataType type;
    if constexpr(std::is_same_v<V, float>)
    {
        type = AlgoExecutor::dataType::floatType;
    }
    else
    {
        type = AlgoExecutor::dataType::doubleType;
    }

    dwXLXS_->AddDataToChart(AlgoExecutor::NAME_SHEET, chart_, AlgoExecutor::dataTypeStr.at(type),
                            XLSX::CoordRange{.start = XLSX::CoordCell{.row = start_row, .column = 1}, .end = XLSX::CoordCell{.row = end_row, .column = 1}},
                            XLSX::CoordRange{.start = XLSX::CoordCell{.row = start_row, .column = 2}, .end = XLSX::CoordCell{.row = end_row, .column = 2}});
}

#endif /* ALGORITHM_EXECUTOR_H */
