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

int main(){
	readData* waveData_12 = new readData(11121,8);
	readData* waveData_3 = new readData(11122,8);
	waveData_12->load();
	waveData_3->load();
	std::vector<short> wave_3, wave_12;
	TFile* f = new TFile("./roots/intADC_11122.root","recreate");

	TTree *t = new TTree("intADC","");
	double intADC_3;
	double intADC_12;
	t->Branch("intADC_w",&intADC_3);
	t->Branch("intADC_wo",&intADC_12);
	int nevt = (waveData_12->getNevt() > waveData_3->getNevt()) ? waveData_3->getNevt() : waveData_12->getNevt();
	std::cout<<nevt<<std::endl;
	for (int i=0; i < nevt; i++){
		waveData_3->getEvt();
		waveData_12->getEvt();
		wave_3 = waveData_3->readEvt(4);
		wave_12 = waveData_12->readEvt(4);
		std::vector<double> pedcor_wave_3 = pedcorwave(wave_3,100);
		std::vector<double> pedcor_wave_12 = pedcorwave(wave_12,100);

		intADC_3 = std::accumulate(pedcor_wave_3.begin() + 300, pedcor_wave_3.begin()+700,0.);
		intADC_12 = std::accumulate(pedcor_wave_12.begin() + 300, pedcor_wave_12.begin()+700,0.);
		t->Fill();	
	
	}
	std::cout<<"??"<<std::endl;
	t->Write();
	f->Close();
	return 0;
}
