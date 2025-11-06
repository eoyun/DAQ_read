#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include "readData.h"
#include "TTree.h"
#include "TFile.h"
#include "functions.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TFitResultPtr.h"
#include "TCanvas.h"
#include <map>
#include <algorithm>

class TimeCalibrator {
private:
    int mid, runnum;
    std::vector<std::vector<double>> local_calib_constants;
    std::vector<double> global_calib_constants;
    bool use_global_calib;
    
public:
    TimeCalibrator(int mid_val, int runnum_val) : mid(mid_val), runnum(runnum_val), use_global_calib(false) {
        local_calib_constants.resize(32, std::vector<double>(1024, 200.0));
        global_calib_constants.resize(1024, 200.0);
    }
    
    // Load local calibration constants for each channel
    bool loadLocalCalibration() {
        std::cout << "Loading local calibration constants..." << std::endl;
        bool success = true;
        
        for (int ch = 1; ch <= 32; ch++) {
            char filename[300];
            sprintf(filename, "/u/user/eoyun/DAQ/25.03.27/DAQ_read/time_calib/MID_%d_ch_%d.txt", mid, ch);
            
            FILE* fp = fopen(filename, "rt");
            if (!fp) {
                std::cout << "Warning: Cannot open " << filename << ", using default values" << std::endl;
                continue;
            }
            
            double val;
            int bin = 0;
            while (fscanf(fp, "%lf", &val) == 1 && bin < 1024) {
                local_calib_constants[ch-1][bin] = val;
                bin++;
            }
            fclose(fp);
            
            if (bin != 1024) {
                std::cout << "Warning: Channel " << ch << " has " << bin << " values instead of 1024" << std::endl;
            }
        }
        
        return success;
    }
    
    // Generate global calibration constants by averaging across channels
    void generateGlobalCalibration() {
        std::cout << "Generating global calibration constants..." << std::endl;
        
        for (int bin = 0; bin < 1024; bin++) {
            double sum = 0.0;
            int count = 0;
            
            for (int ch = 0; ch < 32; ch++) {
                if (local_calib_constants[ch][bin] > 0) {
                    sum += local_calib_constants[ch][bin];
                    count++;
                }
            }
            
            if (count > 0) {
                global_calib_constants[bin] = sum / count;
            } else {
                global_calib_constants[bin] = 200.0; // Default value
            }
        }
    }
    
    // Save calibration constants to file
    void saveCalibration(const std::string& suffix = "") {
        // Save local calibration
        for (int ch = 1; ch <= 32; ch++) {
            char filename[300];
            sprintf(filename, "/u/user/eoyun/DAQ/25.03.27/DAQ_read/time_calib/MID_%d_ch_%d%s.txt", 
                   mid, ch, suffix.c_str());
            
            std::ofstream file(filename);
            if (file.is_open()) {
                for (int bin = 0; bin < 1024; bin++) {
                    file << std::fixed << std::setprecision(3) << local_calib_constants[ch-1][bin] << std::endl;
                }
                file.close();
            }
        }
        
        // Save global calibration
        char global_filename[300];
        sprintf(global_filename, "/u/user/eoyun/DAQ/25.03.27/DAQ_read/time_calib/MID_%d_global%s.txt", 
               mid, suffix.c_str());
        
        std::ofstream global_file(global_filename);
        if (global_file.is_open()) {
            for (int bin = 0; bin < 1024; bin++) {
                global_file << std::fixed << std::setprecision(3) << global_calib_constants[bin] << std::endl;
            }
            global_file.close();
            std::cout << "Global calibration saved to: " << global_filename << std::endl;
        }
    }
    
    // Apply time calibration with choice of local or global
    std::map<double,double> applyCalibration(const std::vector<double>& waveform, int channel, 
                                           int drs_stop, bool use_global = false) {
        std::map<double,double> time_calib_waveform;
        double time_sum = 0.0;
        
        const std::vector<double>& calib_constants = use_global ? global_calib_constants : 
                                                                 local_calib_constants[channel-1];
        
        for (int i = 0; i < (int)waveform.size(); i++) {
            int bin;
            if (drs_stop + i < 1024) {
                bin = i + drs_stop;
            } else {
                bin = i + drs_stop - 1024;
            }
            
            time_sum += calib_constants[bin] / 1000.0; // Convert ps to ns
            time_calib_waveform.insert({time_sum, waveform[i]});
        }
        
        return time_calib_waveform;
    }
    
    // Iterative calibration improvement using zero crossings
    void improveCalibration(readData* waveData, int channel, int max_iterations = 5) {
        std::cout << "Improving calibration for channel " << channel << "..." << std::endl;
        
        int drs_num = (channel - 1) / 8;
        int nevt = std::min(waveData->getNevt(), 1000); // Limit events for speed
        
        for (int iter = 0; iter < max_iterations; iter++) {
            std::cout << "Iteration " << iter + 1 << "/" << max_iterations << std::endl;
            
            std::vector<double> correction_factors(1024, 1.0);
            std::vector<int> bin_counts(1024, 0);
            
            for (int evt = 0; evt < nevt; evt++) {
                waveData->getEvt();
                std::vector<short> waveform = waveData->readEvt(channel);
                std::vector<int> drs_stop = waveData->getDrsStop();
                std::vector<double> pedcor_wave = pedcorwave(waveform, 100);
                
                int drs_stop_val = drs_stop[drs_num];
                
                // Find zero crossings
                std::vector<int> zerocross = FindZeroCross(pedcor_wave);
                
                for (int j = 0; j < (int)zerocross.size() - 1; j++) {
                    if (zerocross[j] < 1 || zerocross[j] > 1001 || zerocross[j+1] > 1001) continue;
                    
                    int start_bin, finish_bin;
                    if (drs_stop_val + zerocross[j] + 1 < 1024) {
                        start_bin = zerocross[j] + drs_stop_val + 1;
                    } else {
                        start_bin = zerocross[j] + drs_stop_val + 1 - 1024;
                    }
                    
                    if (drs_stop_val + zerocross[j+1] + 1 < 1024) {
                        finish_bin = zerocross[j+1] + drs_stop_val + 1;
                    } else {
                        finish_bin = zerocross[j+1] + drs_stop_val + 1 - 1024;
                    }
                    
                    // Calculate expected time interval (5 ns for 1 GHz sampling)
                    double expected_time = 5.0; // ns
                    
                    // Calculate actual time with current calibration
                    double actual_time = 0.0;
                    if (start_bin < finish_bin) {
                        for (int k = start_bin; k < finish_bin; k++) {
                            actual_time += local_calib_constants[channel-1][k] / 1000.0;
                        }
                    } else {
                        for (int k = 0; k < finish_bin; k++) {
                            actual_time += local_calib_constants[channel-1][k] / 1000.0;
                        }
                        for (int k = start_bin; k < 1024; k++) {
                            actual_time += local_calib_constants[channel-1][k] / 1000.0;
                        }
                    }
                    
                    // Calculate correction factor
                    if (actual_time > 0) {
                        double correction = expected_time / actual_time;
                        
                        // Apply correction to bins
                        if (start_bin < finish_bin) {
                            for (int k = start_bin; k < finish_bin; k++) {
                                correction_factors[k] += correction;
                                bin_counts[k]++;
                            }
                        } else {
                            for (int k = 0; k < finish_bin; k++) {
                                correction_factors[k] += correction;
                                bin_counts[k]++;
                            }
                            for (int k = start_bin; k < 1024; k++) {
                                correction_factors[k] += correction;
                                bin_counts[k]++;
                            }
                        }
                    }
                }
            }
            
            // Apply averaged corrections
            for (int bin = 0; bin < 1024; bin++) {
                if (bin_counts[bin] > 0) {
                    double avg_correction = correction_factors[bin] / bin_counts[bin];
                    local_calib_constants[channel-1][bin] *= avg_correction;
                }
            }
        }
        
        std::cout << "Calibration improvement completed for channel " << channel << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cout << "Usage: " << argv[0] << " <runnum> <mid> <ch1> <ch2> [mode]" << std::endl;
        std::cout << "Mode: 0=local only, 1=global only, 2=both (default=2)" << std::endl;
        return 1;
    }
    
    int runnum = atoi(argv[1]);
    int mid = atoi(argv[2]);
    int ch1 = atoi(argv[3]);
    int ch2 = atoi(argv[4]);
    int mode = (argc > 5) ? atoi(argv[5]) : 2;
    
    std::cout << "Enhanced Time Calibration Analysis" << std::endl;
    std::cout << "Run: " << runnum << ", MID: " << mid << ", Channels: " << ch1 << " & " << ch2 << std::endl;
    
    // Initialize calibrator and data reader
    TimeCalibrator calibrator(mid, runnum);
    readData* waveData = new readData(runnum, mid);
    waveData->load();
    
    // Load existing calibrations
    calibrator.loadLocalCalibration();
    calibrator.generateGlobalCalibration();
    
    // Improve calibrations if requested
    if (mode >= 1) {
        calibrator.improveCalibration(waveData, ch1);
        calibrator.improveCalibration(waveData, ch2);
        calibrator.generateGlobalCalibration(); // Regenerate after improvement
    }
    
    // Save improved calibrations
    calibrator.saveCalibration("_enhanced");
    
    // Create ROOT output file
    TFile* f = new TFile(Form("./roots/time_calib_enhanced_run_%d_mid_%d_ch_%d_%d.root", 
                             runnum, mid, ch1, ch2), "recreate");
    TTree* t = new TTree("calib_tree", "Enhanced Time Calibration Tree");
    
    // Branch variables
    double time_diff_local, time_diff_global, time_diff_uncalib;
    double time1_local, time1_global, time1_uncalib;
    double time2_local, time2_global, time2_uncalib;
    int drs_stop1, drs_stop2;
    
    t->Branch("time_diff_local", &time_diff_local);
    t->Branch("time_diff_global", &time_diff_global);
    t->Branch("time_diff_uncalib", &time_diff_uncalib);
    t->Branch("time1_local", &time1_local);
    t->Branch("time1_global", &time1_global);
    t->Branch("time1_uncalib", &time1_uncalib);
    t->Branch("time2_local", &time2_local);
    t->Branch("time2_global", &time2_global);
    t->Branch("time2_uncalib", &time2_uncalib);
    t->Branch("drs_stop1", &drs_stop1);
    t->Branch("drs_stop2", &drs_stop2);
    
    // Analysis histograms
    TH1D* h_diff_local = new TH1D("h_diff_local", "Time Diff Local Calib;Time [ns];Events", 200, -10, 10);
    TH1D* h_diff_global = new TH1D("h_diff_global", "Time Diff Global Calib;Time [ns];Events", 200, -10, 10);
    TH1D* h_diff_uncalib = new TH1D("h_diff_uncalib", "Time Diff Uncalibrated;Time [ns];Events", 200, -10, 10);
    
    int drs_num1 = (ch1 - 1) / 8;
    int drs_num2 = (ch2 - 1) / 8;
    int nevt = std::min(waveData->getNevt(), 10000); // Limit for analysis
    
    std::cout << "Analyzing " << nevt << " events..." << std::endl;
    
    for (int i = 0; i < nevt; i++) {
        if (i % 1000 == 0) {
            std::cout << "Processing event " << i << "/" << nevt << std::endl;
        }
        
        waveData->getEvt();
        
        // Read waveforms
        std::vector<short> waveform1 = waveData->readEvt(ch1);
        std::vector<short> waveform2 = waveData->readEvt(ch2);
        std::vector<int> drs_stop = waveData->getDrsStop();
        
        // Pedestal correction
        std::vector<double> pedcor_wave1 = pedcorwave(waveform1, 100);
        std::vector<double> pedcor_wave2 = pedcorwave(waveform2, 100);
        
        drs_stop1 = drs_stop[drs_num1];
        drs_stop2 = drs_stop[drs_num2];
        
        // Apply different calibrations
        std::map<double,double> TC_wave1_local = calibrator.applyCalibration(pedcor_wave1, ch1, drs_stop1, false);
        std::map<double,double> TC_wave1_global = calibrator.applyCalibration(pedcor_wave1, ch1, drs_stop1, true);
        std::map<double,double> TC_wave2_local = calibrator.applyCalibration(pedcor_wave2, ch2, drs_stop2, false);
        std::map<double,double> TC_wave2_global = calibrator.applyCalibration(pedcor_wave2, ch2, drs_stop2, true);
        
        // Calculate timing with different methods
        time1_local = getTimewTC(TC_wave1_local, 0.4);
        time1_global = getTimewTC(TC_wave1_global, 0.4);
        time1_uncalib = getTime(pedcor_wave1, 0.4);
        
        time2_local = getTimewTC(TC_wave2_local, 0.4);
        time2_global = getTimewTC(TC_wave2_global, 0.4);
        time2_uncalib = getTime(pedcor_wave2, 0.4);
        
        // Calculate time differences
        time_diff_local = time1_local - time2_local;
        time_diff_global = time1_global - time2_global;
        time_diff_uncalib = time1_uncalib - time2_uncalib;
        
        // Fill histograms
        h_diff_local->Fill(time_diff_local);
        h_diff_global->Fill(time_diff_global);
        h_diff_uncalib->Fill(time_diff_uncalib);
        
        t->Fill();
    }
    
    // Write results
    t->Write();
    h_diff_local->Write();
    h_diff_global->Write();
    h_diff_uncalib->Write();
    
    // Print statistics
    std::cout << "\nCalibration Comparison Results:" << std::endl;
    std::cout << "Local Calibration  - RMS: " << h_diff_local->GetRMS() << " ns" << std::endl;
    std::cout << "Global Calibration - RMS: " << h_diff_global->GetRMS() << " ns" << std::endl;
    std::cout << "No Calibration     - RMS: " << h_diff_uncalib->GetRMS() << " ns" << std::endl;
    
    f->Close();
    delete waveData;
    
    std::cout << "Enhanced time calibration analysis completed!" << std::endl;
    std::cout << "Output file: time_calib_enhanced_run_" << runnum << "_mid_" << mid 
              << "_ch_" << ch1 << "_" << ch2 << ".root" << std::endl;
    
    return 0;
}