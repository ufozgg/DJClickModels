#ifndef LOAD_FEATUREH
#define LOAD_FEATUREH
int line_zjq_feature(const string &line)
{
    if(line.size()<10)
        return -1;
    int ret=0,w;
    vector<string> res = split(line,'\t');
    assert(res.size()==9);
    vector<string> doc_names = split(res[1],' ');
    vector<string> doc_features = split(res[6],' ');
    assert(doc_names.size()==doc_features.size());
    for(int i=0;i<doc_names.size();++i)
    {
        string name=res[0]+"#"+doc_names[i];
        w=doc_name2id[name];
        if(w==0)
        {
            w=docs.size();
            Doc d;
            d.name=name;
            docs.push_back(d);
            doc_name2id[name]=w;
        }
        ++ret;
        docs[w].type=atoi(doc_features[i].data());
        if(docs[w].type>=MAXVERTICLE)
            MAXVERTICLE=docs[w].type+1;
    }
    return ret;
}
bool load_zjq_feature(string file_name)
{
    fstream infile;
	infile.open(file_name.data(),ios::in);
	if(!infile)
	{
		cout<<"Open file FAIL: "<<file_name.data()<<endl;
		return false;
	}
    cout<<"Open file : "<<file_name.data()<<endl;
	int cnt=0,cc;
	while(infile)
	{
		infile.getline(tmp,MAXLINELEN);

		cc=line_zjq_feature(tmp);
        if(cc==-1)
            break;
    	cnt+=cc;
	}
    cout<<"Read "<<cnt<<" Features\n";
	infile.close();
    int verticle_num[4][DOCPERPAGE+2]={0};
    for(auto &sess:sessions)
    {
        sess.type=0;
        for(int i=1;i<=DOCPERPAGE;++i)
            if(docs[sess.doc_id[i]].type)
                ++sess.type;
        //if(sess.click_cnt)
            ++verticle_num[sess.kind][sess.type];
    }
    cout<<"Verticle number cnt"<<endl;
    string kind_name[4]={"None","Train","Test","Val"};
    for(int k=0;k<=3;++k)
    {
        cout<<kind_name[k]<<" :\t";
        for(int i=0;i<=DOCPERPAGE;++i)
            cout<<verticle_num[k][i]<<"\t";
        cout<<endl;
    }
    for(int i=0;i<docs.size();++i)
        if(docs[i].type==-1)
            docs[i].type=MAXVERTICLE;
	return true;
}
#endif