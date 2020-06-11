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
	#ifdef REVISITTEST
	int co[2][12][12],v[2][12];
	memset(co,0,sizeof(co));
	memset(v,0,sizeof(v));
	for(auto &query:querys)
		if(query.enable)
		{
			for(int i=query.last;i>0;i=sessions[i].query_nex)
			{
				Session &sess=sessions[i];
				if(!sess.enable)
					continue;
				for(int i=1;i<=10;++i)
					if(sess.click_time[i]>1)
					{
						++v[(docs[sess.doc_id[i]].type>1)][i];
						bool flag=false;
						for(int j=i+1;j<=10;++j)
							if(sess.click_time[j]>1&&sess.click_time[j]<sess.click_time[i])
							{
								flag=(docs[sess.doc_id[j]].type>1);
								++co[flag][i][j];
								break;
							}
					}
			}
		}
	for(int i=1;i<=10;++i)
	{
		for(int j=1;j<=10;++j)
			cerr<<co[0][i][j]/(co[0][i][j]+co[1][i][j]+0.01)-v[0][j]/(v[0][j]+v[1][j]+0.01)<<",";
		cerr<<endl;
	}
	cerr<<endl;
	for(int i=1;i<=10;++i)
	{
		for(int j=1;j<=10;++j)
			cerr<<co[1][i][j]<<",";
		cerr<<endl;
	}
	#endif
	#ifdef VERTICALTYPECNT
	int type_cnt[MAXVERTICLE+2]={};
	for(int i=0;i<=MAXVERTICLE;++i)
		type_cnt[i]=0;
	for(auto &query:querys)
		if(query.enable)
		{
			Vc=V=0;
			for(int i=query.last;i>0;i=sessions[i].query_nex)
			{
				Session &sess=sessions[i];
				sess.type=0;
				for(int i=1;i<=DOCPERPAGE;++i)
					++type_cnt[docs[sess.doc_id[i]].type];
			}
		}
	for(int i=0;i<=MAXVERTICLE;++i)
	{
		cerr<<type_cnt[i]<<"\t";
	}
	cerr<<endl;
	#endif
	#ifdef VERTICALPOSCNT
	int pos_cnt[DOCPERPAGE+2]={},s_cnt=0;
	for(int i=0;i<=DOCPERPAGE;++i)
		pos_cnt[i]=0;
	for(auto &query:querys)
		if(query.enable)
		{
			Vc=V=0;
			for(int i=query.last;i>0;i=sessions[i].query_nex)
			{
				Session &sess=sessions[i];
				sess.type=0;
				s_cnt+=1;
				for(int i=1;i<=DOCPERPAGE;++i)
					if(docs[sess.doc_id[i]].type>1)
						++pos_cnt[i];
			}
		}
	for(int i=0;i<=DOCPERPAGE;++i)
	{
		cerr<<1.0*pos_cnt[i]/s_cnt<<"\t";
	}
	cerr<<endl;
	#endif
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
	vector<int> ver_click_cnt,nonver_click_cnt;
	ver_click_cnt.resize(MINDOCPERPAGE+1);
	nonver_click_cnt.resize(MINDOCPERPAGE+1);
	int fir_clk,serp_cnt=0,ver_cnt[DOCPERPAGE+2],nonver_cnt[DOCPERPAGE+2];
	for(int i=0;i<=DOCPERPAGE;++i)
	{
		ver_cnt[i]=nonver_cnt[i]=0;
	}
	for(auto &query:querys)
		if(query.enable)
		{
			for(int i=query.last;i>0;i=sessions[i].query_nex)
				if(sessions[i].enable)
				{
					serp_cnt+=1;
					Session &sess=sessions[i];
					fir_clk=-1;
					for(int j=1;j<=DOCPERPAGE;++j)
						if(sess.click_time[j]>.1&&(fir_clk==-1||sess.click_time[j]>sess.click_time[fir_clk]))
						{
							fir_clk=j;
							//break;
						}
					for(int j=1;j<=DOCPERPAGE;++j)
						if(docs[sess.doc_id[j]].type>1)
							ver_cnt[j]+=1;
						else
							nonver_cnt[j]+=1;
					if(docs[sess.doc_id[fir_clk]].type>1)
						++ver_click_cnt[fir_clk];
					else
						++nonver_click_cnt[fir_clk];
				}
		}
	cout<<"NON VERTICAL FIRST CLICK RATE"<<endl;
	for(int i=1;i<=DOCPERPAGE;++i)
	{
		cerr<<nonver_click_cnt[i]*1.0/serp_cnt<<"\t";
	}
	cerr<<endl;
	cout<<"VERTICAL FIRST CLICK RATE"<<endl;
	for(int i=1;i<=DOCPERPAGE;++i)
	{
		cerr<<ver_click_cnt[i]*1.0/serp_cnt<<"\t";
	}
	cerr<<endl;
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
	#ifdef CLKSEQ
	long long seqcnt[DOCPERPAGE+2][1<<DOCPERPAGE+2],sum[DOCPERPAGE+2][1<<DOCPERPAGE+2];//seqcnt[A][B]  A=click_cnt  B=click_seq,0 for Non-V 1 for V
	int clk_c,clk_k;
	long long sess_cnt=0;
	pair<int,double> clk_s[DOCPERPAGE+2];
	memset(seqcnt,0,sizeof(seqcnt));
	for(auto &sess:sessions)
	{
		if(!sess.enable)
			continue;
		++sess_cnt;
		clk_c=0;
		clk_k=0;
		for(int i=1;i<=DOCPERPAGE;++i)
			if(sess.click_time[i]>.1)
				clk_s[++clk_c]=make_pair(docs[sess.doc_id[i]].type==1?0:1,sess.click_time[i]);
		sort(clk_s+1,clk_s+clk_c+1,[](pair<int,double> a, pair<int,double> b) -> bool { return a.second < b.second; });
		for(int i=1;i<=clk_c;++i)
			clk_k=clk_k<<1|clk_s[i].first;
		++seqcnt[clk_c][clk_k];
	}
	memcpy(sum,seqcnt,sizeof(sum));
	for(int i=DOCPERPAGE;i;--i)
	{
		for(int j=0;j<(1<<i);++j)
			sum[i-1][j>>1]+=sum[i][j];
	}
	for(int i=1;i<=4;++i)
	{
		for(int j=0;j<(1<<i);++j)
		if(seqcnt[i][j])
		{
			cerr<<i<<"\t";
			for(int k=i-1;~k;--k)
				if((1<<k)&j)
					cerr<<"1";
				else
					cerr<<"0";
			cerr<<"\t"<<100.*seqcnt[i][j]/sum[i][j]<<endl;
		}
	}
	for(int i=1;i<=4;++i)
	{
		for(int j=0;j<(1<<i);++j)
		if(seqcnt[i][j])
		{
			cerr<<i<<"\t";
			for(int k=i-1;~k;--k)
				if((1<<k)&j)
					cerr<<"1";
				else
					cerr<<"0";
			cerr<<"\t"<<100.*sum[i][j]/sum[i-1][j>>1]<<endl;
		}
	}
	for(int i=1;i<=4;++i)
	{
		for(int j=0;j<(1<<i);++j)
		if(seqcnt[i][j])
		{
			cerr<<i<<"\t";
			for(int k=i-1;~k;--k)
				if((1<<k)&j)
					cerr<<"1";
				else
					cerr<<"0";
			cerr<<"\t"<<100.*sum[i][j]/(sum[i][j]+sum[i][j^1])<<endl;
		}
	}
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
	#ifdef FIRSTCLK
	int i,j;
	int fir_clk[20][5],cnt[20];
	memset(fir_clk,0,sizeof(fir_clk));
	memset(cnt,0,sizeof(cnt));
	for(auto &sess:sessions)
		if(sess.enable)
		{
			int clk_pla=0;
			double clk_time=1e18;
			for(int i=1;i<=4;++i)
				if(sess.click_time[i]>.1&&sess.click_time[i]<clk_time)
				{
					clk_time=sess.click_time[i];
					clk_pla=i;
				}
			int ver=0;
			for(int i=1;i<=4;++i)
			{
				ver<<=1;
				if(docs[sess.doc_id[i]].type!=1)
					ver|=1;
			}
			if(clk_pla<=1)
				continue;
			for(int i=1;i<=10;++i)
				if(sess.click_time[i]>.1&&clk_pla==i)
					++fir_clk[ver][min(4,i)];
			//++fir_clk[ver][clk_pla];
			++cnt[ver];
		}
	for(i=0;i<20;++i)
		if(cnt[i]>100)
		{
			for(j=3;~j;--j)
				if(i&(1<<j))
					cerr<<"1";
				else
					cerr<<"0";
			cerr<<"\t"<<cnt[i];
			for(j=0;j<5;++j)
				cerr<<"\t"<<1.*fir_clk[i][j]/cnt[i];
			cerr<<endl;
		}
	#endif
	#ifdef CLKSEQ
	int cnt[20][2][2][2]={};
	for(auto &sess:sessions)
		if(sess.enable)
		{
			for(int i=1;i<=8;++i)
				if(sess.click_time[i]>.1&&sess.click_time[i+2]>.1)
				{
					++cnt[i][docs[sess.doc_id[i]].type>1][docs[sess.doc_id[i+2]].type>1][sess.click_time[i]>sess.click_time[i+2]];
				}
		}
	int i,j,k,s;
	for(i=1;i<=8;++i)
	{
		for(j=0;j<=1;++j)
		for(k=0;k<=1;++k)
		for(s=0;s<=1;++s)
			cerr<<cnt[i][j][k][s]<<"\t";
		cerr<<endl;
	}
	#endif
}