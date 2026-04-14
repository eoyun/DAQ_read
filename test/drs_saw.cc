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
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "TROOT.h"
#include <map>
#include <fstream>
#include <iomanip>


struct BinStats {
    double sum   = 0.0;
    std::uint64_t count = 0;
};

int main(int argc, char* argv[]){
	int runnum = atoi(argv[1]);
	int mid = atoi(argv[2]);
	int ch = atoi(argv[3]);
	//int drs_num = atoi(argv[4]);
	int drs_num = (int) (ch-1)/8;

	readData *waveData = new readData(runnum,mid);
	waveData->load();
	int nevt = waveData->getNevt();
	std::cout<<nevt<<std::endl;	
	std::vector<short> waveform;
	std::vector<int> drs_stop;
	std::vector<int> bin_array;
	std::vector<int> delta_v;
	TFile *f = new TFile(Form("./roots/drs_saw_time_%d.root",runnum),"recreate");
	TTree *t = new TTree("delta_v","");
	t->Branch("bin_array",&bin_array);
	t->Branch("delta_v",&delta_v);
	TH1D * h1_low_flat[1024];
	TH1D * h1_high_flat[1024];
	TH1D * h1_up[1024];
	TH1D * h1_down[1024];
	for (int k = 0; k< 1024; k++){
		h1_low_flat[k] = new TH1D(Form("%d_bin_low_flat",k),"",200,-100,100);
		h1_high_flat[k] = new TH1D(Form("%d_bin_high_flat",k),"",200,-100,100);
		h1_up[k] = new TH1D(Form("%d_bin_up",k),"",200,-100,100);
		h1_down[k] = new TH1D(Form("%d_bin_down",k),"",200,-100,100);
	}
	for (int i = 0; i < nevt; i++){
		waveData->getEvt();
		waveform = waveData->readEvt(ch);
		drs_stop = waveData->getDrsStop();
		
		int max_wave = *std::max_element(waveform.begin()+1,waveform.begin()+1001);
		int min_wave = *std::min_element(waveform.begin()+1,waveform.begin()+1001);
		float cut_up = (max_wave - min_wave) * 0.7 + min_wave;
		float cut_down = (max_wave - min_wave) * 0.3 + min_wave;
		std::map<int, int> crossbin = FindCross(waveform,cut_up,cut_down);
		int tmp_bin = -1, tmp_flag=-100;
		bin_array.clear();
		delta_v.clear();
		for (auto iter : crossbin){
			if (tmp_flag - iter.second == 1){
				for  (int j = tmp_bin;j<iter.first;j++){
					int bin;
					if (j + drs_stop.at(drs_num) <1024) bin = j + drs_stop.at(drs_num) ; 
					else bin = j + drs_stop.at(drs_num) - 1024; 
					bin_array.push_back(bin);
					delta_v.push_back(waveform.at(j+1) - waveform.at(j));
					h1_up[bin]->Fill(waveform.at(j+1) - waveform.at(j));
				}
			}
			else if (tmp_flag - iter.second == -1){
				for  (int j = tmp_bin;j<iter.first;j++){
					int bin;
					if (j + drs_stop.at(drs_num) <1024) bin = j + drs_stop.at(drs_num) ; 
					else bin = j + drs_stop.at(drs_num) - 1024; 
					bin_array.push_back(bin);
					delta_v.push_back(waveform.at(j+1) - waveform.at(j));
					h1_down[bin]->Fill(waveform.at(j+1) - waveform.at(j));
				}
			}
			else if (tmp_flag + iter.second == 2) {
				for  (int j = tmp_bin+32;j<iter.first-32;j++){
					int bin;
					if (j + drs_stop.at(drs_num) <1024) bin = j + drs_stop.at(drs_num) ; 
					else bin = j + drs_stop.at(drs_num) - 1024; 
					bin_array.push_back(bin);
					delta_v.push_back(waveform.at(j+1) - waveform.at(j));
					h1_high_flat[bin]->Fill(waveform.at(j+1) - waveform.at(j));
				}

				
			}
			else if (tmp_flag + iter.second == 0) {
				for  (int j = tmp_bin+32;j<iter.first-32;j++){
					int bin;
					if (j + drs_stop.at(drs_num) <1024) bin = j + drs_stop.at(drs_num) ; 
					else bin = j + drs_stop.at(drs_num) - 1024; 
					bin_array.push_back(bin);
					delta_v.push_back(waveform.at(j+1) - waveform.at(j));
					h1_low_flat[bin]->Fill(waveform.at(j+1) - waveform.at(j));
				}

				
			}
			tmp_bin = iter.first;
			tmp_flag = iter.second; 
		}
		tmp_flag = -100;
		tmp_bin = -1;
		t->Fill();
	}

	t->Write();
	for (int k = 0; k< 1024; k++){
		h1_low_flat[k]->Write();
		h1_high_flat[k]->Write();
		h1_up[k]->Write();
		h1_down[k]->Write();
	}
	f->Close();

	return 0;
}
