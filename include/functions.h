#include <vector>
#include <numeric>
#include <map>

double getStd (std::vector<double> &data);
std::vector<int> FindZeroCrossPosSlope (std::vector<double> waveform);
std::vector<int> FindZeroCross (std::vector<double> waveform);
std::map<int,int> FindCross (std::vector<short> waveform, int cut_up, int cut_down);
std::vector<double> pedcorwave (std::vector<short> waveform, int range);
std::map<double,double> TCwave (std::vector<double> waveform, int MID, int channel, int drs_stop);

std::vector<double> fastemul (std::vector<short> waveform, int RE, int width, double fraction);

double getTimewTC (std::map<double,double> pedcorwavewTC, double fraction);
double getTime (std::vector<double> pedcorwave, double fraction);
int getTimeBin (std::vector<double> pedcorwave, double fraction);
//double timeFit (double* x, double* par);
