#ifndef CONFIG_H
#define CONFIG_H
#define MAXFILECNT 2
#define MAXLINELEN 10010
#define MINDOCPERPAGE 10
//Filter when load
#define MAXDOCPERPAGE 13
//Filter when load
#define DOCPERPAGE MINDOCPERPAGE
#define MINCLICK 1
#define MAXCLICK 8
//Filter when load(by enable flag)
#define MINSESSION 0
//Filter when data filter (by enable flag)
#define MAXSESSION 10000000
//Filter when data filter (by enable flag)
#define SAVEBLOCK 0
//Querys per file (when dump)
#define TRAIN_DATA 0.7
#define TEST_DATA (1.0-TRAIN_DATA)
#define VALI 4
//VALI = 1/4 TEST ERROR!!!!
#define SAMPLE_NUM 10000000
#include<string>
extern bool IFFILTER=false,IFDIV=false;
extern std::string data_dir="../data/part-r-";
extern std::string save_file="../prework/part-r-";
extern int MAXVERTICLE=1;
extern int MAXROUND=100;
//Use the save_file option,we save all data in orignal format EXCEPT been filted. 
//If the save_dir equals to data_dir ,source data MAY BE RECOVERED.
#endif
