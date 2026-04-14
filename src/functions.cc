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

std::vector<int> FindZeroCrossNegSlope(std::vector<double> waveform){
    std::vector<int> zerocrossbin;

    bool armed = (waveform.at(0) > 0);
    int candidate = -1;

    for (int i = 1; i < 951; i++){

        // 이전 iteration에서 만든 candidate를 현재 bin으로 확정/폐기
        if (candidate >= 0) {
            if (waveform.at(i) <= 0) {
                // + -> <=0 이후 한 bin 더 <=0 이면 진짜 crossing으로 확정
                zerocrossbin.push_back(candidate);
                candidate = -1;
                armed = false;
                continue;
            }
            else {
                // 바로 다시 양수면 0 근처 노이즈로 보고 폐기
                candidate = -1;
                armed = true;
            }
        }

        // 양수 영역에 들어가면 다음 negative crossing 허용
        if (waveform.at(i) > 0) {
            armed = true;
        }

        // 정확한 negative-slope zero crossing 후보:
        // [i-1, i] 사이에서 + -> <=0
        if (armed &&
            waveform.at(i-1) > 0 &&
            waveform.at(i) <= 0) {
            candidate = i;
        }
    }

    return zerocrossbin;
}

std::vector<int> FindZeroCrossPosSlope(std::vector<double> waveform){
    std::vector<int> zerocrossbin;

    bool armed = false;
    int candidate = -1;

    // 앞쪽 보정: 0,1,2가 상승일 때만 체크
    if (waveform.at(0) < waveform.at(1) &&
        waveform.at(1) < waveform.at(2)) {

        if (waveform.at(0) < 0 && waveform.at(1) >= 0) {
            // 바로 확정하지 않고 후보로 둠
            candidate = 1;
        }
        else if (waveform.at(1) < 0 && waveform.at(2) >= 0) {
            candidate = 2;
        }
        else if (waveform.at(2) < 0) {
            armed = true;
        }
    }
    else {
        if (waveform.at(0) < 0 || waveform.at(1) < 0 || waveform.at(2) < 0) {
            armed = true;
        }
    }

    // 앞쪽 후보 확정
    if (candidate == 1) {
        if (waveform.at(2) > 0) {
            zerocrossbin.push_back(1);
            armed = false;
        }
        else {
            candidate = -1;
        }
    }
    else if (candidate == 2) {
        if (waveform.at(3) > 0 && waveform.at(4) > 0) {
            zerocrossbin.push_back(2);
            armed = false;
        }
        else {
            candidate = -1;
        }
    }

    for (int i = 2; i < 949; i++){
        // 충분히 음수 상태로 들어가면 다음 crossing 허용
        if (waveform.at(i-1) < 0) {
            armed = true;
        }

        // 후보 생성: 정확한 - -> + bin만 후보
        if (armed &&
            candidate < 0 &&
            waveform.at(i-1) < 0 &&
            waveform.at(i) >= 0) {
            candidate = i;
        }

        // 후보 확정:
        // crossing 뒤 두 샘플도 양수이면 진짜 crossing으로 인정
        if (candidate == i) {
            if (waveform.at(i+1) > 0 && waveform.at(i+2) > 0) {
                zerocrossbin.push_back(i);
                armed = false;
                candidate = -1;
            }
            // 후보 직후 다시 음수로 내려가면 노이즈로 보고 폐기
            else if (waveform.at(i+1) < 0) {
                candidate = -1;
            }
        }
    }

    return zerocrossbin;
}

//std::vector<int> FindZeroCrossPosSlope (std::vector<double> waveform){
//	std::vector<int> zerocrossbin;
//	for (int i = 2;i<951;i++){
//		if (waveform.at(i) * waveform.at(i+1) < 0 && waveform.at(i-1) * waveform.at(i+2) < 0 && waveform.at(i-1) < 0 ) zerocrossbin.push_back(i);
//	}
//	return zerocrossbin;
//}

std::vector<int> FindZeroCross (std::vector<double> waveform){
	std::vector<int> zerocrossbin;
	for (int i = 2;i<951;i++){
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

std::map<int,int> FindCross (std::vector<double> waveform, int cut_up, int cut_down){
	std::map<int, int> crossbin;
	for (int i = 1;i<1001;i++){
		if ((waveform.at(i) - cut_up) * (waveform.at(i+1) - cut_up) <= 0) crossbin.insert({i,0});
		if ((waveform.at(i) - cut_down) * (waveform.at(i+1) - cut_down) <= 0.) crossbin.insert({i,1});
	}
	return crossbin;
}
std::map<double,double> TCwave_saw (std::vector<double> waveform,int MID, int channel, int drs_stop){
	FILE* fp;
	char file_name[200];
	sprintf(file_name,"/u/user/eoyun/DAQ/25.03.27/DAQ_read/time_calib/MID_%d_ch_%d_saw.txt",MID,channel);
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
std::map<double,double> TCwave (std::vector<double> waveform,int MID, int channel, int drs_stop){
	FILE* fp;
	char file_name[200];
	sprintf(file_name,"/u/user/eoyun/DAQ/25.03.27/DAQ_read/time_calib/global_AC_MID_%d_ch_%d.txt",MID,channel);
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

std::vector<double> ADC_calib (int MID,int ch){
	FILE* fp;
	char file_name[200];
	sprintf(file_name,"/u/user/eoyun/DAQ/25.03.27/DAQ_read/ADC_calib/MID_%d_ch_%d_cor.txt",MID,ch);
	std::cout<<file_name<<std::endl;
	fp = fopen(file_name,"rt");
	double val;
	std::vector<double> ADC_value;
	while (fscanf(fp,"%lf",&val)==1){
		ADC_value.push_back(val);
	}
	return ADC_value;
	
}

std::vector<double> wave_ADC_calib (std::vector<short> waveform, int drs_stop, std::vector<double> ADC_value){
	std::vector<double> ADCcorwave;
	for (int i = 0; i<(int) waveform.size();i++){
		int bin;
		if (drs_stop + i  < 1024) bin = i + drs_stop ; 
		else bin = i + drs_stop  - 1024;
		ADCcorwave.push_back(waveform.at(i) + ADC_value.at(bin));
		
		
	}
	return ADCcorwave;
}
std::vector<double> pedcorwave_ADC_calib (std::vector<short> waveform, int range,  int drs_stop, std::vector<double> ADC_value){
	std::vector<double> ADCcorwave;
	for (int i = 0; i<(int)waveform.size();i++){
		int bin;
		if (drs_stop + i  < 1024) bin = i + drs_stop ; 
		else bin = i + drs_stop  - 1024;
		ADCcorwave.push_back(waveform.at(i) + ADC_value.at(bin) );
		
		
	}
	double ped = (double) std::accumulate(ADCcorwave.begin() + 1, ADCcorwave.begin() + 1 + range, 0.)/range;
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

