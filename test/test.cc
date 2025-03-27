#include "readData.h"
#include <vector>
#include "functions.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"

int main( int argc, char* argv[]){

	TFile* f = new TFile("test_11051.root","recreate");
	readData* fast_11 = new readData(11051,11);
	readData* wave_11 = new readData(11051,11);
	fast_11->setFast();
	fast_11->load();
	wave_11->load();
	std::vector<short> wave_4_out;
	std::vector<short> wave_6_out;
	std::vector<short> fast_4_out;
	std::vector<short> fast_6_out;
	std::vector<double> wave_4_fastemul;
	std::vector<double> wave_6_fastemul;
	TTree *t = new TTree("fast_wave_comp","");
	double wave_intADC, fast_intADC;
	double wave_time, fast_time;
	t->Branch("wave_intADC",&wave_intADC);
	t->Branch("fast_intADC",&fast_intADC);
	t->Branch("wave_time",&wave_time);
	t->Branch("fast_time",&fast_time);
	for (int i=0; i < fast_11->getNevt();i++){
		wave_11->getEvt();
		fast_11->getEvt();
		wave_4_out = wave_11->readEvt(4);	
		wave_6_out = wave_11->readEvt(12);	
		fast_4_out = fast_11->readEvt(4);	
		fast_6_out = fast_11->readEvt(12);
		TH1D* h = new TH1D(Form("plot%d",i),"",1023,0,1023);
		for (int j=0; j<(int)wave_4_out.size();j++) h->Fill(j,wave_4_out.at(j));	
		wave_4_fastemul = fastemul(wave_4_out,110,210,0.4);	
		wave_6_fastemul = fastemul(wave_6_out,110,210,0.4);
		fast_intADC = (double)(fast_4_out.at(0) + 65536 * fast_4_out.at(1));
		fast_time = (double)(fast_4_out.at(2) - fast_6_out.at(2))*0.0125;
		wave_intADC = wave_4_fastemul.at(0);
		wave_time = wave_4_fastemul.at(1) - wave_6_fastemul.at(1);
		//std::cout << wave_4_fastemul.at(1) <<" | "<<wave_6_fastemul.at(1) <<" || "<<fast_4_out.at(2)*0.0125<<" | "<<fast_6_out.at(2)*0.0125<<" || "<<wave_time<<" | "<<fast_time<<std::endl;
		t->Fill();
		//h->Write();	
	}
	t->Write();
	f->Close();
	
	return 0;
}
