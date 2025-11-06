#include <vector>
#include <iostream>
#include <cstdlib>
#include <readData.h>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>

readData::readData(const int runnum, const int mid):
runNum(runnum),
Mid(mid)
{
	drs_stop.clear();
}

readData::~readData(){
	fclose(fp);
}


void readData::load(){
	for (int i=0;i<100;i++){

		if (fastflag == 0)
			sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Wave/Run_%d_Wave_MID_%d/Run_%d_Wave_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,i);
			//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/SHECRO/SRun_%d/SRun_%d_Wave/SRun_%d_Wave_MID_%d/SRun_%d_Wave_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,i);
			//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/sungwon/2024_DRC_TB_Data/Run_%d/Run_%d_Wave/Run_%d_Wave_MID_%d/Run_%d_Wave_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,i);
		else 
			sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Fast/Run_%d_Fast_MID_%d/Run_%d_Fast_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,i);
			//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/SHECRO/SRun_%d/SRun_%d_Fast/SRun_%d_Fast_MID_%d/SRun_%d_Fast_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,i);
			//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/sungwon/2024_DRC_TB_Data/Run_%d/Run_%d_Fast/Run_%d_Fast_MID_%d/Run_%d_Fast_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,i);
		//printf("%s %d\n",filename,nevt);
		if (access(filename,0)!=0)break;
		fp = fopen(filename, "rb");
  		fseek(fp, 0L, SEEK_END);
  		file_size = ftell(fp);
		//std::cout<<"file size : "<<file_size<<std::endl;
  		fclose(fp);
		if (fastflag == 0) nevt += file_size/65536;
		else nevt += file_size/256;
		file_nevts.push_back(nevt);
	}
	if (fastflag == 0)
		sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Wave/Run_%d_Wave_MID_%d/Run_%d_Wave_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
		//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/SHECRO/SRun_%d/SRun_%d_Wave/SRun_%d_Wave_MID_%d/SRun_%d_Wave_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
		//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/sungwon/2024_DRC_TB_Data/Run_%d/Run_%d_Wave/Run_%d_Wave_MID_%d/Run_%d_Wave_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
	else 
		sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Fast/Run_%d_Fast_MID_%d/Run_%d_Fast_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
		//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/SHECRO/SRun_%d/SRun_%d_Fast/SRun_%d_Fast_MID_%d/SRun_%d_Fast_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
		//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/sungwon/2024_DRC_TB_Data/Run_%d/Run_%d_Fast/Run_%d_Fast_MID_%d/Run_%d_Fast_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
	fp = fopen(filename,"rb");		
	std::cout<<filename<<" is loaded"<<std::endl;
}

void readData::getEvt(){
	drs_stop.clear();
	fread(header,1,64,fp);
	data_length = header[0] & 0xFF;
	itmp = header[1] & 0xFF;
	itmp = itmp << 8;
	data_length = data_length + itmp;
	itmp = header[2] & 0xFF;
	itmp = itmp << 16;
	data_length = data_length + itmp;
	itmp = header[3] & 0xFF;
	itmp = itmp << 24;
	data_length = data_length + itmp;
	
	
	run_number = header[4] & 0xFF;
	itmp = header[5] & 0xFF;
	itmp = itmp << 8;
	run_number = run_number + itmp;
	
	
	tcb_trig_type = header[6] & 0xFF;
	
	
	tcb_trig_number = header[7] & 0xFF;
	itmp = header[8] & 0xFF;
	itmp = itmp << 8;
	tcb_trig_number = tcb_trig_number + itmp;
	itmp = header[9] & 0xFF;
	itmp = itmp << 16;
	tcb_trig_number = tcb_trig_number + itmp;
	itmp = header[10] & 0xFF;
	itmp = itmp << 24;
	tcb_trig_number = tcb_trig_number + itmp;
	
	
	fine_time = header[11] & 0xFF;
	fine_time = fine_time * 11;     // actually * (1000 / 90)
	coarse_time = header[12] & 0xFF;
	ltmp = header[13] & 0xFF;
	ltmp = ltmp << 8;
	coarse_time = coarse_time + ltmp;
	ltmp = header[14] & 0xFF;
	ltmp = ltmp << 16;
	coarse_time = coarse_time + ltmp;
	ltmp = header[15] & 0xFF;
	ltmp = ltmp << 24;
	coarse_time = coarse_time + ltmp;
	ltmp = header[16] & 0xFF;
	ltmp = ltmp << 32;
	coarse_time = coarse_time + ltmp;
	ltmp = header[17] & 0xFF;
	ltmp = ltmp << 40;
	coarse_time = coarse_time + ltmp;
	coarse_time = coarse_time * 1000;   // get ns
	tcb_trig_time = fine_time + coarse_time;
	
	
	mid = header[18] & 0xFF;
	
	
	local_trig_number = header[19] & 0xFF;
	itmp = header[20] & 0xFF;
	itmp = itmp << 8;
	local_trig_number = local_trig_number + itmp;
	itmp = header[21] & 0xFF;
	itmp = itmp << 16;
	local_trig_number = local_trig_number + itmp;
	itmp = header[22] & 0xFF;
	itmp = itmp << 24;
	local_trig_number = local_trig_number + itmp;
	
	
	local_trigger_pattern = header[23] & 0xFF;
	itmp = header[24] & 0xFF;
	itmp = itmp << 8;
	local_trigger_pattern = local_trigger_pattern + itmp;
	itmp = header[25] & 0xFF;
	itmp = itmp << 16;
	local_trigger_pattern = local_trigger_pattern + itmp;
	itmp = header[26] & 0xFF;
	itmp = itmp << 24;
	local_trigger_pattern = local_trigger_pattern + itmp;
	
	
	fine_time = header[27] & 0xFF;
	fine_time = fine_time * 11;     // actually * (1000 / 90)
	coarse_time = header[28] & 0xFF;
	ltmp = header[29] & 0xFF;
	ltmp = ltmp << 8;
	coarse_time = coarse_time + ltmp;
	ltmp = header[30] & 0xFF;
	ltmp = ltmp << 16;
	coarse_time = coarse_time + ltmp;
	ltmp = header[31] & 0xFF;
	ltmp = ltmp << 24;
	coarse_time = coarse_time + ltmp;
	ltmp = header[32] & 0xFF;
	ltmp = ltmp << 32;
	coarse_time = coarse_time + ltmp;
	ltmp = header[33] & 0xFF;
	ltmp = ltmp << 40;
	coarse_time = coarse_time + ltmp;
	coarse_time = coarse_time * 1000;   // get ns
	local_trig_time = fine_time + coarse_time;
	
	diff_time = local_trig_time - tcb_trig_time;
	for (int k=0; k<4;k++){
		drs_stop_tmp = header[34 + k*2] & 0xFF;
		itmp = header[35 + k * 2] & 0xFF;
		itmp = itmp << 8;
		drs_stop_tmp += itmp;
		drs_stop.push_back(drs_stop_tmp);
	}
	//std::cout<<drs_stop_tmp<<" | stop bin "<<std::endl;
	
	if (fastflag == 0) 
		fread(adc,2,32736,fp);
	else 
		fread(adc_fast,2,96,fp);
	
	evt ++;
	auto it = std::find(file_nevts.begin(),file_nevts.end(),evt);
	if (it != file_nevts.end()){
		filenum ++;
		
		fclose(fp);
		if (fastflag == 0)
			sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Wave/Run_%d_Wave_MID_%d/Run_%d_Wave_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
			//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/SHECRO/SRun_%d/SRun_%d_Wave/SRun_%d_Wave_MID_%d/SRun_%d_Wave_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
			//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/sungwon/2024_DRC_TB_Data/Run_%d/Run_%d_Wave/Run_%d_Wave_MID_%d/Run_%d_Wave_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
		else 
			sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Fast/Run_%d_Fast_MID_%d/Run_%d_Fast_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
			//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/SHECRO/SRun_%d/SRun_%d_Fast/SRun_%d_Fast_MID_%d/SRun_%d_Fast_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
			//sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/sungwon/2024_DRC_TB_Data/Run_%d/Run_%d_Fast/Run_%d_Fast_MID_%d/Run_%d_Fast_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
		fp = fopen(filename,"rb");			
		std::cout<<filename<<" is loaded"<<std::endl;
	} 
}

std::vector<short> readData::readEvt(int channel){
	if (channel < 1)
		ch_to_plot = 0;
	else if (channel > 32)
		ch_to_plot = 31;
	else
		ch_to_plot = channel - 1;
	std::vector<short> output;
	if (fastflag == 0){ 
		for (int i=0;i<1023;i++)
			output.push_back(adc[i * 32 + ch_to_plot]);
	}
	else {
		for (int i=0;i<3;i++)
			output.push_back(adc_fast[ch_to_plot * 3 + i]);
	}
	return output;	
}

std::vector<int> readData::getDrsStop(){
	return drs_stop;
}


