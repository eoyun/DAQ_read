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
	TH1D* h[32];
	TFile* f = new TFile(Form("./roots/MID_%d_run_%d.root",mid,runnum),"recreate");
	for (int i=0;i<32;i++){
		h[i] = new TH1D(Form("%d ch",i+1),"",1000,0,1000);
	}
	std::cout<<nevt<<std::endl;
	for (int i=0;i<nevt;i++){
		waveData->getEvt();
		for (int j=0;j<32;j++){
			wave[j] = waveData->readEvt(j+1);
		}
		for (int j=1;j<1001;j++){
			for (int k=0;k<32;k++){
				h[k]->Fill(j - 1,(double)wave[k].at(j)/nevt);
			}
		}
		
	}
	for (int i=0;i<32;i++){
		h[i]->Write();
	}
	f->Close();
}
