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
	readData* waveData_800 = new readData(11066,11);
	readData* waveData_850 = new readData(11067,11);
	readData* waveData_900 = new readData(11068,11);

	waveData_800->load();
	waveData_850->load();
	waveData_900->load();

	std::vector<short> wave_800_NT, wave_800_ET;
	std::vector<short> wave_850_NT, wave_850_ET;
	std::vector<short> wave_900_NT, wave_900_ET;

	TFile *f = new TFile("./roots/scint_trig.root","recreate");

	TTree* t = new TTree("scint","");
	double time_800, time_850, time_900;
	double intADC_800_NT, intADC_800_ET;
	double intADC_850_NT, intADC_850_ET;
	double intADC_900_NT, intADC_900_ET;
	
	t->Branch("time_800",&time_800);
	t->Branch("time_850",&time_850);
	t->Branch("time_900",&time_900);

	t->Branch("intADC_800_NT",&intADC_800_NT);
	t->Branch("intADC_850_NT",&intADC_850_NT);
	t->Branch("intADC_900_NT",&intADC_900_NT);
	t->Branch("intADC_800_ET",&intADC_800_ET);
	t->Branch("intADC_850_ET",&intADC_850_ET);
	t->Branch("intADC_900_ET",&intADC_900_ET);
	
	for (int i = 0;i<waveData_900->getNevt();i++){
		waveData_800->getEvt();
		waveData_850->getEvt();
		waveData_900->getEvt();

		wave_800_NT = waveData_800->readEvt(4);
		wave_800_ET = waveData_800->readEvt(6);
		wave_850_NT = waveData_850->readEvt(4);
		wave_850_ET = waveData_850->readEvt(6);
		wave_900_NT = waveData_900->readEvt(4);
		wave_900_ET = waveData_900->readEvt(6);
		
		std::vector<double> pedcor_wave_800_NT = pedcorwave(wave_800_NT,100);
		std::vector<double> pedcor_wave_800_ET = pedcorwave(wave_800_ET,100);
		std::vector<double> pedcor_wave_850_NT = pedcorwave(wave_850_NT,100);
		std::vector<double> pedcor_wave_850_ET = pedcorwave(wave_850_ET,100);
		std::vector<double> pedcor_wave_900_NT = pedcorwave(wave_900_NT,100);
		std::vector<double> pedcor_wave_900_ET = pedcorwave(wave_900_ET,100);
		
		intADC_800_NT = std::accumulate(pedcor_wave_800_NT.begin()+150,pedcor_wave_800_NT.begin()+400,0.);
		intADC_800_ET = std::accumulate(pedcor_wave_800_ET.begin()+150,pedcor_wave_800_ET.begin()+400,0.);
		intADC_850_NT = std::accumulate(pedcor_wave_850_NT.begin()+150,pedcor_wave_850_NT.begin()+400,0.);
		intADC_850_ET = std::accumulate(pedcor_wave_850_ET.begin()+150,pedcor_wave_850_ET.begin()+400,0.);
		intADC_900_NT = std::accumulate(pedcor_wave_900_NT.begin()+150,pedcor_wave_900_NT.begin()+400,0.);
		intADC_900_ET = std::accumulate(pedcor_wave_900_ET.begin()+150,pedcor_wave_900_ET.begin()+400,0.);

		time_800 = getTime(pedcor_wave_800_NT,0.4) - getTime(pedcor_wave_800_ET,0.4);
		time_850 = getTime(pedcor_wave_850_NT,0.4) - getTime(pedcor_wave_850_ET,0.4);
		time_900 = getTime(pedcor_wave_900_NT,0.4) - getTime(pedcor_wave_900_ET,0.4);
		
		t->Fill();
	}
	t->Write();
	f->Close();
}
