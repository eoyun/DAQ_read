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
	long long trg_time;
	TFile* f = new TFile(Form("./roots/noise_MID_%d_run_%d.root",mid,runnum),"recreate");
	TTree* t = new TTree("tree","tree");
	for (int i=0;i<32;i++){
		t->Branch(Form("ch%d_noise",i+1),&bin[i]);
	}
	t->Branch("trg_time",&trg_time);
	for (int i=0;i<nevt;i++){
		waveData->getEvt();
		for (int j=0;j<32;j++){
			wave[j] = waveData->readEvt(j+1);
			int sum = std::accumulate(wave[j].begin()+1,wave[j].begin()+1001,0);
			double mean = (double) sum / 1000.;
			double sq_sum = std::inner_product(wave[j].begin() + 1,wave[j].begin() + 1001,wave[j].begin() + 1,0.0);
			bin[j] = std::sqrt(sq_sum / 1000. - mean * mean); 
			//std::cout<<bin[j] <<" : std of ch " <<j<< " | sq_sum : "<<sq_sum<<" | mean : "<<mean<<std::endl;
		}
		trg_time = waveData->getTrigTime();
		t->Fill();
		
	}
	t->Write();
	f->Close();
}
