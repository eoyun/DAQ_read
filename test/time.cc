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
	auto timeFit = [](double *x, double *par){
		return par[0]/(1+exp((par[2]-x[0])/par[1]));
	};
	readData* wave_11 = new readData(11104,8);	
	//readData* wave_8 = new readData(11050,8);	
	wave_11->load();
	//wave_8->load();
	std::vector<short> wave_4_out, wave_6_out;
	TFile *f = new TFile("./roots/time_11104.root","recreate");

	TTree *t = new TTree("time","");
	double time;
	double time_fit;
	double chi2_1st, chi2_2nd;
	t->Branch("time",&time);
	t->Branch("time_fit",&time_fit);
	t->Branch("chi2_1st",&chi2_1st);
	t->Branch("chi2_2nd",&chi2_2nd);
	TH2D* hist_2d_4 = new TH2D("2d_4","",1023,0,1023,4096,-500,3596);
	TH2D* hist_2d_6 = new TH2D("2d_6","",1023,0,1023,4096,-500,3596);
	for (int i=0;i < wave_11->getNevt(); i++){
	//for (int i=0;i < 1000; i++){
		wave_11->getEvt();
		//wave_8->getEvt();
		wave_4_out = wave_11->readEvt(4);
		wave_6_out = wave_11->readEvt(12);
		std::vector<double> pedcor_wave_4_out = pedcorwave(wave_4_out,100);
		std::vector<double> pedcor_wave_6_out = pedcorwave(wave_6_out,100);
		//std::cout<<"#########################"<<std::endl;
		time = getTime(pedcor_wave_4_out,0.4) - getTime(pedcor_wave_6_out,0.4);
		//std::cout<<"#########################"<<std::endl;
		
		TH1D* wave_4_hist = new TH1D(Form("4_%d",i),"",1023,0,1023);
		TH1D* wave_6_hist = new TH1D(Form("6_%d",i),"",1023,0,1023);
		for (int j = 0;j<pedcor_wave_4_out.size();j++){
			wave_4_hist->Fill(j,pedcor_wave_4_out.at(j));
			wave_6_hist->Fill(j,pedcor_wave_6_out.at(j));
			hist_2d_4->Fill(j,pedcor_wave_4_out.at(j));
			hist_2d_6->Fill(j,pedcor_wave_6_out.at(j));
		}
		//std::cout<<getTimeBin(pedcor_wave_4_out,0.2)<<std::endl;;
		double max_4 = *std::max_element(pedcor_wave_4_out.begin()+1,pedcor_wave_4_out.end());
		double max_6 = *std::max_element(pedcor_wave_6_out.begin()+1,pedcor_wave_6_out.end());
		TF1* fit_wave_4 = new TF1("",timeFit,getTimeBin(pedcor_wave_4_out,0.2),getTimeBin(pedcor_wave_4_out,0.8),3);
		TF1* fit_wave_6 = new TF1("",timeFit,getTimeBin(pedcor_wave_6_out,0.2),getTimeBin(pedcor_wave_6_out,0.8),3);
		fit_wave_4->SetParameter(0,max_4);
		fit_wave_4->SetParLimits(0,max_4,max_4);
		fit_wave_4->SetParLimits(2,300,420);
		fit_wave_4->SetParameter(2,400);
		fit_wave_4->SetParameter(1,1);
		fit_wave_4->SetParLimits(1,0,100);
		fit_wave_6->SetParameter(0,max_6);
		fit_wave_6->SetParLimits(0,max_6,max_6);
		fit_wave_6->SetParLimits(2,300,430);
		fit_wave_6->SetParLimits(1,0,100);
		fit_wave_6->SetParameter(2,400);
		fit_wave_6->SetParameter(1,1);
		TFitResultPtr fitResult = wave_4_hist->Fit(fit_wave_4,"QSR+");
		TFitResultPtr fitResult2 = wave_6_hist->Fit(fit_wave_6,"QSR+");
		//std::cout<<time<<std::endl;
		//wave_4_hist->Write();
		//wave_6_hist->Write();
		//fit_wave_4->Write();
		//fit_wave_6->Write();
		time_fit = (fit_wave_4->GetParameter(2) - fit_wave_6->GetParameter(2))*0.2;
		chi2_1st = fit_wave_4->GetChisquare()/fit_wave_4->GetNDF();
		chi2_2nd = fit_wave_6->GetChisquare()/fit_wave_6->GetNDF();
		t->Fill();	
	}
	hist_2d_4->Write();
	hist_2d_6->Write();
	t->Write();
	f->Close();
	return 0;
}
