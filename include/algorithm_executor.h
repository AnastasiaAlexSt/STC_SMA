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
#define NUM_TEST 20
namespace  AlgoExecutor{

static std::string FILEPATH  = "..";
static std::string FILENAME = "result.xlsx";
static std::string COLUMNNAME_TYPE =  "dataType";
static std::string COLUMNNAME_WINDOW = "window";
static std::string COLUMNNAME_TIME = "time";
static std::string NAME_SHEET = "ResultData";
static std::string NAME_COUNT_TIME = "count/ms";
static std::string NAME_CHART = "Result";

static unsigned inline int current_row_ = 1;
enum class dataType
{
    doubleType = 0,
    floatType,
};

static std::unordered_map<dataType, std::string> dataTypeStr
{
    {dataType::doubleType, "double"},
    {dataType::floatType, "float"},
};


static std::shared_ptr<DataWriterXLXS> CreateXlsxFileSMA()
{
    std::vector<std::string>title_columns{AlgoExecutor::COLUMNNAME_TYPE,
                      AlgoExecutor::COLUMNNAME_WINDOW,
                      AlgoExecutor::COLUMNNAME_TIME+ ", " + AlgoExecutor::NAME_COUNT_TIME};

    std::shared_ptr<DataWriterXLXS> dwXLXS;
    dwXLXS = std::make_shared<DataWriterXLXS>(AlgoExecutor::FILEPATH + "/" + AlgoExecutor::FILENAME);
    dwXLXS->CreateColumnsName(AlgoExecutor::NAME_SHEET, title_columns);
    return dwXLXS;
}

static lxw_chart* CreateChart(std::shared_ptr<DataWriterXLXS> dwXLXS)
{
    lxw_chart *chart = dwXLXS->CreateLineChart();
    return chart;
}

static void DrawSMAChart(std::shared_ptr<DataWriterXLXS> dwXLXS, lxw_chart *chart)
{
    dwXLXS->SetSmooth(true);
    dwXLXS->AddTitleChart(chart, AlgoExecutor::NAME_CHART);
    dwXLXS->AddAxisXName(chart, AlgoExecutor::COLUMNNAME_TYPE);
    dwXLXS->AddAxisYName(chart, AlgoExecutor::COLUMNNAME_TIME+ ", " + AlgoExecutor::NAME_COUNT_TIME);
    dwXLXS->SetChartStyle(chart);
    dwXLXS->DrawLineChart(AlgoExecutor::NAME_SHEET, chart, XLSX::CoordCell{.row = 0, .column = 5});
}
};

template <typename  T>
concept isDoubleOrFloat = std::is_same_v<T, double> || std::is_same_v<T, float>;

template <isDoubleOrFloat  V>
class AlgorithmExecutor
{
public:

    AlgorithmExecutor(){;}
     ~AlgorithmExecutor(){;}
     void SetXlsx(std::shared_ptr<DataWriterXLXS> dwXLXS)
     {
         dwXLXS_ = dwXLXS;
     }
     void SetChart(lxw_chart *chart)
     {
         chart_ = chart;
     }
     void SetParam(unsigned int data_size = DEFAULT_DATASIZE, bool en_seed = true, unsigned int seed = 10)
     {
         data_size_ = data_size;
         en_seed_ = en_seed;
         seed_ = seed;
         GenerateData();
     }
     void StartSMA(const std::vector<unsigned int> &windows)
     {
         long long int timeSum = 0;
         unsigned int start_row = AlgoExecutor::current_row_;
         for(auto &window: windows)
         {
             if(data_.empty() || (data_.size() < window)){return;}

             for(int i = 0; i < NUM_TEST; i++)
             {
                 timer_.Start();
                 std::vector<V> result = algo_.getSMA(data_, window);
                 std::chrono::milliseconds time = timer_.Stop();
                 timeSum += data_size_ / time.count();
             }
             long long int mean_time = timeSum / NUM_TEST;

             AddSMADataToXlsx(window, mean_time, AlgoExecutor::current_row_);
             AlgoExecutor::current_row_++;
         }
         AddSMAChart(start_row, AlgoExecutor::current_row_ - 1);
     }
private:
     void GenerateData()
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
     void AddSMADataToXlsx(unsigned int window, double mean_time, unsigned int current_row)
     {
         AlgoExecutor::dataType type;
         if(std::is_same_v<V, float>)
         {
             type = AlgoExecutor::dataType::floatType;
         }
         else
         {
             type = AlgoExecutor::dataType::doubleType;
         }
         dwXLXS_->AddData(AlgoExecutor::NAME_SHEET, AlgoExecutor::dataTypeStr[type], XLSX::CoordCell{.row = current_row, .column = 0});
         dwXLXS_->AddData(AlgoExecutor::NAME_SHEET, window, XLSX::CoordCell{.row = current_row, .column = 1});
         dwXLXS_->AddData(AlgoExecutor::NAME_SHEET, mean_time, XLSX::CoordCell{.row = current_row, .column = 2});
     }
     void AddSMAChart(unsigned start_row, unsigned int end_row)
     {
         AlgoExecutor::dataType type;
         if(std::is_same_v<V, float>)
         {
             type = AlgoExecutor::dataType::floatType;
         }
         else
         {
             type = AlgoExecutor::dataType::doubleType;
         }

         dwXLXS_->AddDataToChart(AlgoExecutor::NAME_SHEET, chart_, AlgoExecutor::dataTypeStr[type],
                                 XLSX::CoordRange{.start = XLSX::CoordCell{.row = start_row, .column = 1}, .end = XLSX::CoordCell{.row = end_row, .column = 1}},
                                 XLSX::CoordRange{.start = XLSX::CoordCell{.row = start_row, .column = 2}, .end = XLSX::CoordCell{.row = end_row, .column = 2}});
     }

    Timer timer_;
    AlgorithmCollection<V> algo_;
    std::vector<std::string> title_columns_;
    std::shared_ptr<DataWriterXLXS> dwXLXS_;
    lxw_chart *chart_;
    std::vector<V> data_;
    unsigned int data_size_;
    bool en_seed_;
    unsigned int seed_;
};

#endif /* ALGORITHM_EXECUTOR_H */
