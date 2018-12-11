#ifndef DUMP_H
#define DUMP_H
bool chk(int cnt,Session &sess)
{
	return IFDIV&&istest(sess,cnt);
}
void save_as_clc()
{
	int cnt=0,num=0;
	sprintf(tmp,"%05d",num);
	string file_name,file_name2;
	FILE* sav,*sav2;
	if(IFDIV)
	{
		file_name=save_file+tmp+"_train.clc";
		cout<<"DUMP to "<<file_name<<endl;
		sav=fopen(file_name.data(),"w");
		file_name2=save_file+tmp+"_test.clc";
		cout<<"DUMP to "<<file_name2<<endl;
		sav2=fopen(file_name2.data(),"w");
	}
	else
	{
		if(SAVEBLOCK)
			file_name=save_file+tmp+".clc";
		else
			file_name=save_file+".clc";
		cout<<"DUMP to "<<file_name<<endl;
		sav=fopen(file_name.data(),"w");
	}
	for(auto &j:querys)
	{
		for(int w=j.last;w>=0;)
		{
			Session &i=sessions[w];
			if(!i.enable)
			{
				w=i.query_nex;
				continue;
			}
			++cnt;
			fprintf(chk(cnt,i)?sav2:sav,"%s\t",querys[i.query_id].name.data());
			//fprintf(sav,"UFOZGG#%s#1#%.3lf#%s",users[i.user_id].name.data(),i.begin_time,i.ip.data());
			//for(int j=1;j<=DOCPERPAGE;++j)
			//	fprintf(chk(cnt,i)?sav2:sav,"%d%c",i.doc_id[j],j==DOCPERPAGE?'\t':' ');
			for(int j=1;j<=DOCPERPAGE;++j)
				fprintf(chk(cnt,i)?sav2:sav,"%s%c",split(docs[i.doc_id[j]].name.data(),'#')[1].data(),j==DOCPERPAGE?'\t':' ');
			for(int j=1;j<=DOCPERPAGE;++j)
				fprintf(chk(cnt,i)?sav2:sav,"%d%c",i.click_time[j]>0?1:0,j==DOCPERPAGE?'\t':' ');
			for(int j=1;j<=DOCPERPAGE;++j)
				if(i.click_time[j]>.1)
					fprintf(chk(cnt,i)?sav2:sav,"%.3lf%c",i.click_time[j],j==DOCPERPAGE?'\t':' ');
				else
					fprintf(chk(cnt,i)?sav2:sav,"-1%c",j==DOCPERPAGE?'\t':' ');
			for(int j=1;j<=DOCPERPAGE;++j)
				fprintf(chk(cnt,i)?sav2:sav,"0#0%c",j==DOCPERPAGE?'\t':' ');
				fprintf(chk(cnt,i)?sav2:sav,"1\t%.3lf\t1\n",i.begin_time);
				if(IFDIV==false&&SAVEBLOCK&&cnt==SAVEBLOCK)
				{
					cnt=0;
					fclose(sav);
					++num;
					sprintf(tmp,"%05d",num);
					file_name=save_file+tmp;
					sav=fopen(file_name.data(),"w");
				}
			w=i.query_nex;
		}
	}
	fclose(sav);
}
#endif
