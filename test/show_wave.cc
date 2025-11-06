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

	readData *waveData = new readData(runnum,mid);
	waveData->load();

	int nevt = waveData->getNevt();

	std::vector<short> waveform;
	std::vector<double> pedcor_wave;



	TH1D* h = new TH1D("h","",1000,0,1000);
	TGraph* h2 = new TGraph();
	h->Sumw2();
	int cont=std::numeric_limits<int>::max();
	TCanvas *c = new TCanvas();
	for (int i = 0;i<100;i++){
	//for (int i = 0;i<nevt;i++){
		waveData->getEvt();
		waveform = waveData->readEvt(ch);
		pedcor_wave = pedcorwave(waveform,1000);
		std::vector<int> zerocross = FindZeroCross(pedcor_wave);
		h->Reset();
		h2->Set(0);
		for (int j = 1;j<1001;j++){
			h->Fill(j-1,pedcor_wave.at(j));
		}
		for (auto i : zerocross){
			h2->AddPoint(i,0);
		}
		h->Draw("p");
		h2->Draw("p");
		h2->SetMarkerColor(kRed);
		h->GetYaxis()->SetRangeUser(-2000,2000);
		h->GetXaxis()->SetRangeUser(100,200);
		c->Modified();
                c->Update();
		c->SaveAs(Form("./pngs/run_%d_%d.eps",runnum,i));


	}
}
