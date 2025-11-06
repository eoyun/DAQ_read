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

int main(int argc,char* argv[]){
	int runnum = atoi(argv[1]);
	int mid = atoi(argv[2]);
	readData* waveData = new readData(runnum,mid);
	waveData->load();

	std::vector<short> wave[32];
	std::vector<double> pedcor_wave[32];
	
	//int nevt =(waveData->getNevt() > 1000) ? 1000 : waveData->getNevt();
	int nevt = waveData->getNevt();
	double bin[32];
	TFile* f;
	TTree* t;
	f = new TFile(Form("./roots/MID_%d_run_%d_0.root",mid,runnum),"recreate");
	t = new TTree("tree","tree");
	for (int i=0;i<32;i++){
		t->Branch(Form("ch%d",i+1),&bin[i]);
	}
	for (int i=0;i<nevt;i++){
		if (i % 1000 == 0 ){
			t->Write();
			f->Close();
			f = new TFile(Form("./roots/MID_%d_run_%d_%d.root",mid,runnum,i/1000),"recreate");
			t = new TTree("tree","tree");
			for (int i=0;i<32;i++){
				t->Branch(Form("ch%d",i+1),&bin[i]);
			}
			
		
		}
		for (int j=0;j<32;j++){
			wave[j] = waveData->readEvt(j+1);
			pedcor_wave[j]= pedcorwave(wave[j],100);
		}
		waveData->getEvt();
		for (int j=1;j<1001;j++){
			for (int k=0;k<32;k++){
				bin[k] = pedcor_wave[k].at(j);
			}
			t->Fill();
		}
		
	}
	t->Write();
	f->Close();
}
