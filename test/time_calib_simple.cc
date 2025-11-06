#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <tuple>
#include "readData.h"
#include "functions.h"

class TimeCalibrator {
private:
    int mid, runnum;
    std::vector<std::vector<double>> local_calib_constants;
    std::vector<double> global_calib_constants;
    
public:
    TimeCalibrator(int mid_val, int runnum_val) : mid(mid_val), runnum(runnum_val) {
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
        
        // Print some statistics
        double avg_global = 0.0;
        for (int bin = 0; bin < 1024; bin++) {
            avg_global += global_calib_constants[bin];
        }
        avg_global /= 1024;
        std::cout << "Global calibration average: " << avg_global << " ps" << std::endl;
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
    
    // Get calibration statistics
    void printCalibrationStats() {
        std::cout << "\n=== Calibration Statistics ===" << std::endl;
        
        // Local calibration stats per channel
        for (int ch = 0; ch < 32; ch++) {
            double sum = 0.0, min_val = 1e9, max_val = -1e9;
            for (int bin = 0; bin < 1024; bin++) {
                double val = local_calib_constants[ch][bin];
                sum += val;
                if (val < min_val) min_val = val;
                if (val > max_val) max_val = val;
            }
            double avg = sum / 1024;
            
            // Calculate RMS
            double rms = 0.0;
            for (int bin = 0; bin < 1024; bin++) {
                double diff = local_calib_constants[ch][bin] - avg;
                rms += diff * diff;
            }
            rms = sqrt(rms / 1024);
            
            std::cout << "Channel " << std::setw(2) << ch+1 << ": avg=" << std::setw(7) << std::fixed << std::setprecision(2) 
                     << avg << " ps, rms=" << std::setw(6) << rms << " ps, range=[" 
                     << std::setw(6) << min_val << ", " << std::setw(6) << max_val << "]" << std::endl;
        }
        
        // Global calibration stats
        double sum = 0.0, min_val = 1e9, max_val = -1e9;
        for (int bin = 0; bin < 1024; bin++) {
            double val = global_calib_constants[bin];
            sum += val;
            if (val < min_val) min_val = val;
            if (val > max_val) max_val = val;
        }
        double avg = sum / 1024;
        
        double rms = 0.0;
        for (int bin = 0; bin < 1024; bin++) {
            double diff = global_calib_constants[bin] - avg;
            rms += diff * diff;
        }
        rms = sqrt(rms / 1024);
        
        std::cout << "Global    : avg=" << std::setw(7) << std::fixed << std::setprecision(2) 
                 << avg << " ps, rms=" << std::setw(6) << rms << " ps, range=[" 
                 << std::setw(6) << min_val << ", " << std::setw(6) << max_val << "]" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cout << "Usage: " << argv[0] << " <runnum> <mid> <ch1> <ch2> [nevents]" << std::endl;
        std::cout << "Example: " << argv[0] << " 11422 9 4 6 100" << std::endl;
        return 1;
    }
    
    int runnum = atoi(argv[1]);
    int mid = atoi(argv[2]);
    int ch1 = atoi(argv[3]);
    int ch2 = atoi(argv[4]);
    int max_events = (argc > 5) ? atoi(argv[5]) : 100;
    
    std::cout << "=== Enhanced Time Calibration Analysis ===" << std::endl;
    std::cout << "Run: " << runnum << ", MID: " << mid << ", Channels: " << ch1 << " & " << ch2 << std::endl;
    std::cout << "Max events: " << max_events << std::endl;
    
    // Initialize calibrator and data reader
    TimeCalibrator calibrator(mid, runnum);
    readData* waveData = new readData(runnum, mid);
    waveData->load();
    
    // Load existing calibrations
    calibrator.loadLocalCalibration();
    calibrator.generateGlobalCalibration();
    calibrator.printCalibrationStats();
    
    // Save calibrations
    calibrator.saveCalibration("_enhanced");
    
    // Open output files for timing results
    char output_filename[300];
    sprintf(output_filename, "time_calib_results_run_%d_mid_%d_ch_%d_%d.txt", runnum, mid, ch1, ch2);
    std::ofstream output_file(output_filename);
    
    if (!output_file.is_open()) {
        std::cout << "Error: Cannot create output file " << output_filename << std::endl;
        return 1;
    }
    
    // Write header
    output_file << "# Enhanced Time Calibration Results" << std::endl;
    output_file << "# Run: " << runnum << " MID: " << mid << " Channels: " << ch1 << " " << ch2 << std::endl;
    output_file << "# Event Time1_Local Time1_Global Time1_Uncalib Time2_Local Time2_Global Time2_Uncalib";
    output_file << " Diff_Local Diff_Global Diff_Uncalib DRS_Stop1 DRS_Stop2" << std::endl;
    
    int drs_num1 = (ch1 - 1) / 8;
    int drs_num2 = (ch2 - 1) / 8;
    int nevt = std::min(waveData->getNevt(), max_events);
    
    // Statistics tracking
    std::vector<double> time_diffs_local, time_diffs_global, time_diffs_uncalib;
    
    std::cout << "\nAnalyzing " << nevt << " events..." << std::endl;
    
    for (int i = 0; i < nevt; i++) {
        if (i % 50 == 0) {
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
        
        int drs_stop1 = drs_stop[drs_num1];
        int drs_stop2 = drs_stop[drs_num2];
        
        // Apply different calibrations
        std::map<double,double> TC_wave1_local = calibrator.applyCalibration(pedcor_wave1, ch1, drs_stop1, false);
        std::map<double,double> TC_wave1_global = calibrator.applyCalibration(pedcor_wave1, ch1, drs_stop1, true);
        std::map<double,double> TC_wave2_local = calibrator.applyCalibration(pedcor_wave2, ch2, drs_stop2, false);
        std::map<double,double> TC_wave2_global = calibrator.applyCalibration(pedcor_wave2, ch2, drs_stop2, true);
        
        // Calculate timing with different methods
        double time1_local = getTimewTC(TC_wave1_local, 0.4);
        double time1_global = getTimewTC(TC_wave1_global, 0.4);
        double time1_uncalib = getTime(pedcor_wave1, 0.4);
        
        double time2_local = getTimewTC(TC_wave2_local, 0.4);
        double time2_global = getTimewTC(TC_wave2_global, 0.4);
        double time2_uncalib = getTime(pedcor_wave2, 0.4);
        
        // Calculate time differences
        double time_diff_local = time1_local - time2_local;
        double time_diff_global = time1_global - time2_global;
        double time_diff_uncalib = time1_uncalib - time2_uncalib;
        
        // Store for statistics
        time_diffs_local.push_back(time_diff_local);
        time_diffs_global.push_back(time_diff_global);
        time_diffs_uncalib.push_back(time_diff_uncalib);
        
        // Write to output file
        output_file << i << " " << std::fixed << std::setprecision(3);
        output_file << time1_local << " " << time1_global << " " << time1_uncalib << " ";
        output_file << time2_local << " " << time2_global << " " << time2_uncalib << " ";
        output_file << time_diff_local << " " << time_diff_global << " " << time_diff_uncalib << " ";
        output_file << drs_stop1 << " " << drs_stop2 << std::endl;
    }
    
    output_file.close();
    
    // Calculate and print statistics
    auto calculate_stats = [](const std::vector<double>& data) {
        if (data.empty()) return std::make_tuple(0.0, 0.0, 0.0);
        
        double sum = 0.0;
        for (double val : data) sum += val;
        double mean = sum / data.size();
        
        double variance = 0.0;
        for (double val : data) {
            double diff = val - mean;
            variance += diff * diff;
        }
        double rms = sqrt(variance / data.size());
        
        std::vector<double> sorted_data = data;
        std::sort(sorted_data.begin(), sorted_data.end());
        double median = (data.size() % 2 == 0) ? 
            (sorted_data[data.size()/2 - 1] + sorted_data[data.size()/2]) / 2.0 :
            sorted_data[data.size()/2];
            
        return std::make_tuple(mean, rms, median);
    };
    
    auto [mean_local, rms_local, median_local] = calculate_stats(time_diffs_local);
    auto [mean_global, rms_global, median_global] = calculate_stats(time_diffs_global);
    auto [mean_uncalib, rms_uncalib, median_uncalib] = calculate_stats(time_diffs_uncalib);
    
    std::cout << "\n=== Time Difference Statistics ===" << std::endl;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Local Calibration   - Mean: " << std::setw(8) << mean_local << " ns, RMS: " << std::setw(8) << rms_local << " ns, Median: " << std::setw(8) << median_local << " ns" << std::endl;
    std::cout << "Global Calibration  - Mean: " << std::setw(8) << mean_global << " ns, RMS: " << std::setw(8) << rms_global << " ns, Median: " << std::setw(8) << median_global << " ns" << std::endl;
    std::cout << "No Calibration      - Mean: " << std::setw(8) << mean_uncalib << " ns, RMS: " << std::setw(8) << rms_uncalib << " ns, Median: " << std::setw(8) << median_uncalib << " ns" << std::endl;
    
    std::cout << "\nImprovement factors:" << std::endl;
    std::cout << "Local vs Uncalibrated:  " << std::setw(6) << std::setprecision(2) << (rms_uncalib / rms_local) << "x better" << std::endl;
    std::cout << "Global vs Uncalibrated: " << std::setw(6) << std::setprecision(2) << (rms_uncalib / rms_global) << "x better" << std::endl;
    std::cout << "Local vs Global:        " << std::setw(6) << std::setprecision(2) << (rms_global / rms_local) << "x " << (rms_local < rms_global ? "better" : "worse") << std::endl;
    
    delete waveData;
    
    std::cout << "\nEnhanced time calibration analysis completed!" << std::endl;
    std::cout << "Results saved to: " << output_filename << std::endl;
    
    return 0;
}