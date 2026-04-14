#include <vector>
#include <numeric>
#include <map>

double getStd (std::vector<double> &data);
std::vector<int> FindZeroCrossPosSlope (std::vector<double> waveform);
std::vector<int> FindZeroCrossNegSlope (std::vector<double> waveform);
std::vector<int> FindZeroCross (std::vector<double> waveform);
std::map<int,int> FindCross (std::vector<short> waveform, int cut_up, int cut_down);
std::map<int,int> FindCross (std::vector<double> waveform, int cut_up, int cut_down);
std::vector<double> ADC_calib (int MID,int ch);
std::vector<double> pedcorwave_ADC_calib (std::vector<short> waveform, int range, int drs_stop, std::vector<double> ADC_value);
std::vector<double> wave_ADC_calib (std::vector<short> waveform, int drs_stop, std::vector<double> ADC_value);
std::vector<double> pedcorwave (std::vector<short> waveform, int range);
std::map<double,double> TCwave (std::vector<double> waveform, int MID, int channel, int drs_stop);
std::map<double,double> TCwave_saw (std::vector<double> waveform, int MID, int channel, int drs_stop);

std::vector<double> fastemul (std::vector<short> waveform, int RE, int width, double fraction);

double getTimewTC (std::map<double,double> pedcorwavewTC, double fraction);
double getTime (std::vector<double> pedcorwave, double fraction);
int getTimeBin (std::vector<double> pedcorwave, double fraction);
//double timeFit (double* x, double* par);
