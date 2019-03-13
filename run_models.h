#include"common.h"
void run_models(vector<string> mods)
{
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

}