#include <vector>
#include <numeric>
#include <algorithm>
#include "functions.h"
#include <iostream>
#include <limits>
#include <math.h>
#include <map>
#include <iterator>
#include <cstdio>

double getStd (std::vector<double> &data){
    	if (data.empty()) return 0.0; // 비어있을 경우 0 반환

   	// 평균 계산
    	double mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();

    	// 분산 계산
    	double variance = 0.0;
    	for (auto val : data) {
        	variance += (val - mean) * (val - mean);
    	}
    	variance /= data.size(); // 표본분산이 아니라 모집단 분산 기준

    	// 표준편차 = 분산의 제곱근
    	return std::sqrt(variance);

}

std::vector<int> FindZeroCrossPosSlope (std::vector<double> waveform){
	std::vector<int> zerocrossbin;
	for (int i = 2;i<1001;i++){
		if (waveform.at(i) * waveform.at(i+1) < 0 && waveform.at(i-1) * waveform.at(i+2) < 0 && waveform.at(i) < 0 && waveform.at(i-1) < 0 ) zerocrossbin.push_back(i);
	}
	return zerocrossbin;
}

std::vector<int> FindZeroCross (std::vector<double> waveform){
	std::vector<int> zerocrossbin;
	for (int i = 2;i<1001;i++){
		if (waveform.at(i) == 0) zerocrossbin.push_back(i);
		if (waveform.at(i) * waveform.at(i+1) < 0 && waveform.at(i-1) * waveform.at(i+2) < 0 ) zerocrossbin.push_back(i);
	}
	return zerocrossbin;
}

std::map<int,int> FindCross (std::vector<short> waveform, int cut_up, int cut_down){
	std::map<int, int> crossbin;
	for (int i = 1;i<1001;i++){
		if ((waveform.at(i) - cut_up) * (waveform.at(i+1) - cut_up) <= 0) crossbin.insert({i,0});
		if ((waveform.at(i) - cut_down) * (waveform.at(i+1) - cut_down) <= 0.) crossbin.insert({i,1});
	}
	return crossbin;
}

std::map<double,double> TCwave (std::vector<double> waveform,int MID, int channel, int drs_stop){
	FILE* fp;
	char file_name[200];
	sprintf(file_name,"/u/user/eoyun/DAQ/25.03.27/DAQ_read/time_calib/global_MID_%d_ch_%d_loop_40000.txt",MID,channel);
	fp = fopen(file_name,"rt");
	double val;
	std::vector<double> TC_value;
	std::map<double,double> time_calib_waveform;
	while (fscanf(fp,"%lf",&val)==1){
		TC_value.push_back(val);
	}
	double time_sum=0.0;
	for (int i=0;i<(int)waveform.size();i++){
		int bin;
		if (drs_stop + i  < 1024) bin = i + drs_stop ; 
		else bin = i + drs_stop  - 1024;
		time_calib_waveform.insert({time_sum, waveform.at(i)});
		time_sum += TC_value.at(bin) / 1000.;

	}
	fclose(fp);	
	return time_calib_waveform;



}

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

double getTimewTC (std::map<double,double> pedcorwavewTC, double fraction){
	auto maxIt = std::max_element(
			pedcorwavewTC.begin(), pedcorwavewTC.end(),
			[](const std::pair<double, double>& a, const std::pair<double,double>&b){
				return a.second < b.second;
			}
		);
	//double max_time = maxIt->first;
	double max = maxIt->second;
	double time = std::numeric_limits<double>::max();
	for (auto iter = std::make_reverse_iterator(maxIt); iter != pedcorwavewTC.rend(); ++iter){
		if (iter->second < max * fraction){
			auto prevIt = std::prev(iter);
			time = iter->first + (max * fraction - iter->second)/( prevIt->second - iter->second) * (prevIt->first - iter->first); 
			//std::cout<<idx<<" | "<<(max * fraction - pedcorwave.at(idx))/( pedcorwave.at(idx + 1) - pedcorwave.at(idx))<<std::endl;
			break;
		}
	}
	return time;
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

