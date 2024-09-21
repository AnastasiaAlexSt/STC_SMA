#include <iostream>
#include <vector>
#include <numeric>
#include <chrono>
#include <random>
#include <map>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <type_traits>

#include "xlsxwriter.h"

#define COLUMNNAME_TYPE   "dataType"
#define COLUMNNAME_SIZE   "sizeData"
#define COLUMNNAME_WINDOW "window"
#define COLUMNNAME_TIME   "time"

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

    void timerReset()
    {
        timeStart_ = std::chrono::steady_clock::now();
    }
private:
    std::chrono::time_point<std::chrono::steady_clock> timeStart_;
};

template<typename T> requires std::is_same_v<T, double> || std::is_same_v<T, float>
std::vector<T> generateData(unsigned int dataSize = 1000000, bool enSeed = false, unsigned int seed = 10)
{
    std::random_device rd;
    std::mt19937 g;
    if(enSeed){g.seed(seed);}
    else{g.seed(rd());}

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

    return std::move(result);
}

enum class dataType
{
    doubleType = 0,
    floatType,
};

std::map<dataType, std::string> dataTypeStr
{
    {dataType::doubleType, "double"},
    {dataType::floatType, "float"},
};

std::string nameFile;

#define DEL ","

void addDataToCSV(dataType dType, unsigned int size, unsigned int k, double time)
{
    std::ofstream outFile;
    bool exist = std::filesystem::exists(nameFile);
    outFile.open(nameFile, std::ios::app);
    if(!exist)
    {
        outFile << COLUMNNAME_TYPE << DEL << COLUMNNAME_SIZE << DEL << COLUMNNAME_WINDOW << DEL << COLUMNNAME_TIME << "\n";
    }
    if(outFile.is_open())
    {
        outFile << dataTypeStr[dType] << DEL << size << DEL << k << DEL << time << "\n";
    }
    outFile.close();
}
struct CoordCell
{
    unsigned int row;
    unsigned int column;
};

struct CoordRange
{
    CoordCell start;
    CoordCell end;
};

class DataWriterXLXS
{
public:
    DataWriterXLXS(const std::string &filename)
    {
        filename_ = filename;
        workbook_  = workbook_new(filename_.c_str());
    }
    ~DataWriterXLXS()
    {
        workbook_close(workbook_);
    };
    void CreateColumnsName(const std::string &name_sheet, const std::vector<std::string> namecolumns)
    {
        if(!worksheet_.contains(name_sheet))
        {
            worksheet_[name_sheet] = workbook_add_worksheet(workbook_, name_sheet.c_str());
        }
        for(unsigned int i = 0; i < namecolumns.size(); i++)
        {
            worksheet_write_string(worksheet_[name_sheet], 0, i, namecolumns[i].c_str(), NULL);
        }
    };

    template<typename T>
    void AddData(const std::string &name_sheet, T value, CoordCell coord_cell)
    {
        if constexpr(std::is_arithmetic_v<T>)
        {
            worksheet_write_number(worksheet_[name_sheet], coord_cell.row, coord_cell.column, value, NULL);
        }
        else if constexpr(std::is_same_v<T, std::string>)
        {
            worksheet_write_string(worksheet_[name_sheet], coord_cell.row, coord_cell.column, value.c_str(), NULL);
        }
    };

    lxw_chart *CreateLineChart()
    {
        lxw_chart *chart = workbook_add_chart(workbook_, LXW_CHART_LINE);
        return chart;
    };


    void AddDataToChart(const std::string &name_sheet, lxw_chart * chart, const std::string &name_series, CoordRange categories, CoordRange values)
    {
        series_ = chart_add_series(chart, NULL, NULL);

        chart_series_set_categories(series_, name_sheet.c_str(), categories.start.row, categories.start.column, categories.end.row, categories.end.column);
        chart_series_set_values(series_, name_sheet.c_str(), values.start.row, values.start.column, values.end.row, values.end.column);

        chart_series_set_name(series_, name_series.c_str());
    }

    void AddTitleChart(lxw_chart * chart, const std::string &title)
    {
        chart_title_set_name(chart, title.c_str());
    }

    void AddAxisXName(lxw_chart *chart, const std::string &name)
    {
        chart_axis_set_name(chart->x_axis, name.c_str());
    }

    void AddAxisYName(lxw_chart *chart, const std::string &name)
    {
        chart_axis_set_name(chart->y_axis, name.c_str());
    }

    void SetChartStyle(lxw_chart *chart, uint8_t style_id = 10)
    {
        chart_set_style(chart, style_id);
    }

    void SetSmooth(bool isSet)
    {
        if(isSet)
        {
            chart_series_set_smooth(series_, LXW_TRUE);
        }
        else
        {
            chart_series_set_smooth(series_, LXW_FALSE);
        }

    }

    void DrawLineChart(const std::string &name_sheet, lxw_chart * chart, CoordCell place_chart)
    {
        worksheet_insert_chart(worksheet_[name_sheet], place_chart.row, place_chart.column, chart);
    }
private:
    lxw_chart_series *series_;
    lxw_workbook *workbook_;
    std::unordered_map<std::string, lxw_worksheet *> worksheet_;
    std::string filename_;

};


int main()
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<int> dataSize(1000000,2000000);
    dataSize(rd);
    std::vector<float> dataFloat = generateData<float>(10000000, true);

    std::vector<unsigned int> windows{4,8,16,32,64,128};

    Timer timer;
    DataWriterXLXS dwXLXS("resultData_5000000.xlsx");
    dwXLXS.CreateColumnsName("ResultData", std::vector<std::string>{"dataType", "window", "time, counts/sec"});
    unsigned int current_row = 1;
    for(auto &window: windows)
    {
        if(dataFloat.empty() || (dataFloat.size() < window)){return 0;}
        timer.timerReset();
        for(int i = 0; i < 20; i++)
        {
            std::vector<float> result = getSMA(dataFloat, window);
        }
        double time = timer.timerStop();
        dwXLXS.AddData("ResultData", dataTypeStr[dataType::floatType], CoordCell{.row = current_row, .column = 0});
        dwXLXS.AddData("ResultData", window, CoordCell{.row = current_row, .column = 1});
        dwXLXS.AddData("ResultData", time, CoordCell{.row = current_row, .column = 2});
        current_row++;
    }

    std::vector<double> dataDouble = generateData<double>(10000000, true);
    for(auto &window: windows)
    {
        if(dataDouble.empty() || (dataDouble.size() < window)){return 0;}
        timer.timerReset();
        for(int i = 0; i < 20; i++)
        {
            std::vector<double> result = getSMA(dataDouble, window);
        }
        double time = timer.timerStop();
        dwXLXS.AddData("ResultData", dataTypeStr[dataType::doubleType], CoordCell{.row = current_row, .column = 0});
        dwXLXS.AddData("ResultData", window, CoordCell{.row = current_row, .column = 1});
        dwXLXS.AddData("ResultData", time, CoordCell{.row = current_row, .column = 2});
        current_row++;
    }

    lxw_chart *chart = dwXLXS.CreateLineChart();
    dwXLXS.AddDataToChart("ResultData", chart, "double", CoordRange{.start = CoordCell{.row = static_cast<unsigned int>(1 + windows.size()), .column = 1}, .end = CoordCell{.row = static_cast<unsigned int>(1 + 2*windows.size() - 1), .column = 1}},
                                                 CoordRange{.start = CoordCell{.row = static_cast<unsigned int>(1 + windows.size()), .column = 2}, .end = CoordCell{.row = static_cast<unsigned int>(1 + 2*windows.size() - 1), .column = 2}});

    dwXLXS.AddDataToChart("ResultData", chart, "float", CoordRange{.start = CoordCell{.row = 1, .column = 1}, .end = CoordCell{.row = static_cast<unsigned int>((1 + windows.size() - 1)), .column = 1}},
                                                 CoordRange{.start = CoordCell{.row = 1, .column = 2}, .end = CoordCell{.row = static_cast<unsigned int>((1 + windows.size() - 1)), .column = 2}});


    dwXLXS.SetSmooth(true);
    dwXLXS.AddTitleChart(chart, "Result");
    dwXLXS.AddAxisXName(chart, "window");
    dwXLXS.AddAxisYName(chart, "time, counts/sec");
    dwXLXS.SetChartStyle(chart);

    dwXLXS.DrawLineChart("ResultData", chart, CoordCell{.row = 0, .column = 5});

    return 0;
}
