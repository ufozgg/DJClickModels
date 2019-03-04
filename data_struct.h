#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include"config.h"
#include"common.h"
using namespace std;
int doc_per_page[MAXDOCPERPAGE+1];
int qFilter[10];
int Filter[10];
unordered_map<int,int> v_type;
vector<double> tim_div;
class Doc
{
    public:
        string name;
        int last_sess,last_pos;
		int train_tim,test_tim;
		int type;
        Doc()
        {
            name="";
			train_tim=test_tim=0;
            last_sess=last_pos=-1;
			type=0;
        }
};
class Session
{
    public:
        int id,click_cnt,enable,kind;//kind=1(train) kind=2(test) kind=3(vali)
        string ip;
        double begin_time,click_time[MINDOCPERPAGE+2];
        int doc_id[MINDOCPERPAGE+2],nex_sess[MINDOCPERPAGE+2],nex_pos[MINDOCPERPAGE+2];
        int user_id,user_nex,query_id,query_nex;
		int type;
    Session()
    {
        click_cnt=0;
		kind=0;
		enable=1;
        memset(click_time,0,sizeof(click_time));
        memset(doc_id,0,sizeof(doc_id));
		type=0;
    }
};
class Query
{
    public:
        string name;
        int last,enable;
        int sess_cnt;
		int type;
        Query()
        {
            name="";
            sess_cnt=0;
            last=-1;
			type=0;
        }
};
class User
{
    public:
        string name;
        int last;
};
vector<Session> sessions;
vector<Query> querys;
vector<Doc> docs;
vector<User> users;
unordered_map<string,int> user_name2id;
unordered_map<string,int> qry_name2id;
unordered_map<string,int> doc_name2id;
unordered_map<string,int> feature_name2id;
unordered_map<string,bool> query_list;
void uidadd(string uid,Session &now);
bool qryadd(string qry_w,Session &now);
void addDoc(string doc_name,Session &now,int pos,int ifclick,double clicktime,int ty);
void line_Data_20170903(const string &line);
void read_Data_20170903();
char tmp[MAXLINELEN];
void uidadd(string uid,Session &now)
{
	int w=doc_name2id[uid];
	if(w==0)
	{
		w=users.size();
		User e;
		e.name=uid;
		e.last=-1;
		users.push_back(e);
		user_name2id[uid]=w;
	}
	now.user_id=w;
	now.user_nex=users[w].last;
	users[w].last=now.id;
}
bool qryadd(string qry_w,Session &now)
{
	/*static int cntt=0;
	static string name="";
	if(name!=qry_w)
	{
		if(cntt)
			cout<<cntt<<"\t"<<name<<endl;
		cntt=1;
		name=qry_w;
	}
	else
		++cntt;
	return false;*/
	if(query_list.size()>0&&query_list[qry_w]==false)
		return false;
	int w=qry_name2id[qry_w];
	if(w==0)
	{
		w=querys.size();
		Query q;
		q.name=qry_w;
		q.last=-1;
		q.enable=true;
		querys.push_back(q);
		qry_name2id[qry_w]=w;
	}
	now.query_id=w;
	now.query_nex=querys[w].last;
	querys[w].last=now.id;
	return true;
}
void addDoc(string doc_name,Session &now,int pos,int ifclick,double clicktime,int ty=0)
{
	doc_name=querys[now.query_id].name+"#"+doc_name;
	int w=doc_name2id[doc_name];
	if(w==0)
	{
		w=docs.size();
		Doc d;
		d.name=doc_name;
		d.last_sess=d.last_pos=-1;
		d.type=ty;
		if(d.type>=MAXVERTICLE)
            MAXVERTICLE=d.type+1;
		docs.push_back(d);
		doc_name2id[doc_name]=w;
	}
	now.doc_id[pos]=w;
	now.nex_sess[pos]=docs[w].last_sess;
	now.nex_pos[pos]=docs[w].last_pos;
	docs[w].last_sess=now.id;
	docs[w].last_pos=pos;
	if(ifclick)
	{
		now.click_time[pos]=clicktime;
		++now.click_cnt;
	}
	else
		now.click_time[pos]=0;
}
int maxx;
int sp,sp2;
void line_Data_20170903(const string &line)
{
	//cout<<line<<endl;
	vector<string> res = split(line, '\t');
	vector<string> sess_info;
	int cnt=0,v;
	Session now;
	int now_id=sessions.size();
	//cout<<"W "<<res.size()<<" XSD";
	if(res.size()%4!=2)
	{
		++Filter[0];
		return;
	}
	if(res.size()>MAXDOCPERPAGE*4+2)
	{
		++Filter[3];
		return;
	}
	if(res.size()<MINDOCPERPAGE*4+2)
	{
		++Filter[4];
		return;
	}
	//UUID(nouse)#USER_ID#PAGE_NUM#TIME#IPaddress
	sess_info=split(res[0],'#');
	if(sess_info.size()!=5)
	{
		++Filter[1];
		//cout<<"[Warning]Ignore "<<i<<endl;
		return;
	}
	if(sess_info[2]!="1")
	{
		++Filter[2];
		//cout<<"[Warning]Not first page,Ignore "<<i<<endl;
		return;
	}
	now=Session();
	now.begin_time=atof(sess_info[3].data());
	now.id=now_id;
	if(now.begin_time<1000000000||now.begin_time>1800000000)
		return;
	if(!qryadd(res[1],now))
		return;
	now.id=sessions.size();
	uidadd(sess_info[1],now);
	tim_div.push_back(now.begin_time);
	now.ip=sess_info[4];

	int ty;
	//now.type=0;
	for(int i=1;i<=DOCPERPAGE;++i)
	{
		vector<string> e=split(res[i*4+1],'#');
		if(e.size()>=2)
		{
			sp+=(stoi(e[0])==-1);
			sp2+=(stoi(e[0])!=-1);
			if(v_type.count(stoi(e[0]))>0)
				ty=v_type[stoi(e[0])];
			else
			{
				ty=1;
				//cerr<<e[0]<<endl;
			}
		}
		else
			ty=19;//UNKNOW
		addDoc(res[i*4-2],now,i,atoi(res[i*4-1].data()),atof(res[i*4].data()),ty);
		//now.type+=doc[now.doc_id[i]].type;
	}
	
	++doc_per_page[res.size()>>2];
	if(now.click_cnt>=MINCLICK&&now.click_cnt<=MAXCLICK)
	{
		++querys[now.query_id].sess_cnt;
		now.enable=1;
	}
	else
	{
		now.enable=0;
		++Filter[7];
	}
	sessions.push_back(now);
	//exit(0);
}
void read_Data_20170903(string w)
{
	int i,cnt=0,len;
	fstream infile;
	string line;
	string file_prefix=w.substr(0,w.size()-5),file_name;
	int st=stoi(w.substr(w.size()-5,5));
	for(i=0;i<MAXFILECNT;++i)
	{
		sprintf(tmp,"%05d",i+st);
		file_name=file_prefix+tmp;
		infile.open(file_name.data(),ios::in);
		if(!infile)
			return;
		cout<<"Open file : "<<file_name.data()<<endl;
		cnt=0;
		while(infile)
		{
			infile.getline(tmp,MAXLINELEN);
			line_Data_20170903(tmp);
			++cnt;
		}
		cout<<"Read "<<cnt<<" Lines\n";
		infile.close();
	}
	printf("D\t%d %d\n",sp,sp2);
}
void v_line(const string &w)
{
	vector<string> ww;
	ww=split(w,'\t');
	if(ww.size()<2)
		return;
	int ty=stoi(ww[1]),num=0;
	for(auto c:ww[0])
	{
		if('0'<=c&&c<='9')
		{
			num=num*10+c-'0';
		}
		else
		{
			v_type[num]=ty;
			num=0;
		}
	}
	if(num)
		v_type[num]=ty;
}
void load_vertical_type(string file_name)
{
	fstream infile;
	infile.open(file_name.data(),ios::in);
	if(!infile)
		return;
	int cnt=0;
	cout<<"Open file : "<<file_name.data()<<endl;
	while(infile)
	{
		infile.getline(tmp,MAXLINELEN);
		v_line(tmp);
		++cnt;
	}
	v_type[-1]=1;
	cout<<"v type : "<<cnt<<endl;
}
vector<pain<double,int>> div_vec;
void divide(double div_time,int sess_lim=1000000000)
{
	if(div_time<0.1)
	{
		assert(0);//ARGS ERROR
		return;
	}
	int a,b,A,B;
	A=B=0;
	double C;
	for(int i=0;i<querys.size();++i)
	{
		a=b=0;
		for(int j=querys[i].last;j>0;)
		{
			Session &sess=sessions[j];
			if(sess.enable==0)
			{
				j=sess.query_nex;
				continue;
			}
			if(sess.begin_time<div_time)
				++a;
			else
				++b;
			j=sess.query_nex;
		}
		if(a&&b)
		{
			A+=a;
			B+=b;
			div_vec.push_back(make_pair(1.0*a/(a+b),i));
		}
	}
	C=A/(A+B);
	for(auto &x:div_vec)
	{
		x.first=max((x.first-C)/(1.0-C),(C-x.first)/C);
	}
	sort(div_vec.begin(),div_vec.end());
	int flag=0;//give kind
	
}
#endif
