#include <vector>
#include <numeric>
#include <algorithm>
#include "functions.h"
#include <iostream>
#include <limits>
#include <math.h>

std::vector<double> pedcorwave (std::vector<short> waveform, int range){
	double ped = (double) std::accumulate(waveform.begin() + 1, waveform.begin() + 1 + range, 0.)/range;
	std::vector<double> pedcor;
	//std::cout<<ped<<std::endl;
	for (int i = 0; i < (int)waveform.size(); i++ )
		pedcor.push_back((double)(ped - waveform.at(i)));
	return pedcor;
}

std::vector<double> fastemul (std::vector<short> waveform, int RE, int width, double fraction){
	double ped=0.;
	double sum=0.;
	double time;
	int min_idx = min_element(waveform.begin()+1,waveform.end()) - waveform.begin();
	int min = *min_element(waveform.begin()+1,waveform.end());
	for (int i = 0; i < width; i++){
		int idx = i + min_idx - RE - width;
		if (idx < 0) idx = waveform.size() + idx;
		ped += (double)waveform.at(idx)/width; 
	}
	double thres = (double)(ped - min) * fraction;
	int flag = 0;
	for (int i = 0; i < width; i++){
		int idx = i + min_idx - RE;
		if (idx < 0) idx = waveform.size() + idx;
		sum += (double)(ped - waveform.at(idx));
		if (flag == 0 && ped - waveform.at(idx) > thres){
			flag = 1;
			time = idx - 1 + (ped - waveform.at(idx - 1) - thres)/(waveform.at(idx) - waveform.at(idx - 1));
			//std::cout<<idx<<" | "<<(ped - waveform.at(idx - 1) - thres)/(waveform.at(idx) - waveform.at(idx - 1))<<std::endl;
		}
	}
	std::vector<double> fastResults;
	fastResults.push_back(sum);
	fastResults.push_back(time * 0.2);
	return fastResults;
}

double getTime (std::vector<double> pedcorwave, double fraction){
	int max_idx = max_element(pedcorwave.begin()+1,pedcorwave.end())-pedcorwave.begin();
	double max = *max_element(pedcorwave.begin()+1,pedcorwave.end());
	double time = std::numeric_limits<double>::max();
	for (int i = 0; i<1000; i++){
		int idx = max_idx - i;
		if (idx < 0) break;
		if (pedcorwave.at(idx) < max * fraction){
			time = idx + (max * fraction - pedcorwave.at(idx))/( pedcorwave.at(idx + 1) - pedcorwave.at(idx)); 
			//std::cout<<idx<<" | "<<(max * fraction - pedcorwave.at(idx))/( pedcorwave.at(idx + 1) - pedcorwave.at(idx))<<std::endl;
			break;
		}
	}
	return time * 0.2;
}

int getTimeBin (std::vector<double> pedcorwave, double fraction){
	int max_idx = max_element(pedcorwave.begin()+1,pedcorwave.end())-pedcorwave.begin();
	double max = *max_element(pedcorwave.begin()+1,pedcorwave.end());
	int time = std::numeric_limits<int>::max();
	for (int i = 0; i<1000; i++){
		int idx = max_idx - i;
		if (idx < 0) break;
		if (pedcorwave.at(idx) < max * fraction){
			time = idx; 
			//std::cout<<idx<<" | "<<(max * fraction - pedcorwave.at(idx))/( pedcorwave.at(idx + 1) - pedcorwave.at(idx))<<std::endl;
			break;
		}
	}
	return time;
}

//double time_fit (double *x, double* par){
//	  int xx = x[0];
//	  double V = par[0]/(1+exp((par[2]-xx)/par[1]));
//	  return V;
//}

