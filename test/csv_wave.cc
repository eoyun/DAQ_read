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

	FILE *fp = fopen("hello.csv","wt");
	//for (int i = 0;i<100;i++){
	for (int i = 0;i<nevt;i++){
		waveData->getEvt();
		waveform = waveData->readEvt(ch);
		for (int j = 1;j<1000;j++){
			fprintf(fp,"%d,",waveform.at(j));
		}
		fprintf(fp,"%d\n",waveform.at(1001));


	}
}
