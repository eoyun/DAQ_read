void ped_S (const int runnum, const int Mid, const int channel){


	int ch_to_plot;
	FILE *fp;
	long long file_size;
	int nevt;
	char data[64];
	short adc[32736];
	int evt;
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
	long long ltmp;
	int i;
	int cont;
	char filename[200];
	char outname[200];
	double ped=0.;
	if (channel < 1)
		ch_to_plot = 0;
	else if (channel > 32)
		ch_to_plot = 31;
	else
		ch_to_plot = channel - 1;
	  
	TCanvas *c1 = new TCanvas("c1", "CAL DAQ", 800, 500);
	TH1F *plot = new TH1F("plot", "Waveform", 1023, 0, 1023); 
	plot->SetStats(0);
	sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/SHECRO/SRun_%d/SRun_%d_Wave/SRun_%d_Wave_MID_%d/SRun_%d_Wave_MID_%d_FILE_0.dat",runnum,runnum,runnum,Mid,runnum,Mid);
	printf("%s",filename);
	fp = fopen(filename, "rb");
	fseek(fp, 0L, SEEK_END);
	file_size = ftell(fp);
	fclose(fp);
	nevt = file_size / 65536;
	
	printf("-----------------------------------------------------------------------\n");
	fp = fopen(filename, "rb");
	
	for (evt=0;evt<nevt;evt++){
		fread(data, 1, 64, fp);
		fread(adc, 2, 32736, fp);
		for (i = 1; i < 101; i++) {
			ped += (double)adc[i * 32 + ch_to_plot]/nevt;
		}
		
	}
	ped = ped/100.;
	fclose(fp);
	sprintf(outname,"./ped/S%d_%d_%d.txt",runnum,Mid,channel);
	fp = fopen(outname,"wt");
	fprintf(fp,"%f",ped);
	fclose(fp);

}
