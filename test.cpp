#include<cmath>
#include<vector>
#include<map>
#include<unordered_map>
#include<cstdio>
#include<fstream>
#include<cstring>
#include<string>
#include<iostream>
#include<cassert>
#include<iomanip>
#include<omp.h>
#include<memory>
#include<atomic>
using namespace std;
int main()
{

    #pragma omp parallel for
	for(int i=1;i<=10;++i)
		cout<<i<<endl;
	return 0;
}
