#include "common.h"
vector<string> split(string w,char cut)
{
	string tmpp="";
	vector<string> ret;
	for(auto &i:w)
		if(i==cut)
		{
			if(tmpp!="")
			{
				ret.push_back(tmpp);
				tmpp="";
			}
		}
		else
			tmpp+=i;
	if(tmpp!="")
		ret.push_back(tmpp);
	return ret;
}