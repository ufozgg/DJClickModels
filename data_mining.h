#include"config.h"
#include"data_struct.h"
bool cmp_sess_cnt(pair<int,double> &c1,pair<int,double> &c2)
{
    return querys[c1.first].sess_cnt<querys[c2.first].sess_cnt;
}
void data_mining()
{
	int verticle_num[20][DOCPERPAGE+2];
	memset(verticle_num,0,sizeof(verticle_num));
	//int cnt_1=0,cnt_no_1=0;
	int V,Vc;
	cerr<<querys.size()<<endl;
	vector<pair<int,double>> tmpp;
	FILE* tmp_f;
	#ifdef VERTICALCNT
	for(auto &query:querys)
		if(query.enable)
		{
			Vc=V=0;
			for(int i=query.last;i>0;i=sessions[i].query_nex)
			{
				Session &sess=sessions[i];
				sess.type=0;
				for(int i=1;i<=DOCPERPAGE;++i)
					if(docs[sess.doc_id[i]].type!=1)
						++sess.type;
				sess.type;
				if(sess.enable&&sess.click_cnt)
				{
					V+=sess.type;
					++Vc;
				}
			}
			//cerr<<Vc;
			if(Vc)
			{
				//cerr<<Vc<<" "<<V<<endl;
				tmpp.push_back(make_pair(Vc,(double)V/Vc));
				++verticle_num[min(10,(int)(log(Vc)/log(10)))][V/Vc];
			}
		}
	sort(tmpp.begin(),tmpp.end());
	tmp_f=fopen("../output/vertical_count","w");
	for(auto i:tmpp)
		fprintf(tmp_f,"%d %lf\n",i.first,i.second);
	fclose(tmp_f);
    cout<<"Verticle number cnt"<<endl;
    string kind_name[4]={"None","Train","Test","Val"};
    for(int k=0;k<=10;++k)
    {
        //cout<<kind_name[k]<<" :\t";
        for(int i=0;i<=DOCPERPAGE;++i)
            cout<<verticle_num[k][i]<<"\t";
        cout<<endl;
    }
	cerr<<querys.size()<<endl;
	tmpp.clear();
	#endif
	#ifdef VERTICALCNTNAME
	for(int x=0;x<querys.size();++x)
    {
        auto &query=querys[x];
		if(query.enable)
		{
			Vc=V=0;
			for(int i=query.last;i>0;i=sessions[i].query_nex)
			{
				Session &sess=sessions[i];
				sess.type=0;
				for(int i=1;i<=DOCPERPAGE;++i)
					if(docs[sess.doc_id[i]].type!=1)
						++sess.type;
				sess.type;
				if(sess.enable&&sess.click_cnt)
				{
					V+=sess.type;
					++Vc;
				}
			}
			//cerr<<Vc;
			if(Vc)
			{
				//cerr<<Vc<<" "<<V<<endl;
                query.sess_cnt=Vc;
				tmpp.push_back(make_pair(x,(double)V/Vc));
				++verticle_num[min(10,(int)(log(Vc)/log(10)))][V/Vc];
			}
		}
    }
	sort(tmpp.begin(),tmpp.end(),cmp_sess_cnt);
	tmp_f=fopen("../output/vertical_count_name","w");
	for(auto i:tmpp)
		fprintf(tmp_f,"%s %d %lf\n",querys[i.first].name.c_str(),querys[i.first].sess_cnt,i.second);
	fclose(tmp_f);
    cout<<"Verticle number cnt"<<endl;
    string kind_name[4]={"None","Train","Test","Val"};
    for(int k=0;k<=10;++k)
    {
        //cout<<kind_name[k]<<" :\t";
        for(int i=0;i<=DOCPERPAGE;++i)
            cout<<verticle_num[k][i]<<"\t";
        cout<<endl;
    }
	cerr<<querys.size()<<endl;
	tmpp.clear();
	#endif
	#ifdef CLICKCNT
	for(auto &query:querys)
		if(query.enable)
		{
			Vc=V=0;
			for(int i=query.last;i>0;i=sessions[i].query_nex)
			{
				Vc+=sessions[i].click_cnt;
				++V;
			}
			if(V)
				tmpp.push_back(make_pair(V,(double)Vc/V));
		}
	sort(tmpp.begin(),tmpp.end());
	tmp_f=fopen("../output/click_count","w");
	for(auto i:tmpp)
		fprintf(tmp_f,"%d %lf\n",i.first,i.second);
	fclose(tmp_f);
	#endif
	#ifdef VERTICALTYPE
	int last_clk;
	vector<vector<int>> cnt,cnt2;
	cnt.resize(MAXVERTICLE+1);
	cnt2.resize(MAXVERTICLE+1);
	for(int i=0;i<=MAXVERTICLE;++i)
	{
		cnt[i].resize(MAXVERTICLE+1);
		cnt2[i].resize(MAXVERTICLE+1);
	}
	for(auto &query:querys)
		if(query.enable)
		{
			for(int i=query.last;i>0;i=sessions[i].query_nex)
				if(sessions[i].enable)
				{
					last_clk=0;
					Session &sess=sessions[i];
					for(int j=1;j<=DOCPERPAGE;++j)
						if(sess.click_time[j]>.1)
						{
							++cnt[last_clk][docs[sess.doc_id[j]].type];
							last_clk=docs[sess.doc_id[j]].type;
						}
						else
						{
							++cnt2[last_clk][docs[sess.doc_id[j]].type];
						}
						
				}
		}
	cout<<"CLICK RATE OF B for vertical pair (A,B)"<<endl;
	for(int i=0;i<MAXVERTICLE;++i)
	{
		for(int j=0;j<MAXVERTICLE;++j)
			cout<<cnt[i][j]*1000/(1+cnt[i][j]+cnt2[i][j])<<" ";
		cout<<endl;
	}
	cout<<"======================================"<<endl;
	#endif
	#ifdef FIRSTCLICK
	vector<int> click_cnt;
	click_cnt.resize(MINDOCPERPAGE+1);
	for(auto &query:querys)
		if(query.enable)
		{
			for(int i=query.last;i>0;i=sessions[i].query_nex)
				if(sessions[i].enable)
				{
					Session &sess=sessions[i];
					for(int j=1;j<=DOCPERPAGE;++j)
						if(sess.click_time[j]>.1)
						{
							++click_cnt[j];
							break;
						}
				}
		}
	cout<<"FIRST CLICK"<<endl;
	for(int i=0;i<=DOCPERPAGE;++i)
	{
		cerr<<click_cnt[i]<<endl;
	}
	cout<<"======================================"<<endl;
	#endif
	#ifdef VERTICALBEFOREFIRSTCLICK
	vector<int> vbfc;
	int cc=0;
	vbfc.resize(MINDOCPERPAGE+1);
	for(auto &query:querys)
		if(query.enable)
		{
			for(int i=query.last;i>0;i=sessions[i].query_nex)
				if(sessions[i].enable)
				{
					cc=0;
					Session &sess=sessions[i];
					for(int j=1;j<=DOCPERPAGE;++j)
						if(sess.click_time[j]>.1)
						{
							++vbfc[cc];
							break;
						}
						else
							if(docs[sess.doc_id[j]].type!=1)
								++cc;
						
				}
		}
	cout<<"VERTICAL BEFORE FIRST CLICK"<<endl;
	for(int i=0;i<=DOCPERPAGE;++i)
	{
		cerr<<vbfc[i]<<endl;
	}
	cout<<"======================================"<<endl;
	#endif
    #ifdef VOFFIRST3RES
    int cnt[10]={};
    for(auto &query:querys)
		if(query.enable)
		{
			for(int i=query.last;i>0;i=sessions[i].query_nex)
				if(sessions[i].enable)
				{
					Session &sess=sessions[i];
                    ++cnt[(docs[sess.doc_id[1]].type!=1)*4+(docs[sess.doc_id[2]].type!=1)*2+(docs[sess.doc_id[3]].type!=1)];
				}
		}
    cout<<"Vertical of first 3 docs."<<endl;
    for(int i=0;i<8;++i)
        cout<<cnt[i]<<" ";
    cout<<endl;
    cout<<"========================="<<endl;
    #endif
    #ifdef VOFFIRST4RES
    int cnt[20]={};
    int clk_cnt[20][5];
    memset(clk_cnt,0,sizeof(clk_cnt));
    for(auto &query:querys)
		if(query.enable)
		{
			for(int i=query.last;i>0;i=sessions[i].query_nex)
				if(sessions[i].enable)
				{
					Session &sess=sessions[i];
                    ++cnt[(docs[sess.doc_id[1]].type!=1)*8+(docs[sess.doc_id[2]].type!=1)*4+(docs[sess.doc_id[3]].type!=1)*2+(docs[sess.doc_id[4]].type!=1)];
                    for(int j=1;j<=4;++j)
                        if(sess.click_time[j]>.1)
                            ++clk_cnt[(docs[sess.doc_id[1]].type!=1)*8+(docs[sess.doc_id[2]].type!=1)*4+(docs[sess.doc_id[3]].type!=1)*2+(docs[sess.doc_id[4]].type!=1)][j];
				}
		}
    cout<<"Vertical of first 4 docs."<<endl;
    for(int i=0;i<16;++i)
        cout<<cnt[i]<<" ";
    cout<<"CLICK CNT:"<<endl;
    for(int j=1;j<=4;++j)
    {
        for(int i=0;i<16;++i)
            cout<<clk_cnt[i][j]<<" ";
        cout<<endl;
    }
    cout<<"========================="<<endl;
    #endif
}