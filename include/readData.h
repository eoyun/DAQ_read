#include <iostream>
#include <vector>

class readData{
private:
	char filename[200];
	int nevt=0;
	std::vector<int> file_nevts;
	FILE* fp;
	long long file_size;
	int fastflag=0;
	int runNum;
	int Mid;
	char header[64];
	short adc[32736];
	short adc_fast[96];
	int filenum = 0;
	int evt=0;
	int ch_to_plot;
	int data_length;
	int run_number;
	int tcb_trig_type;
	int tcb_trig_number;
	long long tcb_trig_time;
	int mid;
	int local_trig_number;
	int local_trigger_pattern;
	long long local_trig_time;
	long long diff_time;
	long long fine_time;
	long long coarse_time;
	int itmp;
	int drs_stop_tmp;
	long long ltmp;
	std::vector<int> drs_stop;
public:
	readData(const int runnum, const int mid);
	~readData();
	void setFast(){fastflag = 1;}
	void load();
	int getNevt(){return nevt;}
	void getEvt();
	long long getTrigTime(){return tcb_trig_time;}
	long long getLocalTrigTime(){return local_trig_time;}
	std::vector<short> readEvt(int channel);
	std::vector<int> getDrsStop();
	
};
