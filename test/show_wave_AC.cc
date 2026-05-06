#include <iostream>
#include <vector>
#include "readData.h"
#include "TTree.h"
#include "TFile.h"
#include "functions.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TGraph.h"
#include "TFitResultPtr.h"
#include "TCanvas.h"
#include <map>

int main(int argc, char* argv[]){
	int runnum = atoi(argv[1]);
	int mid = atoi(argv[2]);
	int ch = atoi(argv[3]);
	int drs_num = (int) (ch-1)/8;

	readData *waveData = new readData(runnum,mid);
	waveData->load();

	int nevt = waveData->getNevt();

	std::vector<short> waveform;
	std::vector<double> pedcor_wave;
	std::vector<double> ACwaveform;
	std::vector<double> ADC_value;
	std::vector<int> drs_stop;



	TH2D* h = new TH2D("h","",1000,0,1000,4000,0,4000);
	TGraph* h2 = new TGraph();
	h->Sumw2();
	int cont=std::numeric_limits<int>::max();
	TCanvas *c = new TCanvas();
	ADC_value = ADC_calib(mid,ch);
	//for (int i = 0;i<100;i++){
	for (int i = 0;i<nevt;i++){
		waveData->getEvt();
		waveform = waveData->readEvt(ch);
		drs_stop = waveData->getDrsStop();
		pedcor_wave = wave_ADC_calib(waveform,drs_stop.at(drs_num),ADC_value);
		//std::vector<int> zerocross = FindZeroCross(pedcor_wave);
		h2->Set(0);
		for (int j = 1;j<1001;j++){
			h->Fill(j-1,pedcor_wave.at(j));
		}
		//for (auto i : zerocross){
		//	h2->AddPoint(i,0);
		//}
		//h->GetXaxis()->SetRangeUser(100,200);
	}
	TFile *f = new TFile("./roots/wave_cor_2D.root","recreate");
	h->Write();
	f->Close();
}
