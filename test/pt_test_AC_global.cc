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

int main(int argc, char* argv[]){
	int runnum = atoi(argv[1]);
	int mid = atoi(argv[2]);
	int ch = atoi(argv[3]);

	int drs_num = (ch - 1) / 8;
	readData *waveData = new readData(runnum,mid);
	waveData->load();

	int nevt = waveData->getNevt();

	std::vector<short> waveform;
	std::vector<int> drs_stop;
	std::vector<double> pedcor_wave;
	std::vector<double> ADC_value;


	FILE* fp;
	char file_name[400];
	sprintf(file_name,"/u/user/eoyun/DAQ/25.03.27/DAQ_read/time_calib/global_AC_MID_%d_ch_%d.txt",mid,ch);
	if (argc > 4){
		sprintf(file_name,"%s",argv[4]);
	}
	fp = fopen(file_name,"rt");
	double val;
	std::vector<double> TC_value;
	while (fscanf(fp,"%lf",&val)==1){
		TC_value.push_back(val);
	}

	fclose(fp);
	char out_file_name[400];
	sprintf(out_file_name,"./roots/AC_global_pt_test_%d.root",runnum);
	if (argc > 5){
		sprintf(out_file_name,"%s",argv[5]);
	}
	TFile* f_root = new TFile(out_file_name,"recreate");
	TH1D* h = new TH1D("h","",1024,0,1024);
	TH2D* h_2d = new TH2D("h2d","",1024,0,1024,2000,9,11);
	h->Sumw2();
	TH1D* h_nor = new TH1D("h_nor","",1024,0,1024);
	h_nor->Sumw2();
	TH1D* h_period = new TH1D("h_period","",2000,9,11);
	h_period->Sumw2();

	ADC_value = ADC_calib(mid,ch);
	//for (int i = 0;i<10;i++){
	for (int i = 0;i<nevt;i++){
		waveData->getEvt();
		waveform = waveData->readEvt(ch);
		drs_stop = waveData->getDrsStop();
		int drs_stop_val = drs_stop.at(drs_num);
		pedcor_wave = pedcorwave_ADC_calib(waveform,950,drs_stop.at(drs_num),ADC_value);


		std::vector<int> zerocross = FindZeroCross(pedcor_wave);
		for (int j = 0; j<(int)zerocross.size() - 2; j = j + 2  ){
			if (pedcor_wave.at(zerocross.at(0))>0 && j==0) j = 1;
			//if (pedcor_wave.at(zerocross.at(0))>0 && j==0) j = 1;
			if (zerocross.at(j) < 1 ||zerocross.at(j) > 951 || zerocross.at(j + 2) > 951 ) continue;
			
			int start_bin, finish_bin;
			if (drs_stop_val + zerocross.at(j)   < 1024) start_bin = zerocross.at(j) + drs_stop_val ; 
			else start_bin = zerocross.at(j) + drs_stop_val  - 1024;
			if (drs_stop_val + zerocross.at(j+2)  < 1024) finish_bin = zerocross.at(j+2) + drs_stop_val ; 
			else finish_bin = zerocross.at(j+2) + drs_stop_val  - 1024;
			
			//std::cout<<start_bin <<" | " <<finish_bin<<" | "<<finish_bin - start_bin<<std::endl;
			double delta_t_kq = 0.0;
			if (start_bin < finish_bin) {
				delta_t_kq = std::accumulate(TC_value.begin() + start_bin,TC_value.begin() + finish_bin,0.0);
			}
			else{
				delta_t_kq = std::accumulate(TC_value.begin(),TC_value.begin() + finish_bin,0.0 ) + std::accumulate(TC_value.begin() + start_bin,TC_value.end(),0.0);
			}
			double time_cor =0.0;
			time_cor =  - abs(pedcor_wave.at(zerocross.at(j))/(pedcor_wave.at(zerocross.at(j)) - pedcor_wave.at(zerocross.at(j)+1) ))*TC_value.at(start_bin) + abs(pedcor_wave.at(zerocross.at(j+2) )/(pedcor_wave.at(zerocross.at(j + 2)) - pedcor_wave.at(zerocross.at(j+2)+1)))*TC_value.at(finish_bin);
			double period_ns = (delta_t_kq + time_cor)/1000.;
			h->Fill(start_bin,period_ns);
			h_2d->Fill(start_bin,period_ns);
			h_nor->Fill(start_bin);
			h_period->Fill(period_ns);
			
			//std::cout<<delta_t_kq/1000. <<" | "<< time_cor/1000.<<std::endl;

		}
		

	}
	h->Divide(h_nor);
	h->Write();
	h_nor->Write();
	h_2d->Write();
	h_period->Write();
	std::cout << "PT_PERIOD_SIGMA_NS " << h_period->GetStdDev() << std::endl;
	f_root->Close();
}
