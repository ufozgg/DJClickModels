#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include"config.h"
#include"common.h"
using namespace std;
int doc_per_page[MAXDOCPERPAGE+1];
int qFilter[10];
int Filter[10];
class Doc
{
    public:
        string name;
        int last_sess,last_pos;
		int train_tim,test_tim;
        Doc()
        {
            name="";
			train_tim=test_tim=0;
            last_sess=last_pos=-1;
        }
};
class Session
{
    public:
        int id,click_cnt,enable,kind;
        string ip;
        double begin_time,click_time[MINDOCPERPAGE+2];
        int doc_id[MINDOCPERPAGE+2],nex_sess[MINDOCPERPAGE+2],nex_pos[MINDOCPERPAGE+2];
        int user_id,user_nex,query_id,query_nex;
    Session()
    {
        click_cnt=0;
		kind=0;
		enable=1;
        memset(click_time,0,sizeof(click_time));
        memset(doc_id,0,sizeof(doc_id));
    }
};
class Query
{
    public:
        string name;
        int last,enable;
        int sess_cnt;
        Query()
        {
            name="";
            sess_cnt=0;
            last=-1;
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
void uidadd(string uid,Session &now);
void qryadd(string qry_w,Session &now);
void addDoc(string doc_name,Session &now,int pos,int ifclick,double clicktime);
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
void qryadd(string qry_w,Session &now)
{
	int w=qry_name2id[qry_w];
	if(w==0)
	{
		w=querys.size();
		Query q;
		q.name=qry_w;
		q.last=-1;
		querys.push_back(q);
		qry_name2id[qry_w]=w;
	}
	now.query_id=w;
	now.query_nex=querys[w].last;
	querys[w].last=now.id;
}
void addDoc(string doc_name,Session &now,int pos,int ifclick,double clicktime)
{
	doc_name=to_string(now.query_id)+"#"+doc_name;
	int w=doc_name2id[doc_name];
	if(w==0)
	{
		w=docs.size();
		Doc d;
		d.name=doc_name;
		d.last_sess=d.last_pos=-1;
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
	now.id=sessions.size();
	uidadd(sess_info[1],now);
	now.begin_time=atof(sess_info[3].data());
	now.ip=sess_info[4];

	qryadd(res[1],now);
	for(int i=1;i<=DOCPERPAGE;++i)
		addDoc(res[i*4-2],now,i,atoi(res[i*4-1].data()),atof(res[i*4].data()));
	
	++doc_per_page[res.size()>>2];
	if(now.click_cnt>=MINCLICK)
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
void read_Data_20170903()
{
	int i,cnt=0,len;
	fstream infile;
	string line;
	string file_prefix=data_dir,file_name;
	for(i=0;i<MAXFILECNT;++i)
	{
		sprintf(tmp,"%05d",i);
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
}
#endif
