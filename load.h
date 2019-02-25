#ifndef LOAD_H
#define LOAD_H
#include "common.h"
#include "data_struct.h"
void line_data_zjq_181113(const string &line,int type)
{
	if(line.size()<=10)
		return;
	vector<string> res = split(line,'\t');
	if(res.size()!=5)
		cout<<line<<endl;
	assert(res.size()==5);
	Session now=Session();
	int now_id=sessions.size();
	now.id=now_id;
	qryadd(res[0],now);
	vector<string> doc = split(res[2],' ');
	assert(doc.size()==10);
	vector<string> clk = split(res[3],' ');
	assert(clk.size()==10);
	for(int i=0;i<=9;++i)
		addDoc(doc[i],now,i+1,clk[i]=="0"?0:1,1.0);
	now.enable=1;
	now.kind=type;
	++querys[now.query_id].sess_cnt;
	sessions.push_back(now);
}

void load_data_zjq_181113(const string &indir,int type)
{
	//FILE *train_in=fopen("../../data/log_data/click_data_train","r");
	fstream infile;
	infile.open(indir.data(),ios::in);
	//infile.open("../../log_data/click_data_test",ios::in);
	//cout<<"Open file : "<<"train"<<endl;
	int cnt=0;
	while(infile)
	{
		infile.getline(tmp,MAXLINELEN);
		line_data_zjq_181113(tmp,type);
		++cnt;
		if(cnt%1000000==0)
			cout<<"Load "<<cnt<<" lines."<<endl;
	}
	cout<<"Load "<<cnt<<" lines\n";
	infile.close();
}
bool line_Data_clc(const string &line,int type)
{
	static int cntt=0;
	vector<string> res = split(line, '\t');
	if(res.size()!=8)
		return 0;
	vector<string> sess_info;
	vector<string> doc_info;
	vector<string> if_click;
	vector<string> click_time_info;
	doc_info=split(res[1],' ');
	if(doc_info.size()!=DOCPERPAGE)
		return 0;
	if_click=split(res[2],' ');
	if(if_click.size()!=DOCPERPAGE)
		return 0;
	click_time_info=split(res[3],' ');
	if(click_time_info.size()!=DOCPERPAGE)
		return 0;
	sess_info=split(res[4],' ');
	if(sess_info.size()!=DOCPERPAGE)
		return 0;
	int cnt=0,v;
	Session now;
	int now_id=sessions.size();
	now=Session();
	now.id=now_id;
	now.begin_time=atof(res[6].data());
	qryadd(res[0],now);
	for(int i=1;i<=DOCPERPAGE;++i)
		addDoc(doc_info[i-1],now,i,atoi(if_click[i-1].data()),atof(click_time_info[i-1].data()));
	if(IFFILTER==0||(now.click_cnt>=MINCLICK&&now.click_cnt<=MAXCLICK))
	{
		++querys[now.query_id].sess_cnt;
		now.enable=1;
		now.kind=type;
		return 1;
	}
	else
	{
		now.enable=0;
		++Filter[7];
	}
	sessions.push_back(now);
	return 0;
}
bool read_clc_file(string file_name,int type=0)
{
	fstream infile;
	infile.open(file_name.data(),ios::in);
	if(!infile)
	{
		cout<<"Open file FAIL: "<<file_name.data()<<endl;
		return false;
	}
	cout<<"Open file : "<<file_name.data()<<endl;
	int cnt=0;
	while(infile)
	{
		infile.getline(tmp,MAXLINELEN);
		if(line_Data_clc(tmp,type))
			++cnt;
	}
	cout<<"Read "<<cnt<<" Lines\n";
	infile.close();
	return true;
}
bool read_clc_files(string file_name,int type=0)
{
	if(file_name.size()<6)
		return read_clc_file(file_name,type);
	for(int i=1;i<=5;++i)
		if(file_name[file_name.size()-i]<'0'||file_name[file_name.size()-i]>'9')
			return read_clc_file(file_name,type);
	file_name=file_name.substr(0,file_name.size()-5);
	string w;
	int i;
	for(i=0;i<MAXFILECNT;++i)
	{
		sprintf(tmp,"%05d",i);
		w=file_name+tmp+".clc";
		if(!read_clc_file(w,type))
			break;
	}
	return i;
}
bool line_Data_ucf(const string &line,int type)
{
	static int cntt=0;
	vector<string> res = split(line, '\t');
	if(res.size()!=2+DOCPERPAGE*3)
		return 0;
	int cnt=0,v;
	Session now;
	int now_id=sessions.size();
	now=Session();
	now.id=now_id;
	now.begin_time=atof(res[1].data());
	qryadd(res[0],now);
	for(int i=1;i<=DOCPERPAGE;++i)
		addDoc(res[i*3-1],now,i,stof(res[i*3+1])>0,stof(res[i*3+1]),stoi(res[i*3]));
	if(IFFILTER==0||(now.click_cnt>=MINCLICK&&now.click_cnt<=MAXCLICK))
	{
		++querys[now.query_id].sess_cnt;
		now.enable=1;
		now.kind=type;
	}
	else
	{
		now.enable=0;
		++Filter[7];
	}
	sessions.push_back(now);
	//return 1;
	return 0;
}
bool read_ucf_file(string file_name,int type=0)
{
	fstream infile;
	infile.open(file_name.data(),ios::in);
	if(!infile)
	{
		cout<<"Open file FAIL: "<<file_name.data()<<endl;
		return false;
	}
	cout<<"Open file : "<<file_name.data()<<endl;
	int cnt=0;
	while(infile)
	{
		infile.getline(tmp,MAXLINELEN);
		if(line_Data_ucf(tmp,type))
			++cnt;
	}
	cout<<"Read "<<cnt<<" Lines\n";
	infile.close();
	return true;
}
bool read_ucf_files(string file_name,int type=0)
{
	if(file_name.size()<6)
		return read_ucf_file(file_name,type);
	for(int i=1;i<=5;++i)
		if(file_name[file_name.size()-i]<'0'||file_name[file_name.size()-i]>'9')
			return read_ucf_file(file_name,type);
	file_name=file_name.substr(0,file_name.size()-5);
	string w;
	int i;
	for(i=0;i<MAXFILECNT;++i)
	{
		sprintf(tmp,"%05d",i);
		w=file_name+tmp+".ucf";
		if(!read_ucf_file(w,type))
			break;
	}
	return i;
}
void div1to3()
{
	assert(0);
	if(VALI==0)
		return;
	vector<Session> sess_tmp;
	for(auto &i:querys)
	{
		sess_tmp.clear();
		for(int j=i.last;j;)
		{
			Session w;
			w=sessions[j];
			sess_tmp.push_back(w);
			j=w.query_nex;
		}
		sort(sess_tmp.begin(),sess_tmp.end(),[](Session &A,Session &B)->bool{if(A.begin_time!=B.begin_time)return A.begin_time<B.begin_time;else return A.id<B.id;});
		for(int j=1;j<=sess_tmp.size()/VALI;++j)
			sessions[sess_tmp[sess_tmp.size()-j].id].kind=3;
	}
}
/*void read_clc(bool clcd,bool train_data=true)
{
	int i,cnt=0,len;
	string line;
	string file_prefix=data_dir,file_name;
	if(clcd)
	{
		if(train_data)
		{
			file_name=file_prefix+"_train.clc";
			if(!read_clc_file(file_name,1))
			{
				cout<<"ERROR: NO TRAIN DATA"<<endl;
				exit(0);
			}
		}
		file_name=file_prefix+"_test.clc";
		if(!read_clc_file(file_name,2))
		{
			cout<<"ERROR: NO TEST DATA"<<endl;
			exit(0);
		}
		if(train_data)
		{
			file_name=file_prefix+"_val.clc";
			if(!read_clc_file(file_name,3))
				div1to3();
		}
	}
	else
	{
		for(i=0;i<MAXFILECNT;++i)
		{
			sprintf(tmp,"%05d",i);
			file_name=file_prefix+tmp+".clc";
			if(!read_clc_file(file_name))
				break;
		}
	}
}*/
#endif

