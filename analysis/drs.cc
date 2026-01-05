#include <iostream>
#include <filesystem>
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

namespace fs = std::filesystem;

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
	std::vector<int> bin_array;
	std::vector<int> bin_array_rise;
	std::vector<int> bin_array_fall;
	std::vector<int> delta_v;
	std::vector<double> delta_t_rise;
	std::vector<double> delta_t_fall;
	
	fs::path dir = "./roots";
	fs::path time_dir = "../time_calib";
	if (!fs::exists(dir)) {
        	if (fs::create_directories(dir)) {
            		std::cout << "Directory created: " << dir << std::endl;
        	} else {
            		std::cerr << "Failed to create directory: " << dir << std::endl;
        	}
   	} else {
        	std::cout << "Directory already exists: " << dir << std::endl;
    	}

	if (!fs::exists(time_dir)) {
        	if (fs::create_directories(time_dir)) {
            		std::cout << "Directory created: " << time_dir << std::endl;
        	} else {
            		std::cerr << "Failed to create directory: " << time_dir << std::endl;
        	}
   	} else {
        	std::cout << "Directory already exists: " << time_dir << std::endl;
    	}

	TFile *f = new TFile(Form("./roots/drs_time_%d.root",runnum),"recreate");
	TTree *t = new TTree("delta_v","");
	t->Branch("bin_array",&bin_array);
	t->Branch("bin_array_rise",&bin_array_rise);
	t->Branch("bin_array_fall",&bin_array_fall);
	t->Branch("delta_v",&delta_v);
	t->Branch("delta_t_rise",&delta_t_rise);
	t->Branch("delta_t_fall",&delta_t_fall);
	TH1D * h1 = new TH1D ("","",1024,0,1024);
	for (int i = 0; i < nevt; i++){
	//for (int i = 0; i < 100; i++){
		waveData->getEvt();
		waveform = waveData->readEvt(ch);
		drs_stop = waveData->getDrsStop();
		
		int max_wave = *std::max_element(waveform.begin()+1,waveform.begin()+1001);
		int min_wave = *std::min_element(waveform.begin()+1,waveform.begin()+1001);
		float cut_up = (max_wave - min_wave) * 0.7 + min_wave;
		float cut_down = (max_wave - min_wave) * 0.3 + min_wave;
		std::map<int, int> crossbin = FindCross(waveform,cut_up,cut_down);
		int tmp_bin, tmp_flag=-100;
		bin_array.clear();
		bin_array_rise.clear();
		bin_array_fall.clear();
		delta_v.clear();
		delta_t_rise.clear();
		delta_t_fall.clear();
		for (auto iter : crossbin){
		//	if (i == 1818)std::cout<<cut_up<<" | "<<cut_down<<" | "<<iter.first << " | " <<iter.second<<std::endl;
			if (tmp_flag + iter.second == 1){
				for (int j = tmp_bin; j<iter.first; j++){
					
					if (j<1 || j>1001) continue;
					
					int bin;
					if (j + drs_stop.at(drs_num) <1024) bin = j + drs_stop.at(drs_num) ; 
					else bin = j + drs_stop.at(drs_num) - 1024; 
					bin_array.push_back(bin);
					//delta_v.push_back(abs(waveform.at(j)-waveform.at(j+1)));
					//if (abs(waveform.at(j)-waveform.at(j+1))<10) std::cout<<waveform.at(j)<<" | "<<waveform.at(j+1)<<" | "<<drs_stop.at(0)<< " | "<<bin<<" | "<<j<<" | "<<cut_up<<" | "<<cut_down<<" | "<<tmp_flag<<" | "<<iter.second<<std::endl;
					//std::cout<<i<<" | "<<waveform.at(j)<<" | "<<waveform.at(j+1)<<" | "<<drs_stop.at(0)<< " | "<<bin<<" | "<<j<<" | "<<cut_up<<" | "<<cut_down<<" | "<<tmp_flag<<" | "<<iter.second<<std::endl;
					delta_v.push_back(waveform.at(j)-waveform.at(j+1));
					
					// rise
					if (tmp_flag == 1){
						bin_array_rise.push_back(bin);
						int total_delta_v = waveform.at(iter.first + 1) - waveform.at(tmp_bin);
					      	int n_bins = iter.first + 1 - tmp_bin;
						delta_t_rise.push_back((double) (waveform.at(j+1)-waveform.at(j)) * 1000. / 5.12 * n_bins / total_delta_v);
					}
					// fall
					if (tmp_flag == 0){
						bin_array_fall.push_back(bin);
						int total_delta_v = waveform.at(tmp_bin) - waveform.at(iter.first + 1);
					      	int n_bins = iter.first + 1 - tmp_bin;
						delta_t_fall.push_back((double) (waveform.at(j)-waveform.at(j+1)) * 1000. / 5.12 * n_bins / total_delta_v);
					}
				}
			}
			tmp_bin = iter.first;
			tmp_flag = iter.second; 
		}
		tmp_flag = -100;
		t->Fill();
		std::vector<double> corrected_wave = pedcorwave(waveform, 1000);		
		std::vector<int> ZeroCross = FindZeroCross(corrected_wave);
		//TH1F *h = new TH1F(Form("evt_%d",i),"",1024,0,1024);
		//for (int k = 1;k<1001;k++) h->Fill(k-1,corrected_wave.at(k));
		for (int j = 0;j <(int) ZeroCross.size() - 1;j++){
			//h->Fill(ZeroCross.at(j+1) -ZeroCross.at(j) );
			//if(ZeroCross.at(j+1) -ZeroCross.at(j)<50 ){
		//		std::cout<<ZeroCross.at(j+1) <<" | "<<ZeroCross.at(j)<<" evt :"<<i<<std::endl;
		//		h->Write();	
			if(ZeroCross.at(j) + drs_stop.at(drs_num) + 1 < 1024) h1->Fill(ZeroCross.at(j) + drs_stop.at(drs_num) + 1);
			else h1->Fill(ZeroCross.at(j) + drs_stop.at(drs_num) + 1 - 1024);
		//	}
		}
		//h->Write();
	}
	ROOT::RDataFrame df(*t);

	std::map<int, BinStats> stats_rise;
	std::map<int, BinStats> stats_fall;
	
	// rise용 lambda
	auto fillRise = [&stats_rise](const std::vector<int> &bins,
	                              const std::vector<double> &vals) {
	    const auto n = std::min(bins.size(), vals.size());
	    for (std::size_t i = 0; i < n; ++i) {
	        int bin = bins[i];
	        double val = vals[i];
	        auto &s = stats_rise[bin];
	        s.sum   += val;
	        s.count += 1;
	    }
	};
	
	// fall용 lambda
	auto fillFall = [&stats_fall](const std::vector<int> &bins,
	                              const std::vector<double> &vals) {
	    const auto n = std::min(bins.size(), vals.size());
	    for (std::size_t i = 0; i < n; ++i) {
	        int bin = bins[i];
	        double val = vals[i];
	        auto &s = stats_fall[bin];
	        s.sum   += val;
	        s.count += 1;
	    }
	};
	
	// 브랜치 이름을 실제 이름으로 맞춰 넣기
	df.Foreach(fillRise, {"bin_array_rise", "delta_t_rise"});
	df.Foreach(fillFall, {"bin_array_fall", "delta_t_fall"});
	
	std::cout << "[Rise] bin별 mean\n";
	for (const auto &kv : stats_rise) {
	    int bin = kv.first;
	    const auto &s = kv.second;
	    double mean = (s.count > 0) ? (s.sum / s.count) : 0.0;
	    std::cout << "bin = " << bin
	              << "  mean = " << mean
	              << "  (N = " << s.count << ")\n";
	}
	
	std::cout << "\n[Fall] bin별 mean\n";
	for (const auto &kv : stats_fall) {
	    int bin = kv.first;
	    const auto &s = kv.second;
	    double mean = (s.count > 0) ? (s.sum / s.count) : 0.0;
	    std::cout << "bin = " << bin
	              << "  mean = " << mean
	              << "  (N = " << s.count << ")\n";
	}
	std::string filename_out = "../time_calib/MID_"+std::to_string(mid)+"_ch_"+std::to_string(ch)+".txt";
	std::ofstream fout(filename_out);
        if (!fout.is_open()) {
            std::cerr << "cannot open output file\n";
            return 1;
        }
        
        fout << std::fixed << std::setprecision(6);
        
        for (const auto &kv : stats_rise) {
            int bin = kv.first;
        
            // fall에 같은 bin이 없으면 스킵
            if (stats_fall.find(bin) == stats_fall.end())
                continue;
        
            const auto &r = kv.second;
            const auto &f = stats_fall.at(bin);
        
            if (r.count == 0 || f.count == 0)
                continue;
        
            double mean_rise = r.sum / r.count;
            double mean_fall = f.sum / f.count;
            double mean_avg  = 0.5 * (mean_rise + mean_fall);
        
            fout << mean_avg << "\n";
        }
        
        fout.close();

	t->Write();
	h1->Write();
	f->Close();

	return 0;
}
