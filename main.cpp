/*************
Auther: THUIR Ruizhe Zhang
*************/
#pragma GCC optimize("delete-null-pointer-checks,inline-functions-called-once,expensive-optimizations,optimize-sibling-calls,tree-switch-conversion,inline-small-functions,rerun-cse-after-loop,hoist-adjacent-loads,indirect-inlining,reorder-functions,no-stack-protector,partial-inlining,sched-interblock,cse-follow-jumps,align-functions,strict-aliasing,schedule-insns2,tree-tail-merge,inline-functions,schedule-insns,reorder-blocks,unroll-loops,thread-jumps,crossjumping,caller-saves,devirtualize,align-labels,align-loops,align-jumps,unroll-loops,sched-spec,inline,gcse,gcse-lm,ipa-sra,tree-pre,tree-vrp,peephole2,fastmath,Ofast,-Ofast")//,fastmath,Ofast,-Ofast
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
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
#include"dbn.h"
#include"mcm.h"
#include"ubmlayout.h"
#include"load_feature.h"
using namespace std;
int main(int argc,char* argv[])
{
	cmdline::parser pa;
	assert(MINSESSION<=MAXSESSION);
	pa.add<std::string>("module",'m',"module name such as \"ubm\"",false,"");
	pa.add<std::string>("save",'s',"format: \"clc\"",false,"",cmdline::oneof<std::string>("clc","zrz"));
	pa.add<std::string>("load",'l',"format: \"clc\" or \"zjq\" or \"default\"",false,"default",cmdline::oneof<std::string>("clc","zjq","default"));
	pa.add<std::string>("infiles",'i',"input files, none for no file, traindata,testdata,validata split by ','",false,"");
	pa.add<std::string>("outdir",'o',"format: a dir",false,"");
	pa.add<std::string>("filter",'f',"filter or not",false,"true");
	pa.add<std::string>("sample",'S',"sample or not",false,"false");
	pa.add<std::string>("feature",'F',"load feature or not",false,"false");
	pa.add<std::string>("vertical",'V',"Vertical_type_file_name",false,"false");
	pa.add<std::string>("typetest",'t',"test by type",false,"false");
	pa.add<std::string>("round",'r',"round for models",false,"100");
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
	for(int i=0;i<3;++i)
		if(datas[i]!="none")
		{
			if(pa.get<std::string>("load")=="zjq")
				load_data_zjq_181113(datas[i],i+1);
			if(pa.get<std::string>("load")=="clc")
				read_clc_files(datas[i],i+1);
			if(pa.get<std::string>("load")=="default")
				read_Data_20170903(datas[i]);
		}
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
		if(pa.get<std::string>("filter")=="true")
			Data_Filter();
		vector<string> savekey=split(pa.get<std::string>("save"),',');
		if(find(savekey.begin(),savekey.end(),"clc")!=savekey.end())
			save_as_clc();
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
	#ifdef ZRZ
	int verticle_num[DOCPERPAGE+2]={0};
	//int cnt_1=0,cnt_no_1=0;
    for(auto &sess:sessions)
    {
        sess.type=0;
        for(int i=1;i<=DOCPERPAGE;++i)
            if(docs[sess.doc_id[i]].type!=1)
                ++sess.type;
        if(sess.enable&&sess.click_cnt)
            ++verticle_num[sess.type];
    }
    cout<<"Verticle number cnt"<<endl;
    string kind_name[4]={"None","Train","Test","Val"};
//    for(int k=0;k<=3;++k)
    {
        //cout<<kind_name[k]<<" :\t";
        for(int i=0;i<=DOCPERPAGE;++i)
            cout<<verticle_num[i]<<"\t";
        cout<<endl;
    }
	return 0;
	#endif
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
	cerr<<"Vertical number:="<<MAXVERTICLE<<endl;
	return 0;
} 
