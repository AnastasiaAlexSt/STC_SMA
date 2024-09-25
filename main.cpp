#include <iostream>
#include "algorithm_executor.h"

int main()
{
    std::shared_ptr<DataWriterXLXS> dwXLXS = AlgoExecutor::CreateXlsxFileSMA();
    lxw_chart* chart = AlgoExecutor::CreateChart(dwXLXS);

    std::vector<unsigned int> windows{4,8,16,32,64,128};
    AlgorithmExecutor<double> algDouble;
    algDouble.SetParam();
    algDouble.SetXlsx(dwXLXS);
    algDouble.SetChart(chart);
    algDouble.StartSMA(windows);

    AlgorithmExecutor<float> algFloat;
    algFloat.SetParam();
    algFloat.SetXlsx(dwXLXS);
    algFloat.SetChart(chart);
    algFloat.StartSMA(windows);

    AlgoExecutor::DrawSMAChart(dwXLXS, chart);

    return 0;
}
