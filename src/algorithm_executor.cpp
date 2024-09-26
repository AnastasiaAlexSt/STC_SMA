#include "algorithm_executor.h"
namespace  AlgoExecutor{

std::shared_ptr<DataWriterXLXS> CreateXlsxFileSMA()
{
    std::vector<std::string>title_columns{AlgoExecutor::COLUMNNAME_TYPE,
                AlgoExecutor::COLUMNNAME_WINDOW,
                AlgoExecutor::COLUMNNAME_TIME+ ", " + AlgoExecutor::NAME_COUNT_TIME};

    std::shared_ptr<DataWriterXLXS> dwXLXS;
    dwXLXS = std::make_shared<DataWriterXLXS>(AlgoExecutor::FILEPATH + "/" + AlgoExecutor::FILENAME);
    dwXLXS->CreateColumnsName(AlgoExecutor::NAME_SHEET, title_columns);
    return dwXLXS;
}

lxw_chart *CreateChart(std::shared_ptr<DataWriterXLXS> dwXLXS)
{
    lxw_chart *chart = dwXLXS->CreateLineChart();
    return chart;
}

void DrawSMAChart(std::shared_ptr<DataWriterXLXS> dwXLXS, lxw_chart *chart)
{
    dwXLXS->SetSmooth(true);
    dwXLXS->AddTitleChart(chart, AlgoExecutor::NAME_CHART);
    dwXLXS->AddAxisXName(chart, AlgoExecutor::COLUMNNAME_TYPE);
    dwXLXS->AddAxisYName(chart, AlgoExecutor::COLUMNNAME_TIME+ ", " + AlgoExecutor::NAME_COUNT_TIME);
    dwXLXS->SetChartStyle(chart);
    dwXLXS->DrawLineChart(AlgoExecutor::NAME_SHEET, chart, XLSX::CoordCell{.row = 0, .column = 5});
}
}











