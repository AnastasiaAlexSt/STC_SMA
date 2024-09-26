#ifndef DATA_WRITER_XLSX_H
#define DATA_WRITER_XLSX_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include "xlsxwriter.h"

namespace XLSX {
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
}

class DataWriterXLXS
{
public:
    DataWriterXLXS(const std::string &filename);
    ~DataWriterXLXS();

    void CreateColumnsName(const std::string &name_sheet, const std::vector<std::string> name_columns);

    template<typename T>
    void AddData(const std::string &name_sheet, T value, XLSX::CoordCell coord_cell)
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

    lxw_chart *CreateLineChart();

    void AddDataToChart(const std::string &name_sheet, lxw_chart * chart, const std::string &name_series, XLSX::CoordRange categories, XLSX::CoordRange values);
    void AddTitleChart(lxw_chart * chart, const std::string &title);
    void AddAxisXName(lxw_chart *chart, const std::string &name);
    void AddAxisYName(lxw_chart *chart, const std::string &name);
    void SetChartStyle(lxw_chart *chart, uint8_t style_id = 10);
    void SetSmooth(bool isSet);
    void DrawLineChart(const std::string &name_sheet, lxw_chart * chart, XLSX::CoordCell place_chart);
private:
    lxw_chart_series *series_;
    lxw_workbook *workbook_;
    std::unordered_map<std::string, lxw_worksheet *> worksheet_;
    std::string filename_;

};
#endif /* DATA_WRITER_XLSX_H */
