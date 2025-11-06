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
	int loops = atoi(argv[4]);

	int drs_num = (ch - 1) / 8;
	readData *waveData = new readData(runnum,mid);
	waveData->load();

	int nevt = waveData->getNevt();

	std::vector<short> waveform;
	std::vector<int> drs_stop;
	std::vector<double> pedcor_wave;
	std::vector<double> time_calib_old;
	std::vector<double> time_calib_new;
	std::vector<double> time_calib_best;
	double std_new;
	double std_best = std::numeric_limits<double>::max();

	FILE* fp;
	char file_name[200];
	sprintf(file_name,"/u/user/eoyun/DAQ/25.03.27/DAQ_read/time_calib/MID_%d_ch_%d.txt",mid,ch);
	fp = fopen(file_name,"rt");
	double val;
	std::vector<double> TC_value;
	TGraph* g = new TGraph();	
	TGraph* g_zero = new TGraph();	
	while (fscanf(fp,"%lf",&val)==1){
		TC_value.push_back(val);
	}
	time_calib_old = TC_value;

	fclose(fp);
	//for (int i = 0;i<10000;i++){
	//for (int i = 0;i<nevt;i++){
	TCanvas *c = new TCanvas();
	for (int i = 0;i<loops;i++){
		waveData->getEvt();
		waveform = waveData->readEvt(ch);
		drs_stop = waveData->getDrsStop();
		int drs_stop_val = drs_stop.at(drs_num);
		pedcor_wave = pedcorwave(waveform,1000);
		g->Set(0);
		g_zero->Set(0);
		time_calib_new = time_calib_old;
		for (int k=1;k<1001;k++){
			double sum = 0.0;
			if (k + drs_stop_val < 1024) sum = std::accumulate(time_calib_old.begin() + drs_stop_val,time_calib_old.begin() + drs_stop_val + k,0.0);
			else sum = std::accumulate(time_calib_old.begin() + drs_stop_val, time_calib_old.end(),0.0) + std::accumulate(time_calib_old.begin(),time_calib_old.begin() + k + drs_stop_val -1024,0.0);
			g->AddPoint(sum/1000.,pedcor_wave.at(k));
			//std::cout<<sum/1000.<<" : "<<k<<" : time bin"<<std::endl;
		}
		std::vector<int> zerocross = FindZeroCrossPosSlope(pedcor_wave);
		for (int j = 0; j<(int)zerocross.size() - 1; j++  ){
			if (zerocross.at(j) < 1 ||zerocross.at(j) > 1001 || zerocross.at(j + 1) > 1001 ) continue;
			double time_sum=0.0;
			int start_bin, finish_bin, bin_diff;
			if (drs_stop_val + zerocross.at(j)   < 1024){ 
				start_bin = zerocross.at(j) + drs_stop_val ;
				time_sum = std::accumulate(time_calib_old.begin() + drs_stop_val,time_calib_old.begin() + start_bin,0.0);
			}	
			else {
				start_bin = zerocross.at(j) + drs_stop_val  - 1024;
				time_sum = std::accumulate(time_calib_old.begin() + drs_stop_val, time_calib_old.end(),0.0) + std::accumulate(time_calib_old.begin(),time_calib_old.begin() + start_bin,0.0);
			
			}
			if (drs_stop_val + zerocross.at(j+1)  < 1024) finish_bin = zerocross.at(j+1) + drs_stop_val ; 
			else finish_bin = zerocross.at(j+1) + drs_stop_val  - 1024;
			
			//std::cout<<start_bin <<" | " <<finish_bin<<std::endl;
			double delta_t_kq = 0.0;
			if (start_bin < finish_bin) {
				bin_diff = finish_bin - start_bin;
				delta_t_kq = std::accumulate(time_calib_old.begin() + start_bin,time_calib_old.begin() + finish_bin,0.0);
			}
			else {
				bin_diff = 1024 - start_bin + finish_bin;
				delta_t_kq = std::accumulate(time_calib_old.begin(),time_calib_old.begin() + finish_bin,0.0 ) + std::accumulate(time_calib_old.begin() + start_bin,time_calib_old.end(),0.0);
			}
			double time_cor =  - abs(pedcor_wave.at(zerocross.at(j))/(pedcor_wave.at(zerocross.at(j)) - pedcor_wave.at(zerocross.at(j)+1) ))*time_calib_old.at(start_bin) + abs(pedcor_wave.at(zerocross.at(j+1))/(pedcor_wave.at(zerocross.at(j + 1)) - pedcor_wave.at(zerocross.at(j+1)+1) ))*time_calib_old.at(finish_bin);
			//std::cout<<bin_diff <<" | bin diff"<<std::endl;			
			time_sum = time_sum + abs(pedcor_wave.at(zerocross.at(j))/(pedcor_wave.at(zerocross.at(j)) - pedcor_wave.at(zerocross.at(j)+1) ))*time_calib_old.at(start_bin);
			g_zero->AddPoint(time_sum/1000.,0);
			// multiplying
			double correction = 10000./(delta_t_kq + time_cor);
			//std::cout<<zerocross.at(j)<<" || "<<correction<<" || "<<delta_t_kq<<" || "<<time_cor<<" || "<<std::accumulate(time_calib_old.begin(),time_calib_old.begin() + finish_bin,0.0 )<<" || "<<std::accumulate(time_calib_old.end() - start_bin,time_calib_old.end(),0.0)<<std::endl;

			if (start_bin < finish_bin) for (int k = start_bin ;k<finish_bin ;k++) time_calib_new.at(k) = correction * time_calib_new.at(k);
			else {
				for (int k = 0;k<finish_bin ;k++) time_calib_new.at(k) = correction * time_calib_new.at(k);
				for (int k = start_bin ;k<(int)time_calib_old.size();k++) time_calib_new.at(k) = correction * time_calib_new.at(k);
			}
			// adding
			//double correction = (5000. - (delta_t_kq + time_cor))/bin_diff;
			//if (start_bin < finish_bin) for (int k = start_bin + 1;k<finish_bin + 1;k++) time_calib_new.at(k) = correction + time_calib_new.at(k);
			//else {
			//	for (int k = 0;k<finish_bin + 1;k++) time_calib_new.at(k) = correction + time_calib_new.at(k);
			//	for (int k = start_bin + 1;k<(int)time_calib_old.size();k++) time_calib_new.at(k) = correction + time_calib_new.at(k);
			//}
			

		}
		//for (int j=0;j<(int)time_calib_old.size();j++){
		//	std::cout<<j<<" | "<<TC_value.at(j)<<" | "<<time_calib_old.at(j) << " | "<<time_calib_new.at(j)<<std::endl;
		//}
		//std_new = getStd(time_calib_new);
		//if (std_new < std_best) {
		//	std_best = std_new;
		//	time_calib_best = time_calib_new;
		//	printf("calib update!! std : %f\n",std_best);
		//}
		//g->Draw("apl");
		//g_zero->Draw("p");
		//g_zero->SetMarkerColor(kRed);
		//c->Modified();
		//c->Update();
		//g->GetXaxis()->SetRangeUser(100,150);
		//g->GetYaxis()->SetRangeUser(-2000,2000);
		//c->SaveAs(Form("./pngs/%d_time_calib_pos_slope_%d.root",runnum,i));
		time_calib_old = time_calib_new;
		//std::cout<<"event end!!"<<std::endl;
		

	}
	sprintf(file_name,"/u/user/eoyun/DAQ/25.03.27/DAQ_read/time_calib/global_MID_%d_ch_%d_loop_%d_pos_slope.txt",mid,ch,loops);
	fp = fopen(file_name,"wt");

	for (int i=0;i<(int)time_calib_new.size();i++){
		fprintf(fp,"%lf\n",time_calib_new.at(i));
	} 
	fclose(fp);
	//t->Write();
	//f->Close();
}
