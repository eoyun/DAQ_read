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
	
	int nevt = waveData->getNevt();
	TCanvas *c = new TCanvas();

	for (int i=0;i<nevt;i++){
		TH1D* h[32];
		for (int j=0;j<32;j++){
			h[j] = new TH1D(Form("wave%d_%d",i,j),Form("wave%d",i),1000,0,1000);
			wave[j] = waveData->readEvt(j+1);
		}
		waveData->getEvt();
		for (int j=1;j<1001;j++){
			for (int k=0;k<32;k++){
				h[k]->Fill(j-1,wave[k].at(j));
			}
		}
		for (int j=0;j<32;j++){
			if (j==0) {
				h[j]->Draw("hist");
				h[j]->GetYaxis()->SetRangeUser(3300,3800);
			}
			else h[j]->Draw("same&hist");
			if (j<8) h[j]->SetLineColor(kBlack);
			else if (j<16) h[j]->SetLineColor(kBlue);
			else if (j<24) h[j]->SetLineColor(kRed);
			else h[j]->SetLineColor(kMagenta);
		}
		c->SaveAs(Form("./test_10572/%d_%d.png",mid,i));
	}
}
