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
}

readData::~readData(){
	fclose(fp);
}


void readData::load(){
	for (int i=0;i<100;i++){

		if (fastflag == 0)
			sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Wave/Run_%d_Wave_MID_%d/Run_%d_Wave_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,i);
		else 
			sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Fast/Run_%d_Fast_MID_%d/Run_%d_Fast_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,i);
		//printf("%s %d\n",filename,nevt);
		if (access(filename,0)!=0)break;
		fp = fopen(filename, "rb");
  		fseek(fp, 0L, SEEK_END);
  		file_size = ftell(fp);
  		fclose(fp);
		if (fastflag == 0) nevt += file_size/65536;
		else nevt += file_size/256;
		file_nevts.push_back(nevt);
	}
	if (fastflag == 0)
		sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Wave/Run_%d_Wave_MID_%d/Run_%d_Wave_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
	else 
		sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Fast/Run_%d_Fast_MID_%d/Run_%d_Fast_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
	fp = fopen(filename,"rb");		
	std::cout<<filename<<" is loaded"<<std::endl;
}

void readData::getEvt(){
	fread(header,1,64,fp);
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
		else 
			sprintf(filename,"/pnfs/knu.ac.kr/data/cms/store/user/yeo/HDD/Run_%d/Run_%d_Fast/Run_%d_Fast_MID_%d/Run_%d_Fast_MID_%d_FILE_%d.dat",runNum,runNum,runNum,Mid,runNum,Mid,filenum);
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


