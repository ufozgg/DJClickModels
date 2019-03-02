#ifndef DATA_FILTER_H
#define DATA_FILTER_H
void Data_Filter()
{
	static int cnt2=0;
	for(auto &i :querys)
	{
		//cerr<<"SXXXXXXXXSXXXXX\t"<<i.name<<"\t"<<i.sess_cnt<<"\t"<<querys.size()<<endl;
		if(i.sess_cnt>MAXSESSION)
		{
			++qFilter[0];
			i.enable=0;
			for(int j=i.last;j!=-1;j=sessions[j].query_nex)
			{
				if(sessions[j].enable)
					Filter[5]++;
				sessions[j].enable=0;
			}
			continue;
		}
		if(i.sess_cnt<MINSESSION)
		{
			if(i.sess_cnt)
				++qFilter[1];
			i.enable=0;
			for(int j=i.last;j!=-1;j=sessions[j].query_nex)
			{
				if(sessions[j].enable)
					Filter[6]++;
				sessions[j].enable=0;
			}
			continue;
		}
		i.enable=1;
	}
	int cnt=0;
	for(auto &j:querys)
		if(j.sess_cnt>=MINSESSION&&j.sess_cnt<=MAXSESSION)
		{
			for(int w=j.last;w>0;)
			{
				Session &i=sessions[w];
				if(!i.enable)
				{
					w=i.query_nex;
					continue;
				}
				if(i.kind!=0)
				{
					w=i.query_nex;
					continue;
				}
				++cnt;
				if(istest(i,cnt))
					sessions[w].kind=2;
				else
				{
					++cnt2;
					//if(VALI&&cnt2%VALI==0)
					//	sessions[w].kind=3;
					//else
						sessions[w].kind=1;
				}
			}
		}
}
#endif
