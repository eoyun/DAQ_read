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

	readData *waveData = new readData(runnum,mid);
	waveData->load();
	int nevt = waveData->getNevt();
	std::cout<<nevt<<std::endl;	
	std::vector<short> waveform;
	std::vector<int> drs_stop;
	std::vector<int> drs_idx;
	std::vector<int> drs_value;
	TFile *f = new TFile("bin_mean.root","recreate");
	TTree *t = new TTree("bin","");
	t->Branch("drs_idx",&drs_idx);
	t->Branch("drs_value",&drs_value);
	//TH1D * h1 = new TH1D ("","",1000,0,1000);
	for (int i = 0; i < nevt; i++){
	//for (int i = 0; i < 100; i++){
		waveData->getEvt();
		waveform = waveData->readEvt(ch);
		drs_stop = waveData->getDrsStop();
		drs_idx.clear();
		drs_value.clear();
		for (int j = 1; j<1001; j++){
			int bin;
			if (j + drs_stop.at(0) + 1<1024) bin = j + drs_stop.at(0) + 1; 
			else bin = j + drs_stop.at(0) + 1 - 1024; 
			drs_idx.push_back(bin);
			drs_value.push_back(waveform.at(j));
			//delta_v.push_back(abs(waveform.at(j)-waveform.at(j+1)));

			//if (abs(waveform.at(j)-waveform.at(j+1))<10) std::cout<<waveform.at(j)<<" | "<<waveform.at(j+1)<<" | "<<drs_stop.at(0)<< " | "<<bin<<" | "<<j<<" | "<<cut_up<<" | "<<cut_down<<" | "<<tmp_flag<<" | "<<iter.second<<std::endl;
		}
		t->Fill();
		//std::vector<double> corrected_wave = pedcorwave(waveform, 1000);		
		//std::vector<int> ZeroCross = FindZeroCross(corrected_wave);
		//TH1F *h = new TH1F(Form("evt_%d",i),"",1000,0,1000);
		//for (int k = 1;k<1001;k++) h->Fill(k-1,corrected_wave.at(k));
		//for (int j = 0;j <(int) ZeroCross.size() - 1;j++){
			//h->Fill(ZeroCross.at(j+1) -ZeroCross.at(j) );
		//	if(ZeroCross.at(j+1) -ZeroCross.at(j)<50 ){
		//		std::cout<<ZeroCross.at(j+1) <<" | "<<ZeroCross.at(j)<<" evt :"<<i<<std::endl;
		//		h->Write();	
		//		h1->Fill(ZeroCross.at(j));
		//	}
		//}
		//h->Write();
	}
	t->Write();
	//h1->Write();
	f->Close();

}
