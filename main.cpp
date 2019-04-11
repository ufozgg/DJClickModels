/*************
Auther: THUIR Ruizhe Zhang
Don't use after 2027A.D.
*************/
#pragma GCC optimize("delete-null-pointer-checks,inline-functions-called-once,expensive-optimizations,optimize-sibling-calls,tree-switch-conversion,inline-small-functions,rerun-cse-after-loop,hoist-adjacent-loads,indirect-inlining,reorder-functions,no-stack-protector,partial-inlining,sched-interblock,cse-follow-jumps,align-functions,strict-aliasing,schedule-insns2,tree-tail-merge,inline-functions,schedule-insns,reorder-blocks,unroll-loops,thread-jumps,crossjumping,caller-saves,devirtualize,align-labels,align-loops,align-jumps,unroll-loops,sched-spec,inline,gcse,gcse-lm,ipa-sra,tree-pre,tree-vrp,peephole2")//,fastmath,Ofast,-Ofast
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
#include"cmdline.h"
#include"data_struct.h"
#include"config.h"
#include"common.h"
#include"model.h"
#include"dump.h"
#include"dcm.h"
#include"ubm.h"
#include"fcm_ubm.h"
#include"baseline.h"
#include"data_filter.h"
#include"load.h"
#include"dbn.h"
#include"mcm.h"
#include"ubmlayout.h"
#include"mcm2.h"
#include"mcm3.h"
#include"vcm.h"
#include"vcm_layout.h"
#include"load_feature.h"
#include"load_query_list.h"
#include"data_mining.h"
using namespace std;
int main(int argc,char* argv[])
{
	cmdline::parser pa;
	assert(MINSESSION<=MAXSESSION);
	pa.add<std::string>("module",'m',"module name such as \"ubm\"",false,"");
	pa.add<std::string>("save",'s',"format: \"clc\" or \"ucf\"",false,"",cmdline::oneof<std::string>("clc","ucf"));
	pa.add<std::string>("load",'l',"format: \"clc\" or \"zjq\" or \"ucf\" or \"default\"",false,"default",cmdline::oneof<std::string>("clc","zjq","ucf","default"));
	pa.add<std::string>("infiles",'i',"input files, none for no file, traindata,testdata,validata split by ','",false,"");
	pa.add<std::string>("outdir",'o',"format: a dir",false,"");
	pa.add<std::string>("filter",'f',"filter or not",false,"true");
	pa.add<std::string>("querylist",'q',"query list",false,"false");
	pa.add<std::string>("sample",'S',"sample or not",false,"false");
	pa.add<std::string>("feature",'F',"load feature or not",false,"false");
	pa.add<std::string>("vertical",'V',"Vertical_type_file_name",false,"false");
	pa.add<std::string>("typetest",'t',"test by type",false,"false");
	pa.add<std::string>("timediv",'T',"get time div by 5%",false,"false");
	pa.add<std::string>("round",'r',"round for models",false,"100");
	pa.add<std::string>("sessioncount",'c',"when filter good sessions",false,"1000000000");
	pa.add<std::string>("divide",'d',"divide",false,"0.0");
	pa.add<std::string>("usetrained",'u',"if true ,not train ,load args from file",false,"false");
	//pa.add<std::string>("data",'d',"load data from,default from ../data/part-r-xxxxx",false,"",cmdline::oneof<std::string>("ubm"));
	pa.parse_check(argc,argv);
	std::cerr<<pa.get<std::string>("module")<<endl; 
	std::cerr<<pa.get<std::string>("save")<<endl;
	std::cerr<<pa.get<std::string>("load")<<endl;
	std::cerr<<pa.get<std::string>("infiles")<<endl;
	std::cerr<<pa.get<std::string>("outdir")<<endl;
	MAXROUND=atoi(pa.get<std::string>("round").data());
	/*if(pa.get<std::string>("infiles")!="")
		data_dir=pa.get<std::string>("indir");*/
	if(pa.get<std::string>("outdir")!="")
		save_file=pa.get<std::string>("outdir");
	if(pa.get<std::string>("filter")=="true")
		IFFILTER=true;
	else
		IFFILTER=false;
	if(pa.get<std::string>("vertical")!="false")
	{
		load_vertical_type(pa.get<std::string>("vertical"));
	}
	Doc x=Doc();
	docs.push_back(x);
	Session y=Session();
	y.enable=false;
	sessions.push_back(y);
	Query z=Query();
	z.enable=false;
	querys.push_back(z);
	vector<string> datas;
	datas=split(pa.get<std::string>("infiles"),',');
	if(datas.size()>3)
	{
		cerr<<"More than 3 data"<<endl;
		return 0;
	}
	while(datas.size()<3)
		datas.push_back("none");
	if(pa.get<std::string>("querylist")!="false")
	{
		load_query_list(pa.get<std::string>("querylist"));
		//cerr<<"!!!!!!!!!!!!!!"<<query_list.size();
	}
	int data_cnt=0;
	for(int i=0;i<3;++i)
		if(datas[i]!="none")
		{
			++data_cnt;
			if(pa.get<std::string>("load")=="zjq")
				load_data_zjq_181113(datas[i],i+1);
			if(pa.get<std::string>("load")=="clc")
				read_clc_files(datas[i],i+1);
			if(pa.get<std::string>("load")=="ucf")
				read_ucf_files(datas[i],i+1);
			if(pa.get<std::string>("load")=="default")
				read_Data_20170903(datas[i]);
		}
	sort(tim_div.begin(),tim_div.end());
	if(pa.get<std::string>("timediv")=="true")
	{
		for(int i=0;i<=20;++i)
			cerr<<5*i<<"% time is "<<setiosflags(ios::fixed)<<tim_div[max((int)tim_div.size()*i/20-1,0)]<<endl;
		for(int i=0;i<=20;++i)
			cerr<<setiosflags(ios::fixed)<<tim_div[max((int)tim_div.size()*i/20-1,0)]<<",";
		cerr<<endl;
	}
	cerr<<"***************\n";
	double divide_time=stof(pa.get<std::string>("divide"));
	cerr<<pa.get<std::string>("divide")<<"AAAA"<<divide_time<<endl;
	if(divide_time>0.01&&divide_time<0.99)
		divide_time=tim_div[max((int)(tim_div.size()*divide_time-1),0)];
	if(divide_time>0.1)
		divide(divide_time,stoi(pa.get<std::string>("sessioncount")));
	cerr<<"##########################\n";
	if(pa.get<std::string>("feature")!="false")
	{
		load_zjq_feature(pa.get<std::string>("feature"));
	}
	if(pa.get<std::string>("usetrained")=="false")
	{
		/*if(pa.get<std::string>("load")=="zjq") 
			load_data_zjq_181113(data_dir);
		if(pa.get<std::string>("load")=="clcd"||pa.get<std::string>("load")=="clc")
			read_clc(pa.get<std::string>("load")=="clcd");*/
		//return 0;
		if(divide_time<1&&pa.get<std::string>("filter")=="true")
			Data_Filter();
		vector<string> savekey=split(pa.get<std::string>("save"),',');
		if(find(savekey.begin(),savekey.end(),"clc")!=savekey.end())
		{
			save_as_clc();//TODIV
		}
		if(find(savekey.begin(),savekey.end(),"ucf")!=savekey.end())
		{
			if(divide_time<0.1)
				save_as_ucf(pa.get<std::string>("outdir"));
			else
			{
				save_as_ucf(pa.get<std::string>("outdir")+"_train",1);
				save_as_ucf(pa.get<std::string>("outdir")+"_val",3);
				save_as_ucf(pa.get<std::string>("outdir")+"_test",2);
			}
		}
	}
	/*if(pa.get<std::string>("load")=="clcd"&&pa.get<std::string>("usetrained")=="true")
		read_clc(pa.get<std::string>("load")=="clcd",false);*/
	cerr<<"Format error: "<<Filter[0]<<endl;
	cerr<<"Head Format error: "<<Filter[1]<<endl;
	cerr<<"Not first page: "<<Filter[2]<<endl;
	cerr<<"More than "<<MAXDOCPERPAGE<<" docs' sessions: "<<Filter[3]<<endl;
	cerr<<"Less than "<<MINDOCPERPAGE<<" docs' sessions: "<<Filter[4]<<endl;
	cerr<<"Sessions in query with too few sessions: "<<Filter[6]<<endl;
	cerr<<"Sessions in query with too much sessions: "<<Filter[5]<<endl;
	cerr<<"Querys with too FEW sessions: "<<qFilter[1]<<endl;
	cerr<<"Querys with too MORE sessions: "<<qFilter[0]<<endl;
	cerr<<"Ok Sessions: "<<sessions.size()-Filter[5]-Filter[6]<<endl;
	cerr<<"Ok querys: "<<querys.size()-qFilter[0]-qFilter[1]<<endl;
	cerr<<"All docs num: "<<docs.size()<<endl;
	vector<string> mods=split(pa.get<std::string>("module"),',');
	int verticle_num[20][DOCPERPAGE+2];
	memset(verticle_num,0,sizeof(verticle_num));
	//int cnt_1=0,cnt_no_1=0;
	int V,Vc;
	cerr<<querys.size()<<endl;
	vector<pair<int,double>> tmpp;
	FILE* tmp_f;
	data_mining();
	if(find(mods.begin(),mods.end(),"baseline")!=mods.end())
	{
		baseline baseline_mod=baseline();
		if(datas[0]!="none")
			baseline_mod.train();
		else
			baseline_mod.load();
		
		if(pa.get<std::string>("sample")=="testdata")
		{
			baseline_mod.sample_testdata();//TODO: xxx
		}
		else
		{
			baseline_mod.test();
		}
		if(pa.get<std::string>("typetest")=="true")
		{
			cout<<"Type test baseline:\n";
			for(int i=0;i<=DOCPERPAGE;++i)
				cout<<baseline_mod.test(false,2,i)<<"\t";
			cout<<endl;
		}
		baseline_mod.dump_rel();
		if(pa.get<std::string>("sample")=="true")
			baseline_mod.sample();
		#ifdef DEBUG
			baseline_mod.check();
		#endif
	}
	if(find(mods.begin(),mods.end(),"dcm")!=mods.end())
	{
		dcm dcm_mod=dcm();
		if(datas[0]!="none")
			dcm_mod.train();
		else
			dcm_mod.load();
		if(pa.get<std::string>("sample")=="testdata")
		{
			dcm_mod.sample_testdata();//TODO: xxx
		}
		else
		{
			dcm_mod.test();
		}
		if(pa.get<std::string>("typetest")=="true")
		{
			cout<<"Type test dcm:\n";
			for(int i=0;i<=DOCPERPAGE;++i)
				cout<<dcm_mod.test(false,2,i)<<"\t";
			cout<<endl;
		}
		dcm_mod.dump_rel();
		if(pa.get<std::string>("sample")=="true")
			dcm_mod.sample();
		#ifdef DEBUG
			dcm_mod.check();
		#endif
	}
	if(find(mods.begin(),mods.end(),"ubm")!=mods.end())
	{
		ubm ubm_mod=ubm();
		if(datas[0]!="none")
			ubm_mod.train();
		else
			ubm_mod.load();
		cerr<<"UUU"<<endl;
		if(pa.get<std::string>("sample")=="testdata")
		{
			ubm_mod.sample_testdata();//TODO: xxx
		}
		else
		{
			ubm_mod.test();
		}
		cerr<<"VVV"<<endl;
		if(pa.get<std::string>("typetest")=="true")
		{
			cout<<"Type test ubm:\n";
			for(int i=0;i<=DOCPERPAGE;++i)
				cout<<ubm_mod.test(false,2,i)<<"\t";
			cout<<endl;
		}
		ubm_mod.dump_rel();
		if(pa.get<std::string>("sample")=="true")
			ubm_mod.sample();
		#ifdef DEBUG
			ubm_mod.check();
		#endif
	}
	if(find(mods.begin(),mods.end(),"vcm")!=mods.end())
	{
		vcm vcm_mod=vcm();
		if(datas[0]!="none")
			vcm_mod.train();
		else
			vcm_mod.load();
		
		if(pa.get<std::string>("sample")=="testdata")
		{
			vcm_mod.sample_testdata();//TODO: xxx
		}
		else
		{
			vcm_mod.test();
		}
		if(pa.get<std::string>("typetest")=="true")
		{
			cout<<"Type test ubm:\n";
			for(int i=0;i<=DOCPERPAGE;++i)
				cout<<vcm_mod.test(false,2,i)<<"\t";
			cout<<endl;
		}
		vcm_mod.dump_rel();
		if(pa.get<std::string>("sample")=="true")
			vcm_mod.sample();
		#ifdef DEBUG
			vcm_mod.check();
		#endif
	}
	if(find(mods.begin(),mods.end(),"vcmlayout")!=mods.end())
	{
		vcmlayout vcm_mod=vcmlayout();
		if(datas[0]!="none")
			vcm_mod.train();
		else
			vcm_mod.load();
		
		if(pa.get<std::string>("sample")=="testdata")
		{
			vcm_mod.sample_testdata();//TODO: xxx
		}
		else
		{
			vcm_mod.test();
		}
		if(pa.get<std::string>("typetest")=="true")
		{
			cout<<"Type test ubm:\n";
			for(int i=0;i<=DOCPERPAGE;++i)
				cout<<vcm_mod.test(false,2,i)<<"\t";
			cout<<endl;
		}
		vcm_mod.dump_rel();
		if(pa.get<std::string>("sample")=="true")
			vcm_mod.sample();
		#ifdef DEBUG
			vcm_mod.check();
		#endif
	}
	if(find(mods.begin(),mods.end(),"fcm_ubm")!=mods.end()||find(mods.begin(),mods.end(),"fcm")!=mods.end())
	{
		fcm_ubm ubm_mod=fcm_ubm();
		if(datas[0]!="none")
			ubm_mod.train();
		else
			ubm_mod.load();
		
		if(pa.get<std::string>("sample")=="testdata")
		{
			ubm_mod.sample_testdata();//TODO: xxx
		}
		else
		{
			ubm_mod.test();
		}
		if(pa.get<std::string>("typetest")=="true")
		{
			cout<<"Type test fcm_ubm:\n";
			for(int i=0;i<=DOCPERPAGE;++i)
				cout<<ubm_mod.test(false,2,i)<<"\t";
			cout<<endl;
		}
		ubm_mod.dump_rel();
		if(pa.get<std::string>("sample")=="true")
			ubm_mod.sample();
		#ifdef DEBUG
			ubm_mod.check();
		#endif
	}
	if(find(mods.begin(),mods.end(),"ubmlayout")!=mods.end())
	{
		ubmlayout ubm_mod=ubmlayout();
		if(datas[0]!="none")
			ubm_mod.train();
		else
			ubm_mod.load();
		
		if(pa.get<std::string>("sample")=="testdata")
		{
			ubm_mod.sample_testdata();//TODO: xxx
		}
		else
		{
			ubm_mod.test();
		}
		if(pa.get<std::string>("typetest")=="true")
		{
			cout<<"Type test ubmlayout:\n";
			for(int i=0;i<=DOCPERPAGE;++i)
				cout<<ubm_mod.test(false,2,i)<<"\t";
			cout<<endl;
		}
		ubm_mod.dump_rel();
		if(pa.get<std::string>("sample")=="true")
			ubm_mod.sample();
		#ifdef DEBUG
			ubm_mod.check();
		#endif
	}
	if(find(mods.begin(),mods.end(),"dbn")!=mods.end())
	{
		dbn dbn_mod=dbn();
		if(datas[0]!="none")
			dbn_mod.train();
		else
			dbn_mod.load();
		
		if(pa.get<std::string>("sample")=="testdata")
		{
			dbn_mod.sample_testdata();//TODO: xxx
		}
		else
		{
			dbn_mod.test();
		}
		if(pa.get<std::string>("typetest")=="true")
		{
			cout<<"Type test dbn:\n";
			for(int i=0;i<=DOCPERPAGE;++i)
				cout<<dbn_mod.test(false,2,i)<<"\t";
			cout<<endl;
		}
		dbn_mod.dump_rel();
		if(pa.get<std::string>("sample")=="true")
			dbn_mod.sample();
		#ifdef DEBUG
			dbn_mod.check();
		#endif
	}
	if(find(mods.begin(),mods.end(),"mcm")!=mods.end())
	{
		mcm mcm_mod=mcm();
		if(datas[0]!="none")
			mcm_mod.train();
		else
			mcm_mod.load();
		
		if(pa.get<std::string>("sample")=="testdata")
		{
			mcm_mod.sample_testdata();//TODO: xxx
		}
		else
		{
			mcm_mod.test();
		}
		if(pa.get<std::string>("typetest")=="true")
		{
			cout<<"Type test mcm:\n";
			for(int i=0;i<=DOCPERPAGE;++i)
				cout<<mcm_mod.test(false,2,i)<<"\t";
			cout<<endl;
		}
		mcm_mod.dump_rel();
		if(pa.get<std::string>("sample")=="true")
			mcm_mod.sample();
		#ifdef DEBUG
			mcm_mod.check();
		#endif
	}
	if(find(mods.begin(),mods.end(),"mcm2")!=mods.end())
	{
		mcm2 mcm2_mod=mcm2();
		if(datas[0]!="none")
			mcm2_mod.train();
		else
			mcm2_mod.load();
		
		if(pa.get<std::string>("sample")=="testdata")
		{
			mcm2_mod.sample_testdata();//TODO: xxx
		}
		else
		{
			mcm2_mod.test();
		}
		if(pa.get<std::string>("typetest")=="true")
		{
			cout<<"Type test mcm:\n";
			for(int i=0;i<=DOCPERPAGE;++i)
				cout<<mcm2_mod.test(false,2,i)<<"\t";
			cout<<endl;
		}
		mcm2_mod.dump_rel();
		if(pa.get<std::string>("sample")=="true")
			mcm2_mod.sample();
		#ifdef DEBUG
			mcm2_mod.check();
		#endif
	}
	if(find(mods.begin(),mods.end(),"mcm3")!=mods.end())
	{
		mcm3 mcm2_mod=mcm3();
		if(datas[0]!="none")
			mcm2_mod.train();
		else
			mcm2_mod.load();
		
		if(pa.get<std::string>("sample")=="testdata")
		{
			mcm2_mod.sample_testdata();//TODO: xxx
		}
		else
		{
			mcm2_mod.test();
		}
		if(pa.get<std::string>("typetest")=="true")
		{
			cout<<"Type test mcm:\n";
			for(int i=0;i<=DOCPERPAGE;++i)
				cout<<mcm2_mod.test(false,2,i)<<"\t";
			cout<<endl;
		}
		mcm2_mod.dump_rel();
		if(pa.get<std::string>("sample")=="true")
			mcm2_mod.sample();
		#ifdef DEBUG
			mcm2_mod.check();
		#endif
	}
	cerr<<"Vertical number:="<<MAXVERTICLE<<endl;
	return 0;
} 
