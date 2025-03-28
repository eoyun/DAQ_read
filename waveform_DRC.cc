void waveform_DRC (const int runnum, const int Mid, const int channel){


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
	char pngname[200];
	if (channel < 1)
		ch_to_plot = 0;
	else if (channel > 32)
		ch_to_plot = 31;
	else
		ch_to_plot = channel - 1;
	  
	TCanvas *c1 = new TCanvas("c1", "CAL DAQ", 800, 500);
	TH1F *plot = new TH1F("plot", "Waveform", 1023, 0, 1023); 
	plot->SetStats(0);
	//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Wave/Run_%d_Wave_MID_%d/Run_%d_Wave_MID_%d_FILE_0.dat",runnum,runnum,runnum,Mid,runnum,Mid);
	sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/sungwon/2024_DRC_TB_Data/Run_%d/Run_%d_Wave/Run_%d_Wave_MID_%d/Run_%d_Wave_MID_%d_FILE_0.dat",runnum,runnum,runnum,Mid,runnum,Mid);
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
		plot->Reset();
		for (i = 0; i < 1023; i++) {
			plot->Fill(i, adc[i * 32 + ch_to_plot]);
		}
		plot->Draw("hist");
		c1->Modified();
		c1->Update();
		
		printf("Continue?");
		scanf("%d", &cont);
		
		if ( cont == 0 )
			evt = nevt;
	}
	fclose(fp);

}
