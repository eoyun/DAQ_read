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
	TFile *f = new TFile(Form("./roots/drs_adc_calib_%d_mid_%d_ch_%d.root",runnum,mid,ch),"recreate");
	TH2D * h1 = new TH2D("hist_bin","",1024,0,1024,4096,0,4096);
	TH2D * h1_diff = new TH2D("hist_diff","",1024,0,1024,500,-250,250);
	TH2D * h1_cor = new TH2D("hist_cor","",1024,0,1024,500,-250,250);
	for (int i = 0; i < nevt; i++){
		waveData->getEvt();
		waveform = waveData->readEvt(ch);
		drs_stop = waveData->getDrsStop();
		
		for  (int j = 1;j<951;j++){
			int bin;
			if (j + drs_stop.at(drs_num) <1024) bin = j + drs_stop.at(drs_num) ; 
			else bin = j + drs_stop.at(drs_num) - 1024; 
			h1_diff->Fill(bin,waveform.at(j+1) - waveform.at(j));
			h1->Fill(bin,waveform.at(j));
			h1_cor->Fill(bin, 3600 - waveform.at(j));
		}
	}

	h1->Write();
	h1_diff->Write();
	h1_cor->Write();
	f->Close();

	return 0;
}
