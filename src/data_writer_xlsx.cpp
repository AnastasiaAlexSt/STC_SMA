#include "data_writer_xlsx.h"

DataWriterXLXS::DataWriterXLXS(const std::string &filename)
{
    filename_ = filename;
    workbook_  = workbook_new(filename_.c_str());
}

DataWriterXLXS::~DataWriterXLXS()
{
    workbook_close(workbook_);
}

void DataWriterXLXS::CreateColumnsName(const std::string &name_sheet, const std::vector<std::string> namecolumns)
{
    if(!worksheet_.contains(name_sheet))
    {
        worksheet_[name_sheet] = workbook_add_worksheet(workbook_, name_sheet.c_str());
    }
    for(unsigned int i = 0; i < namecolumns.size(); i++)
    {
        worksheet_write_string(worksheet_[name_sheet], 0, i, namecolumns[i].c_str(), NULL);
    }
}

lxw_chart *DataWriterXLXS::CreateLineChart()
{
    lxw_chart *chart = workbook_add_chart(workbook_, LXW_CHART_LINE);
    return chart;
}

void DataWriterXLXS::AddDataToChart(const std::string &name_sheet, lxw_chart *chart, const std::string &name_series, XLSX::CoordRange categories, XLSX::CoordRange values)
{
    series_ = chart_add_series(chart, NULL, NULL);

    chart_series_set_categories(series_, name_sheet.c_str(), categories.start.row, categories.start.column, categories.end.row, categories.end.column);
    chart_series_set_values(series_, name_sheet.c_str(), values.start.row, values.start.column, values.end.row, values.end.column);

    chart_series_set_name(series_, name_series.c_str());
}

void DataWriterXLXS::AddTitleChart(lxw_chart *chart, const std::string &title)
{
    chart_title_set_name(chart, title.c_str());
}

void DataWriterXLXS::AddAxisXName(lxw_chart *chart, const std::string &name)
{
    chart_axis_set_name(chart->x_axis, name.c_str());
}

void DataWriterXLXS::AddAxisYName(lxw_chart *chart, const std::string &name)
{
    chart_axis_set_name(chart->y_axis, name.c_str());
}

void DataWriterXLXS::SetChartStyle(lxw_chart *chart, uint8_t style_id)
{
    chart_set_style(chart, style_id);
}

void DataWriterXLXS::SetSmooth(bool isSet)
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

void DataWriterXLXS::DrawLineChart(const std::string &name_sheet, lxw_chart *chart, XLSX::CoordCell place_chart)
{
    worksheet_insert_chart(worksheet_[name_sheet], place_chart.row, place_chart.column, chart);
}
