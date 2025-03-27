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
public:
	readData(const int runnum, const int mid);
	~readData();
	void setFast(){fastflag = 1;}
	void load();
	int getNevt(){return nevt;}
	void getEvt();
	std::vector<short> readEvt(int channel);
	
};
