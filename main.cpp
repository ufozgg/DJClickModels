/*************
Auther: THUIR Ruizhe Zhang
*************/
#include"cmdline.h"
#include"data_struct.h"
#include"config.h"
#include"common.h"
#include"model.h"
#include"dump.h"
#include"dcm.h"
#include"ubm.h"
#include"baseline.h"
#include"data_filter.h"
#include"load.h"
using namespace std;
int main(int argc,char* argv[])
{
	cmdline::parser pa;
	assert(MINSESSION<=MAXSESSION);
	pa.add<std::string>("module",'m',"module name such as \"ubm\"",false,"");
	pa.add<std::string>("save",'s',"format: \"clc\" or \"zrz\"",false,"",cmdline::oneof<std::string>("clc","zrz"));
	pa.add<std::string>("load",'l',"format: \"clc\" or \"zrz\"or \"zjq\"",false,"",cmdline::oneof<std::string>("clc","zrz","zjq"));
	pa.add<std::string>("indir",'i',"format: a dir",false,"");
	pa.add<std::string>("outdir",'o',"format: a dir",false,"");
	pa.add<std::string>("filter",'f',"filter or not",false,"true");
	pa.add<std::string>("sample",'S',"sample or not",false,"false");
	//pa.add<std::string>("data",'d',"load data from,default from ../data/part-r-xxxxx",false,"",cmdline::oneof<std::string>("ubm"));
	pa.parse_check(argc,argv);
	std::cout<<pa.get<std::string>("module")<<endl; 
	std::cout<<pa.get<std::string>("save")<<endl;
	std::cout<<pa.get<std::string>("load")<<endl;
	std::cout<<pa.get<std::string>("indir")<<endl;
	std::cout<<pa.get<std::string>("outdir")<<endl;
	if(pa.get<std::string>("indir")!="")
		data_dir=pa.get<std::string>("indir");
	if(pa.get<std::string>("outdir")!="")
		save_file=pa.get<std::string>("outdir");
	if(pa.get<std::string>("filter")=="true")
		IFFILTER=true;
	else
		IFFILTER=false;
	if(pa.get<std::string>("load")=="zjq") 
		load_data_zjq_181113(data_dir);
	else
		read_Data_20170903();
	//return 0;
	if(pa.get<std::string>("filter")=="true")
		Data_Filter();
	vector<string> savekey=split(pa.get<std::string>("save"),',');
	if(find(savekey.begin(),savekey.end(),"clc")!=savekey.end())
		save_as_clc();
	cout<<"Format error: "<<Filter[0]<<endl;
	cout<<"Head Format error: "<<Filter[1]<<endl;
	cout<<"Not first page: "<<Filter[2]<<endl;
	cout<<"More than "<<MAXDOCPERPAGE<<" docs' sessions: "<<Filter[3]<<endl;
	cout<<"Less than "<<MINDOCPERPAGE<<" docs' sessions: "<<Filter[4]<<endl;
	cout<<"Sessions in query with too few sessions: "<<Filter[6]<<endl;
	cout<<"Sessions in query with too much sessions: "<<Filter[5]<<endl;
	cout<<"Querys with too FEW sessions: "<<qFilter[1]<<endl;
	cout<<"Querys with too MORE sessions: "<<qFilter[0]<<endl;
	cout<<"Ok Sessions: "<<sessions.size()-Filter[5]-Filter[6]<<endl;
	cout<<"Ok querys: "<<querys.size()-qFilter[0]-qFilter[1]<<endl;
	cout<<"All docs num: "<<docs.size()<<endl;
	vector<string> mods=split(pa.get<std::string>("module"),',');
	if(find(mods.begin(),mods.end(),"baseline")!=mods.end())
	{
		baseline baseline_mod=baseline();
		baseline_mod.train();
		baseline_mod.test();
		if(pa.get<std::string>("sample")=="true")
			baseline_mod.sample();
		#ifdef DEBUG
			baseline_mod.check();
		#endif
	}
	if(find(mods.begin(),mods.end(),"dcm")!=mods.end())
	{
		dcm dcm_mod=dcm();
		dcm_mod.train();
		dcm_mod.test();
		if(pa.get<std::string>("sample")=="true")
			dcm_mod.sample();
		#ifdef DEBUG
			dcm_mod.check();
		#endif
	}
	if(find(mods.begin(),mods.end(),"ubm")!=mods.end())
	{
		ubm ubm_mod=ubm();
		ubm_mod.train();
		ubm_mod.test();
		if(pa.get<std::string>("sample")=="true")
			ubm_mod.sample();
		#ifdef DEBUG
			ubm_mod.check();
		#endif
	}
	return 0;
} 
