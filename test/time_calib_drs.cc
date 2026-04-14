#include <iostream>
#include <vector>
#include "readData.h"
#include "TTree.h"
#include "TFile.h"
#include "functions.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TFitResultPtr.h"
#include "TCanvas.h"
#include <map>
#include <string>

int main(int argc, char* argv[]){
	int runnum = atoi(argv[1]);
	int mid = atoi(argv[2]);
	int ch1 = atoi(argv[3]);
	int ch2 = atoi(argv[4]);
	int ch1_ref = atoi(argv[5]);
	int ch2_ref = atoi(argv[6]);
	std::string tc_file_ch1 = "";
	std::string tc_file_ch2 = "";
	std::string tc_file_ch1_ref = "";
	std::string tc_file_ch2_ref = "";
	std::string output_root = Form("./roots/260414/time_calib_runnum_%d_mid_%d.root",runnum,mid);
	if (argc > 7) tc_file_ch1 = argv[7];
	if (argc > 8) tc_file_ch2 = argv[8];
	if (argc > 9) tc_file_ch1_ref = argv[9];
	if (argc > 10) tc_file_ch2_ref = argv[10];
	if (argc > 11) output_root = argv[11];

	int drs_num1 = (ch1 - 1) / 8;
	int drs_num2 = (ch2 - 1) / 8;
	readData *waveData = new readData(runnum,mid);
	waveData->load();

	int nevt = waveData->getNevt();

	std::vector<short> waveform1;
	std::vector<short> waveform2;
	std::vector<int> drs_stop;
	std::vector<double> pedcor_wave1;
	std::vector<double> pedcor_wave2;
	std::map<double, double> TC_wave1;
	std::map<double, double> TC_wave2;
	std::vector<short> waveform1_ref;
	std::vector<short> waveform2_ref;
	std::vector<double> pedcor_wave1_ref;
	std::vector<double> pedcor_wave2_ref;
	std::map<double, double> TC_wave1_ref;
	std::map<double, double> TC_wave2_ref;
	std::vector<double> ADC_value1;
	std::vector<double> ADC_value2;
	std::vector<double> ADC_value1_ref;
	std::vector<double> ADC_value2_ref;

	TFile* f = new TFile(output_root.c_str(),"recreate");
	TTree *t = new TTree("tree","");

	double time_diff;
	double time_diff_ref;
	double time_diff_woTC;
	double time_diff_applied_ref;
	int drs_stop_val1, drs_stop_val2;
	t->Branch("time_diff",&time_diff);
	t->Branch("time_diff_applied_ref",&time_diff_applied_ref);
	t->Branch("time_diff_ref",&time_diff_ref);
	t->Branch("time_diff_woTC",&time_diff_woTC);
	t->Branch("drs_stop_val1",&drs_stop_val1);
	t->Branch("drs_stop_val2",&drs_stop_val2);
	ADC_value1 = ADC_calib(mid,ch1);
	ADC_value2 = ADC_calib(mid,ch2);
	ADC_value1_ref = ADC_calib(mid,ch1_ref);
	ADC_value2_ref = ADC_calib(mid,ch2_ref);
	for (int i = 0;i<nevt;i++){
	//for (int i = 0;i<10;i++){
		waveData->getEvt();
		waveform1 = waveData->readEvt(ch1);
		waveform2 = waveData->readEvt(ch2);
		waveform1_ref = waveData->readEvt(ch1_ref);
		waveform2_ref = waveData->readEvt(ch2_ref);
		drs_stop = waveData->getDrsStop();
		
		pedcor_wave1 = pedcorwave_ADC_calib(waveform1,100,drs_stop.at(drs_num1),ADC_value1);
		pedcor_wave2 = pedcorwave_ADC_calib(waveform2,100,drs_stop.at(drs_num2),ADC_value2);
		pedcor_wave1_ref = pedcorwave_ADC_calib(waveform1_ref,100,drs_stop.at(drs_num1),ADC_value1);
		pedcor_wave2_ref = pedcorwave_ADC_calib(waveform2_ref,100,drs_stop.at(drs_num2),ADC_value2_ref);

		if (tc_file_ch1.empty()) TC_wave1 = TCwave(pedcor_wave1,mid,ch1,drs_stop.at(drs_num1));
		else TC_wave1 = TCwave(pedcor_wave1,drs_stop.at(drs_num1),tc_file_ch1);
		if (tc_file_ch2.empty()) TC_wave2 = TCwave(pedcor_wave2,mid,ch2,drs_stop.at(drs_num2));
		else TC_wave2 = TCwave(pedcor_wave2,drs_stop.at(drs_num2),tc_file_ch2);
		if (tc_file_ch1_ref.empty()) TC_wave1_ref = TCwave(pedcor_wave1_ref,mid,ch1_ref,drs_stop.at(drs_num1));
		else TC_wave1_ref = TCwave(pedcor_wave1_ref,drs_stop.at(drs_num1),tc_file_ch1_ref);
		if (tc_file_ch2_ref.empty()) TC_wave2_ref = TCwave(pedcor_wave2_ref,mid,ch2_ref,drs_stop.at(drs_num2));
		else TC_wave2_ref = TCwave(pedcor_wave2_ref,drs_stop.at(drs_num2),tc_file_ch2_ref);
		double time_woTC1 = getTime(pedcor_wave1,0.4);
		double time_woTC2 = getTime(pedcor_wave2,0.4);
		double time1 = getTimewTC(TC_wave1,0.4);	
		double time2 = getTimewTC(TC_wave2,0.4);
		double time1_ref = getTimewTC(TC_wave1_ref,0.4);	
		double time2_ref = getTimewTC(TC_wave2_ref,0.4);
		drs_stop_val1 = drs_stop.at(drs_num1);
		drs_stop_val2 = drs_stop.at(drs_num2);
		time_diff = time1 - time2;
		time_diff_ref = time1_ref - time2_ref;
		time_diff_applied_ref = time_diff - time_diff_ref;
		time_diff_woTC = time_woTC1 - time_woTC2;
		t->Fill();
		//std::cout<<nevt<<" | "<<i<<" | "<<time1-time2<<" | "<<time1<<" | "<<time2<<std::endl;	
		//std::cout<<nevt<<" | "<<drs_stop_val1<<" | "<<drs_stop_val2<<std::endl;

		//for (auto iter : TC_wave1){
		//
		//	std::cout<<iter.first<<" | "<<iter.second<<" | "<<time1<<" | "<<*std::max_element(pedcor_wave1.begin()+1,pedcor_wave1.begin()+1001) * 0.4<<std::endl;
		//}	
		

	}
	t->Write();
	f->Close();
}
