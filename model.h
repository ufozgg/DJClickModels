#ifndef MODEL_H
#define MODEL_H
#define double long double
//#include<alghorithms>
bool istest(Session &sess,int cnt)
{
    if(sess.kind)
        return sess.kind==2;
	return int(cnt*TRAIN_DATA)==int((cnt-1)*TRAIN_DATA);
}
bool istrain(Session &sess,int cnt)
{
    if(sess.kind)
        return sess.kind==1;
	return int(cnt*TRAIN_DATA)!=int((cnt-1)*TRAIN_DATA);
}
bool isvali(Session &sess,int cnt)
{
    return sess.kind==3;
}
class model
{
	public:
		string name;
        vector<double> doc_rel;
        vector<int> shuffle;
        virtual void get_click_prob(Session &sess,double* click_prob)=0;
        //virtual void get_one_sample(Session &sess,bool* clk)=0;
        void get_one_sample(Session &sess,bool* clk)
        {
            Session w=Session(sess);
            double pr[DOCPERPAGE+2];
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                get_click_prob(w,pr);
                if(w.click_time[i]<.1)
                    pr[i]=1.-pr[i];
                if(rand()%10000<pr[i]*10000)
                    w.click_time[i]=i;
                else
                    w.click_time[i]=0;
            }
            for(int i=1;i<=DOCPERPAGE;++i)
                if(w.click_time[i]>.1)
                    clk[i]=1;
                else
                    clk[i]=0;
        }
		void train()
		{

		}
        void sample(int kind=1)
        {
            shuffle=vector<int>(sessions.size());
            for(int i=0;i<sessions.size();++i)
                shuffle[i]=i;
            std::random_shuffle(shuffle.begin(),shuffle.end());
            int cnt=0;
            bool clk[DOCPERPAGE+2];
            double ctr=0,ctr_pos[DOCPERPAGE+2];
            int first_click[DOCPERPAGE+2],last_click[DOCPERPAGE+2],hamming[DOCPERPAGE+2],click_num[DOCPERPAGE+2];
            //vector<double> cita;
            for(int i=0;i<=DOCPERPAGE;++i)
                ctr_pos[i]=first_click[i]=last_click[i]=hamming[i]=click_num[i]=0;
            FILE* file=fopen(("../output/_"+name+"sample").data(),"w");
            for(int i=0;i<sessions.size()&&cnt<SAMPLE_NUM;++i)
            {
                Session &sess=sessions[i];
                if(sess.kind!=kind)
                    continue;
                ++cnt;
                for(int j=1;j<=DOCPERPAGE;++j)
                    if(sess.click_time[j]>.1)
                        clk[j]=1;
                    else
                        clk[j]=0;//ZRZ ERROR
                //get_one_sample(sess,clk);
                int hamm=0,cc=0;
                int fir_clk=0,las_clk=0;
                for(int j=1;j<=DOCPERPAGE;++j)
                {
                    if(clk[j])
                    {
                        if(fir_clk==0)
                            fir_clk=j;
                        las_clk=j;
                        ctr+=1.0;
                        ctr_pos[j]+=1.0;
                        ++cc;
                    }
                    if((clk[j])!=(sess.click_time[j]>.1))
                        ++hamm;
                }
                hamming[hamm]+=1;
                click_num[cc]+=1;
                first_click[fir_clk]+=1;
                last_click[las_clk]+=1;
            }
            
            fprintf(file,"%d\n%s\n",cnt,to_string(ctr/cnt/DOCPERPAGE).data());
            for(int i=0;i<=DOCPERPAGE;++i)
                fprintf(file,"%s,",to_string(ctr_pos[i]/cnt).data());
            fprintf(file,"\n");
            for(int i=0;i<=DOCPERPAGE;++i)
                fprintf(file,"%s,",to_string(first_click[i]*1./cnt).data());
            fprintf(file,"\n");
            for(int i=0;i<=DOCPERPAGE;++i)
                fprintf(file,"%s,",to_string(last_click[i]*1./cnt).data());
            fprintf(file,"\n");
            for(int i=0;i<=DOCPERPAGE;++i)
                fprintf(file,"%s,",to_string(click_num[i]*1./cnt).data());
            fprintf(file,"\n");
            for(int i=0;i<=DOCPERPAGE;++i)
                fprintf(file,"%d,",hamming[i]);
            fprintf(file,"\n");
            fclose(file);
        }
        void dump_rel()
        {
            string file_name="../output/_";
            file_name+=name+".relevence";
            FILE* file=fopen(file_name.data(),"w");
            for(int i=0;i<docs.size();++i)
                if(docs[i].train_tim)
                {
                    fprintf(file,"%s\t%s\n",docs[i].name.data(),to_string(doc_rel[i]).data());
                }
            fclose(file);
        }
        void check_model()
        {
            Session test_case;
            double pr[2][DOCPERPAGE+2];
            for(int i=0;i<sessions.size();++i)
            {
                test_case=sessions[i];
                get_click_prob(test_case,pr[0]);
                for(int round=1;round<=100;++round)
                {
                    int k=rand()%DOCPERPAGE+1;
                    if(test_case.click_time[k]>.1)
                        test_case.click_time[k]=0;
                    else
                        test_case.click_time[k]=1;
                    get_click_prob(test_case,pr[round&1]);
                    if(pr[0][k]+pr[1][k]+1e-6<1.||pr[0][k]+pr[1][k]-1e-6>1.)
                    {
                        cerr<<"ERROR"<<endl;
                    }
                    assert(pr[0][k]+pr[1][k]+1e-6>1.&&pr[0][k]+pr[1][k]-1e-6<1.);
                }
            }
            cerr<<"CHECK MODEL FINISHED"<<endl;
        }
		double test(bool out=true,int kind=2,int type=-1)
		{
            if(out)
                cout<<"Test Module "<<name<<endl;
            int i;
            double position_perplexity[DOCPERPAGE+1];
            double position_skip_perplexity[DOCPERPAGE+1];
            double position_click_perplexity[DOCPERPAGE+1];
            int position_click[DOCPERPAGE+1];
            int position_skip[DOCPERPAGE+1];
            
            int sess_cnt=0,test_cnt=0;
            double log_likelihood=0.0,logp;
            double total_perplexity=0.0;
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                position_perplexity[i]=0.0;
                position_click_perplexity[i]=0.0;
                position_skip_perplexity[i]=0.0;
                position_click[i]=0;
                position_skip[i]=0;
            }
            double click_prob[DOCPERPAGE+2];
                double minn=1;
			for(auto &sess:sessions)
            {
                if(sess.enable==false)
                    continue;
                ++sess_cnt;
                if(sess.kind!=kind||(type!=-1&&type!=sess.type))
                    continue;
                /*bool ok=true;
                for(int i=1;i<=10;++i)
                    if(docs[sess.doc_id[i]].train_tim==0)
                    {
                        ok=false;
                        break;
                    }
                if(!ok)
                    continue;*/
                ++test_cnt;
                double click_prob[DOCPERPAGE+1];
                get_click_prob(sess,click_prob);
                for(int i=1;i<=DOCPERPAGE;++i)
                {
                    logp=log(click_prob[i]);
                    log_likelihood+=logp;
                    if(logp>log(0.99999999999))
                        logp=log(0.99999999999);
                    if(logp<log(0.00000000001))
                        logp=log(0.00000000001);// MAGIC OPTION
                    //log(P(C_1,C_2...C_i)/P(C_1,C_2...C_i-1))
                    //position_perplexity[i]-=log(click_prob[i]/click_prob[i-1]);
                    position_perplexity[i]-=logp;
                }
            }
            //exit(0);
            for(i=1;i<=DOCPERPAGE;++i)
                position_perplexity[i]=pow(2.,position_perplexity[i]/log(2)/test_cnt);
            for(i=1;i<=DOCPERPAGE;++i)
                total_perplexity+=position_perplexity[i];
            total_perplexity/=DOCPERPAGE;
            if(out)
            {
                cout<<"Test_cnt:\t"<<test_cnt<<endl;
                cout<<"Log likelihood(Log e):\t"<<fixed<<setprecision(8)<<log_likelihood/test_cnt/DOCPERPAGE<<endl;
                cout<<"Position Perplexity(Base 2):\n";
                for(i=1;i<=DOCPERPAGE;++i)
                    cout<<position_perplexity[i]<<"\t";
                cout<<endl<<"Perplexity:\t"<<total_perplexity<<endl<<endl<<endl;
            }
            return total_perplexity;
            //return log_likelihood/test_cnt/DOCPERPAGE;
		}
};
#endif