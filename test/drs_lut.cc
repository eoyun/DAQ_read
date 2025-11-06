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
	TFile *f = new TFile("lut.root","recreate");
	TH1D* h_ped[1000];
	for (int i=0;i<1000;i++){
	
		h_ped[i] = new TH1D (Form("%d_bin",i),"",1000,3000,4000);
	}
	for (int i = 0; i < nevt; i++){
	//for (int i = 0; i < 100; i++){
		waveData->getEvt();
		waveform = waveData->readEvt(ch);
		for (int k = 1;k<1001;k++) h_ped[k-1]->Fill(waveform.at(k));
		//h->Write();
	}
	for (int i=0;i<1000;i++) h_ped[i]->Write();
	f->Close();

}
