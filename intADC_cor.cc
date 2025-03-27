void intADC_cor (const int runnum, const int Mid, const int channel){


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
	char pedname[200];
	char outname[200];
	double sum;
	double ped;
	FILE *fp_data;
	if (channel < 1)
		ch_to_plot = 0;
	else if (channel > 32)
		ch_to_plot = 31;
	else
		ch_to_plot = channel - 1;
	  
	TH1F *plot = new TH1F("plot", "Waveform", 100500, -5000, 1000000); 
	plot->SetStats(0);
 	//sprintf(pedname,"./ped/%d_%d_%d.txt",runnum,Mid,channel);
 	//fp = fopen(pedname,"rt");
 	//fscanf(fp,"%lf",&ped);
 	//fclose(fp);
	fp_data = fopen("data.csv","wt");
	fprintf(fp_data,"evt,intADC\n");
	for (int j=0;j<200;j++){
		sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Wave/Run_%d_Wave_MID_%d/Run_%d_Wave_MID_%d_FILE_%d.dat",runnum,runnum,runnum,Mid,runnum,Mid,j);
		if (access( filename, F_OK ) == -1) break;
		printf("%s",filename);
		fp = fopen(filename, "rb");
		fseek(fp, 0L, SEEK_END);
		file_size = ftell(fp);
		fclose(fp);
		nevt = file_size / 65536;
		
		printf("-----------------------------------------------------------------------\n");
		fp = fopen(filename, "rb");
		
		for (evt=0;evt<nevt;evt++){
			sum=0.;
			fread(data, 1, 64, fp);
			fread(adc, 2, 32736, fp);
			ped = 0.;

			for (i=1;i<51;i++){
				ped +=(double) adc[i * 32 + ch_to_plot]/50.;
			}
			for (i = 50; i < 400; i++) {

				sum += (double)(ped - adc[i * 32 + ch_to_plot]);
			}
			plot->Fill(sum*50000./18772.);
			
			fprintf(fp_data,"%d,%f\n",evt,sum);		
		}
		fclose(fp);
	}
	fclose(fp_data);
	sprintf(outname,"./out_cor/%d_%d_%d.root",runnum,Mid,channel);
	TFile *froot = new TFile(outname,"recreate");
	plot->Write();
	froot->Close();
	plot->Draw("hist");

}
