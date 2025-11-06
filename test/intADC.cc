#include <iostream>
#include <vector>
#include <numeric>   // std::accumulate
#include <cstdlib>   // std::atoi
#include <string>

#include "readData.h"
#include "TTree.h"
#include "TFile.h"
#include "functions.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TFitResultPtr.h"

static inline double integrate_window(const std::vector<double>& v, int first, int last) {
    if (v.empty()) return 0.0;
    if (first < 0) first = 0;
    if (last  > (int)v.size()) last = (int)v.size();
    if (first >= last) return 0.0;
    return std::accumulate(v.begin() + first, v.begin() + last, 0.0);
}

int main(int argc, char** argv){
    // Usage: prog run1 run2 mid ch
    //   - If you only pass run1 mid ch (3 args after prog), it will process a single run.
    if (argc != 5 && argc != 4) {
        std::cerr << "Usage:\n  " << argv[0] << " <run1> <run2> <mid> <ch>\n"
                  << "  or\n  " << argv[0] << " <run1> <mid> <ch>\n";
        return 1;
    }

    int run1 = 0, run2 = -1, mid = 0, ch = 0;
    if (argc == 5) {
        run1 = std::atoi(argv[1]);
        run2 = std::atoi(argv[2]);
        mid  = std::atoi(argv[3]);
        ch   = std::atoi(argv[4]);
    } else { // argc == 4, single-run mode
        run1 = std::atoi(argv[1]);
        mid  = std::atoi(argv[2]);
        ch   = std::atoi(argv[3]);
        run2 = -1;
    }

    // Integration & pedestal settings (match your original)
    const int ped_samples = 100;
    const int win_first = 300;
    const int win_last  = 550;

    // Prepare readers
    readData* data1 = nullptr;
    readData* data2 = nullptr;

    try {
        data1 = new readData(run1, mid);
        data1->load();
        if (run2 >= 0) {
            data2 = new readData(run2, mid);
            data2->load();
        }
    } catch (...) {
        std::cerr << "ERROR: Failed to initialize/load readData.\n";
        delete data1;
        delete data2;
        return 2;
    }

    // Output root file name
    std::string outname;
    if (run2 >= 0) outname = "./roots/intADC_" + std::to_string(run1) + "_" + std::to_string(run2) + ".root";
    else           outname = "./roots/intADC_" + std::to_string(run1) + ".root";

    TFile* fout = TFile::Open(outname.c_str(), "RECREATE");
    if (!fout || fout->IsZombie()) {
        std::cerr << "ERROR: Cannot create output file: " << outname << "\n";
        delete data1; delete data2;
        return 3;
    }

    TTree* t = new TTree("intADC", "");
    double intADC_w  = std::numeric_limits<double>::quiet_NaN(); // e.g. run2
    double intADC_wo = std::numeric_limits<double>::quiet_NaN(); // e.g. run1

    // Keep branch names same as your original for compatibility
    t->Branch("intADC_w",  &intADC_w);
    t->Branch("intADC_wo", &intADC_wo);

    // Determine event count
    int n1 = data1->getNevt();
    int nevt = n1;
    if (data2) {
        int n2 = data2->getNevt();
        nevt = (n1 < n2) ? n1 : n2;
    }

    std::cout << "Processing " << nevt << " events on MID " << mid << ", CH " << ch;
    if (data2) std::cout << " (runs " << run1 << " & " << run2 << ")";
    std::cout << std::endl;

    for (int i = 0; i < nevt; ++i) {
        // Advance event
        data1->getEvt();
        if (data2) data2->getEvt();
        // Read raw waveforms for the channel
        std::vector<short> w1_raw = data1->readEvt(ch);
        std::vector<double> w1 = pedcorwave(w1_raw, ped_samples);

        std::vector<double> w2;
        if (data2) {
            std::vector<short> w2_raw = data2->readEvt(ch);
            w2 = pedcorwave(w2_raw, ped_samples);
        }

        // Integrate
        double v1 = integrate_window(w1, win_first, win_last);
        double v2 = data2 ? integrate_window(w2, win_first, win_last) : std::numeric_limits<double>::quiet_NaN();

        // Map to branches (preserving your naming: 'wo' ← first, 'w' ← second)
        intADC_wo = v1;
        intADC_w  = v2;

        t->Fill();
    }
    std::cout<<"dbg2"<<std::endl;

    t->Write();
    fout->Close();

    //delete data1;
    //delete data2;

    std::cout << "Done. Wrote " << nevt << " entries to " << outname << std::endl;
    return 0;
}

