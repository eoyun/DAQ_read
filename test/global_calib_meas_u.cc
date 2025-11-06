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
#include "TGraph.h"
#include <map>

int main(int argc, char* argv[]){
	int runnum = atoi(argv[1]);
	int mid = atoi(argv[2]);
	int ch = atoi(argv[3]);
	//int loops = atoi(argv[4]);

	int drs_num = (ch - 1) / 8;
	readData *waveData = new readData(runnum,mid);
	waveData->load();

	int nevt = waveData->getNevt();

	std::vector<short> waveform;
	std::vector<int> drs_stop;
	std::vector<double> pedcor_wave;
	double std_new;
	double std_best = std::numeric_limits<double>::max();

	FILE* fp;
	char file_name[200];
	sprintf(file_name,"/u/user/eoyun/DAQ/25.03.27/DAQ_read/time_calib/MID_%d_ch_%d.txt",mid,ch);
	fp = fopen(file_name,"rt");
	double val;
	std::vector<double> TC_value;
	TFile *f = new TFile(Form("./roots/global_calib_%d_mid_%d_ch_%d.root",runnum,mid,ch),"recreate");
	TTree *t = new TTree("tree","tree");
	//TGraph* g = new TGraph();	
	//TGraph* g_zero = new TGraph();	
	while (fscanf(fp,"%lf",&val)==1){
		TC_value.push_back(val);
	}
	double u_cor[1024];
	for (int i = 0;i < 1024; i++){
		t->Branch(Form("%d_bin",i),&u_cor[i]);
	}
	fclose(fp);
	//for (int i = 0;i<10000;i++){
	for (int i = 0;i<nevt;i++){
	//TCanvas *c = new TCanvas();
	//for (int i = 0;i<loops;i++){
		for (int j=0;j<1024;j++) u_cor[j] = std::numeric_limits<double>::max();
		waveData->getEvt();
		waveform = waveData->readEvt(ch);
		drs_stop = waveData->getDrsStop();
		int drs_stop_val = drs_stop.at(drs_num);
		pedcor_wave = pedcorwave(waveform,1000);
		//g->Set(0);
		//g_zero->Set(0);
		//for (int k=1;k<1001;k++){
		//	double sum = 0.0;
		//	if (k + drs_stop_val < 1024) sum = std::accumulate(TC_value.begin() + drs_stop_val,TC_value.begin() + drs_stop_val + k,0.0);
		//	else sum = std::accumulate(TC_value.begin() + drs_stop_val, TC_value.end(),0.0) + std::accumulate(TC_value.begin(),TC_value.begin() + k + drs_stop_val -1024,0.0);
		//	g->AddPoint(sum/1000.,pedcor_wave.at(k));
		//	//std::cout<<sum/1000.<<" : "<<k<<" : time bin"<<std::endl;
		//}
		std::vector<int> zerocross = FindZeroCross(pedcor_wave);
		//if(i == 1718)for (auto k : zerocross) std::cout<<k<<std::endl;
		for (int j = 0; j<(int)zerocross.size() - 2; j=j+2  ){
	
			if (zerocross.at(j) < 1 ||zerocross.at(j) > 1001 || zerocross.at(j + 2) > 1001 ) continue;
			double time_sum=0.0;
			double time_sum_start_bin=0.0;
			int start_bin, finish_bin, bin_diff;
			if (drs_stop_val + zerocross.at(j)   < 1024){ 
				start_bin = zerocross.at(j) + drs_stop_val ;
				time_sum = std::accumulate(TC_value.begin() + drs_stop_val,TC_value.begin() + start_bin,0.0);
			}	
			else {
				start_bin = zerocross.at(j) + drs_stop_val  - 1024;
				time_sum = std::accumulate(TC_value.begin() + drs_stop_val, TC_value.end(),0.0) + std::accumulate(TC_value.begin(),TC_value.begin() + start_bin,0.0);
			
			}
			if (drs_stop_val + zerocross.at(j+2)  < 1024){
			       	finish_bin = zerocross.at(j+2) + drs_stop_val ;
				time_sum_start_bin = std::accumulate(TC_value.begin() + drs_stop_val,TC_value.begin() + finish_bin,0.0);
				
			} 
			else {
				finish_bin = zerocross.at(j+2) + drs_stop_val  - 1024;
				time_sum_start_bin = std::accumulate(TC_value.begin() + drs_stop_val, TC_value.end(),0.0) + std::accumulate(TC_value.begin(),TC_value.begin() + finish_bin,0.0);
			}
			
			//std::cout<<start_bin <<" | " <<finish_bin<<std::endl;
			double delta_t_kq = 0.0;
			if (start_bin < finish_bin) {
				bin_diff = finish_bin - start_bin;
				delta_t_kq = std::accumulate(TC_value.begin() + start_bin,TC_value.begin() + finish_bin,0.0);
			}
			else {
				bin_diff = 1024 - start_bin + finish_bin;
				delta_t_kq = std::accumulate(TC_value.begin(),TC_value.begin() + finish_bin,0.0 ) + std::accumulate(TC_value.begin() + start_bin,TC_value.end(),0.0);
			}
			int finish_pre_bin;
			if (finish_bin == 0) finish_pre_bin = 1023;
			else finish_pre_bin = finish_bin - 1;
			double time_cor =  - abs(pedcor_wave.at(zerocross.at(j))/(pedcor_wave.at(zerocross.at(j)) - pedcor_wave.at(zerocross.at(j)+1) ))*TC_value.at(start_bin) + abs(pedcor_wave.at(zerocross.at(j+2))/(pedcor_wave.at(zerocross.at(j + 2)) - pedcor_wave.at(zerocross.at(j+2)+1) ))*TC_value.at(finish_bin);
			double time_cor_start_bin;// =  abs((pedcor_wave.at(zerocross.at(j+2)) - pedcor_wave.at(zerocross.at(j)))/(pedcor_wave.at(zerocross.at(j + 2)) - pedcor_wave.at(zerocross.at(j+2)+1) ))*TC_value.at(finish_bin);
			//std::cout<<pedcor_wave.at(zerocross.at(j))<<" | "<<pedcor_wave.at(zerocross.at(j+2))<<" | "<<time_cor_start_bin <<" | test"<<std::endl;			
			time_sum = time_sum + abs(pedcor_wave.at(zerocross.at(j))/(pedcor_wave.at(zerocross.at(j)) - pedcor_wave.at(zerocross.at(j)+1) ))*TC_value.at(start_bin);
			if (pedcor_wave.at(zerocross.at(j)) < pedcor_wave.at(zerocross.at(j+2)) && pedcor_wave.at(zerocross.at(j)) > 0){
				time_sum_start_bin = time_sum_start_bin + abs((pedcor_wave.at(zerocross.at(j+2)) - pedcor_wave.at(zerocross.at(j)))/(pedcor_wave.at(zerocross.at(j + 2)) - pedcor_wave.at(zerocross.at(j+2)+1) ))*TC_value.at(finish_bin);
				time_cor_start_bin = abs((pedcor_wave.at(zerocross.at(j+2)) - pedcor_wave.at(zerocross.at(j)))/(pedcor_wave.at(zerocross.at(j + 2)) - pedcor_wave.at(zerocross.at(j+2)+1) ))*TC_value.at(finish_bin);
			}
			else if (pedcor_wave.at(zerocross.at(j)) > pedcor_wave.at(zerocross.at(j+2)) && pedcor_wave.at(zerocross.at(j)) > 0){
				//std::cout<<"hello"<<std::endl;
				time_sum_start_bin = time_sum_start_bin - abs((pedcor_wave.at(zerocross.at(j+2)) - pedcor_wave.at(zerocross.at(j)))/(pedcor_wave.at(zerocross.at(j + 2)-1) - pedcor_wave.at(zerocross.at(j+2)) ))*TC_value.at(finish_pre_bin);
				time_cor_start_bin = - abs((pedcor_wave.at(zerocross.at(j+2)) - pedcor_wave.at(zerocross.at(j)))/(pedcor_wave.at(zerocross.at(j + 2)-1) - pedcor_wave.at(zerocross.at(j+2)) ))*TC_value.at(finish_pre_bin);
		}
			else if (pedcor_wave.at(zerocross.at(j)) < pedcor_wave.at(zerocross.at(j+2)) && pedcor_wave.at(zerocross.at(j)) < 0){
				time_sum_start_bin = time_sum_start_bin - abs((pedcor_wave.at(zerocross.at(j+2)) - pedcor_wave.at(zerocross.at(j)))/(pedcor_wave.at(zerocross.at(j + 2)-1) - pedcor_wave.at(zerocross.at(j+2)) ))*TC_value.at(finish_pre_bin);
				time_cor_start_bin = - abs((pedcor_wave.at(zerocross.at(j+2)) - pedcor_wave.at(zerocross.at(j)))/(pedcor_wave.at(zerocross.at(j + 2)-1) - pedcor_wave.at(zerocross.at(j+2)) ))*TC_value.at(finish_pre_bin);
			}else if (pedcor_wave.at(zerocross.at(j)) > pedcor_wave.at(zerocross.at(j+2)) && pedcor_wave.at(zerocross.at(j)) < 0){
				time_sum_start_bin = time_sum_start_bin + abs((pedcor_wave.at(zerocross.at(j+2)) - pedcor_wave.at(zerocross.at(j)))/(pedcor_wave.at(zerocross.at(j + 2)) - pedcor_wave.at(zerocross.at(j+2)+1) ))*TC_value.at(finish_bin);
				time_cor_start_bin = abs((pedcor_wave.at(zerocross.at(j+2)) - pedcor_wave.at(zerocross.at(j)))/(pedcor_wave.at(zerocross.at(j + 2)) - pedcor_wave.at(zerocross.at(j+2)+1) ))*TC_value.at(finish_bin);
			}
			//g_zero->AddPoint(time_sum_start_bin/1000.,pedcor_wave.at(zerocross.at(j)));
			// multiplying
			//double correction = 10000./(delta_t_kq + time_cor);
			double correction = 10000./(delta_t_kq + time_cor);
			if (time_cor_start_bin >10000) continue;
			//if(correction > 1.01 || correction < 0.99 ||correction == 1){
			//	std::cout<<correction<<" | "<<delta_t_kq<<" | "<<time_cor_start_bin<<" | ################## "<<start_bin<<" | "<<finish_bin<<" | "<<i<<std::endl;
			//	std::cout<<pedcor_wave.at(zerocross.at(j+2)-1)<<" | "<<pedcor_wave.at(zerocross.at(j+2))<<" | "<<pedcor_wave.at(zerocross.at(j+2)+1)<<std::endl;
			//	//g->Draw("apl");
			//	//g_zero->Draw("p");
			//	//g_zero->SetMarkerColor(kRed);
			//	//c->Modified();
			//	//c->Update();
			//	//g->GetXaxis()->SetRangeUser(100,150);
			//	//g->GetYaxis()->SetRangeUser(-2000,2000);
			//	c->SaveAs(Form("./pngs/251013_v1_%d_time_calib_%d.root",runnum,i));
			//}
			double correction_start_bin = 10000./(delta_t_kq + time_cor_start_bin);
			u_cor[start_bin] = correction_start_bin;
			if(start_bin < finish_bin)	for (int k = start_bin + 1;k<finish_bin;k++) u_cor[k] = correction;
			else{	
				for (int k = start_bin + 1;k<1024;k++) u_cor[k] = correction;
				for (int k = 0;k<finish_bin;k++) u_cor[k] = correction;
			}
			//std::cout<<zerocross.at(j)<<" || "<<correction<<" || "<<delta_t_kq<<" || "<<time_cor<<" || "<<std::accumulate(TC_value.begin(),TC_value.begin() + finish_bin,0.0 )<<" || "<<std::accumulate(TC_value.end() - start_bin,TC_value.end(),0.0)<<std::endl;
			// adding
			//double correction = (5000. - (delta_t_kq + time_cor))/bin_diff;
			//if (start_bin < finish_bin) for (int k = start_bin + 1;k<finish_bin + 1;k++) time_calib_new.at(k) = correction + time_calib_new.at(k);
			//else {
			//	for (int k = 0;k<finish_bin + 1;k++) time_calib_new.at(k) = correction + time_calib_new.at(k);
			//	for (int k = start_bin + 1;k<(int)TC_value.size();k++) time_calib_new.at(k) = correction + time_calib_new.at(k);
			//}
			

		}
		//for (int j=0;j<(int)TC_value.size();j++){
	//		std::cout<<j<<" | "<<TC_value.at(j)<<" | "<<TC_value.at(j) << " | "<<time_calib_new.at(j)<<std::endl;
	//	}
		//std_new = getStd(time_calib_new);
		//if (std_new < std_best) {
		//	std_best = std_new;
		//	time_calib_best = time_calib_new;
		//	printf("calib update!! std : %f\n",std_best);
		//}
		//g->Draw("apl");
		//g_zero->Draw("p");
		//g_zero->SetMarkerColor(kRed);
		//g_zero->SetMarkerStyle(21);
		//g->SetMarkerStyle(21);
		//c->Modified();
		//c->Update();
		//g->GetXaxis()->SetRangeUser(100,150);
		//g->GetYaxis()->SetRangeUser(-2000,2000);
		//c->SaveAs(Form("./pngs/251013_v3_%d_time_calib_%d.root",runnum,i));
		//TC_value = time_calib_new;
		//std::cout<<"event end!!"<<std::endl;
		t->Fill();	

	}
	t->Write();
	f->Close();
}
