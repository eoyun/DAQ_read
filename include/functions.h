#include <vector>
#include <numeric>

std::vector<double> pedcorwave (std::vector<short> waveform, int range);

std::vector<double> fastemul (std::vector<short> waveform, int RE, int width, double fraction);

double getTime (std::vector<double> pedcorwave, double fraction);
int getTimeBin (std::vector<double> pedcorwave, double fraction);
//double timeFit (double* x, double* par);
