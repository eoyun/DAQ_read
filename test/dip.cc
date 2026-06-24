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

    int run = std::atoi(argv[1]);

    // Integration & pedestal settings (match your original)
    const int ped_samples = 100;
    const int win_first = 300;
    const int win_last  = 550;


    // Prepare readers
    static const int NMID = 4;
    static const int NCH  = 16;
    
    int mid_list[NMID] = {1, 2, 11, 16};
    int ch_list[NCH];
    
    for (int ich = 0; ich < NCH; ++ich) {
        ch_list[ich] = 2 * (ich + 1);   // 2, 4, 6, ..., 32
    }
    
    readData* data_mid[NMID] = {nullptr, nullptr, nullptr, nullptr};
    
    for (int imid = 0; imid < NMID; ++imid) {
        data_mid[imid] = new readData(run, mid_list[imid]);
        data_mid[imid]->load();
    }
    
    // Output root file name
    std::string outname = "./roots/dip_" + std::to_string(run) + ".root";
    
    TFile* fout = TFile::Open(outname.c_str(), "RECREATE");
    
    TTree* t = new TTree("intADC", "");
    
    // intADC[imid][ich]
    // imid = 0,1,2,3  -> MID 1,2,11,16
    // ich  = 0..15    -> CH 2,4,6,...,32
    double intADC[NMID][NCH];
    
    t->Branch("mid",    mid_list, "mid[4]/I");
    t->Branch("ch",     ch_list,  "ch[16]/I");
    t->Branch("intADC", intADC,   "intADC[4][16]/D");
    
    // Determine event count: use minimum event count among all MIDs
    int nevt = data_mid[0]->getNevt();
    
    for (int imid = 1; imid < NMID; ++imid) {
        int n = data_mid[imid]->getNevt();
        if (n < nevt) nevt = n;
    }
    
    std::cout << "Processing " << nevt << " events" << std::endl;
    std::cout << "MIDs: ";
    for (int imid = 0; imid < NMID; ++imid) {
        std::cout << mid_list[imid] << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Channels: ";
    for (int ich = 0; ich < NCH; ++ich) {
        std::cout << ch_list[ich] << " ";
    }
    std::cout << std::endl;
    
    // Event loop
    for (int iev = 0; iev < nevt; ++iev) {
    
        // Advance event for all MIDs
        for (int imid = 0; imid < NMID; ++imid) {
            data_mid[imid]->getEvt();
        }
    
        // Initialize with NaN for safety
        for (int imid = 0; imid < NMID; ++imid) {
            for (int ich = 0; ich < NCH; ++ich) {
                intADC[imid][ich] = std::numeric_limits<double>::quiet_NaN();
            }
        }
    
        // MID × channel loop
        for (int imid = 0; imid < NMID; ++imid) {
            for (int ich = 0; ich < NCH; ++ich) {
    
                int ch = ch_list[ich];
    
                std::vector<short> w_raw = data_mid[imid]->readEvt(ch);
                std::vector<double> w = pedcorwave(w_raw, ped_samples);
    
                double v = integrate_window(w, win_first, win_last);
    
                intADC[imid][ich] = v;
            }
        }
    
        t->Fill();
    }
    
    std::cout << "Finished event loop" << std::endl;
    
    t->Write();
    fout->Close();
    
    // Cleanup
    for (int imid = 0; imid < NMID; ++imid) {
        delete data_mid[imid];
        data_mid[imid] = nullptr;
    }    


    std::cout << "Done. Wrote " << nevt << " entries to " << outname << std::endl;
    return 0;
}

