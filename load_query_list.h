#ifndef LOAD_QUERY_LIST
#define LOAD_QUERY_LIST
int line_query_list(const string &line)
{
    if(line.size()<1)
        return -1;
    vector<string> res=split(line,' ');
    if(res.size()!=2)
	res=split(line,'\t');
    assert(res.size()==2);
    query_list[res[1]]=true;
    return 1;
}
bool load_query_list(string file_name)
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

		cc=line_query_list(tmp);
        if(cc==-1)
            break;
    	cnt+=cc;
	}
	return true;
}
#endif
