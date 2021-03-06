#ifndef DBN_H
#define DBN_H
//#define double long double
class dbn:public model
{
    public:
        vector<double> a,s;
        vector<double> Qa,Qs;
        vector<int> clk_tim;
        double gamma=.9;//.925;
        dbn()
        {
            name="Dbn";
        }
        double get_rel(int id)
        {
            return a[id]*s[id];
        }
        void train()
        {
            
            doc_rel=vector<double>(docs.size()+1);
            a=vector<double>(docs.size()+1);
            s=vector<double>(docs.size()+1);
            Qa=vector<double>(docs.size()+1);
            Qs=vector<double>(docs.size()+1);
            clk_tim=vector<int>(docs.size()+1);
            int sess_cnt=0;
            for(int i=0;i<docs.size();++i)
                docs[i].train_tim=0,clk_tim[i]=0;
            for(auto &sess:sessions)
            {
                if(sess.enable==false)
                    continue;
                ++sess_cnt;
                if(!istrain(sess,sess_cnt))//在没有Overfit可能性的模型里应写作istest，在需要验证集的模型里应写作!istrain
                    continue;
                for(int i=DOCPERPAGE;i;--i)
                {
                    ++docs[sess.doc_id[i]].train_tim;
                    if(sess.click_time[i]>.1)
                        ++clk_tim[sess.doc_id[i]];
                }
            }
            for(int i=1;i<docs.size();++i)
            {
                a[i]=s[i]=0.5;
            }
            int rnd;
            double pr0[DOCPERPAGE+2];
            double last_LL=-100,now_LL,kk=10,kk2;
            int p0=1;
            for(rnd=1;rnd<=MAXROUND;++rnd)
            {
                for(int i=1;i<docs.size();++i)
                    Qa[i]=Qs[i]=0;
                for(auto &sess:sessions)
                {
                    if(sess.enable==false)
                        continue;
                    ++sess_cnt;
                    if(!istrain(sess,sess_cnt))//在没有Overfit可能性的模型里应写作istest，在需要验证集的模型里应写作!istrain
                        continue;
                    get_examine_prob(sess,pr0);
                    for(int i=1;i<=DOCPERPAGE;++i)
                    {
                        if(sess.click_time[i]<.1)
                        {
                            Qa[sess.doc_id[i]]+=a[sess.doc_id[i]]*(1.-pr0[i]);
                        }
                        else
                        {
                            Qa[sess.doc_id[i]]+=1.;
                            Qs[sess.doc_id[i]]+=(1.-pr0[i+1])*s[sess.doc_id[i]]/(1.-gamma+gamma*s[sess.doc_id[i]]);
                            /*if(!(-0.000001<=Qs[sess.doc_id[i]]))
                            {
                                cout<<i<<"XXX"<<pr0[i+1]<<"\t"<<s[sess.doc_id[i]]<<"X"<<endl;
                                exit(0);
                            }*/
                        }
                    }
                }

                for(int i=1;i<docs.size();++i)
                {
                    a[i]=(Qa[i]+1)/(docs[i].train_tim+2);
                    s[i]=(Qs[i]+1)/(clk_tim[i]+2);
                    /*if(!(0.000001<=s[i]&&s[i]<=0.999999))
                    {
                        cout<<i<<"XX"<<Qs[i]<<"\t"<<clk_tim[i]<<endl;
                        exit(0);
                    }*/
                    //(tim-x)/(a-1)+x/a=0
                    //a*tim=x
                    //a=x/tim
                }
                /*if(rnd%100==1)
                {
                    kk2=test(rnd%1000==1,2);
                    cout<<rnd<<"\t"<<fixed<<setprecision(12)<<kk-kk2<<"\t"<<kk2<<endl;
                    kk=kk2;
                }*/
                //cout<<rnd<<"\tVal LL:=\t"<<now_LL<<endl;
                if(rnd%100==0)
                {
                    now_LL=test(false,3);
                    cerr<<rnd<<"\t"<<this->test(false,3)<<endl;
                    if(now_LL-1e-8<last_LL)
                        break;
                }
                //cout<<rnd<<"\tTrain LL:=\t"<<this->test(false,1)<<"\tVal LL:=\t"<<now_LL<<"\tTest LL:=\t"<<this->test(false,2)<<endl;
                last_LL=now_LL;
            }
            for(int i=0;i<docs.size();++i)
                if(docs[i].train_tim)
                {
                    doc_rel[i]=a[i]*s[i];
                }
            FILE* outfile=fopen("../output/dbn_args","w");
            assert(outfile);
            fprintf(outfile,"%.8lf\n",gamma);
            fprintf(outfile,"%u\n",docs.size());
            for(int i=0;i<docs.size();++i)
                if(docs[i].name!="")
                {
                    fprintf(outfile,"%s",docs[i].name.data());
                    fprintf(outfile,"\t%.8lf\t%.8lf\n",a[i],s[i]);
                }
            fclose(outfile);
        }
        void load()
        {
            FILE* infile=fopen("../output/dbn_args","r");
            fscanf(infile,"%lf",&gamma);
            char namee[100];
            double aa,ss;
            string name;
            unsigned int cnt;
            fscanf(infile,"%u",&cnt);
            a=vector<double>(doc_name2id.size()+cnt+2);
            s=vector<double>(doc_name2id.size()+cnt+2);
            while(fscanf(infile,"%s%lf%lf",namee,&aa,&ss)==3)
            {
                name=namee;
                int w=doc_name2id[name];
                if(w==0)
                {
                    w=docs.size();
                    Doc d;
                    d.name=name;
                    docs.push_back(d);
                    doc_name2id[name]=w;
                }
                a[w]=aa;
                s[w]=ss;
            }
            fclose(infile);
        }
        void get_examine_prob(Session &sess,double *pr)
        {
            static double forward[DOCPERPAGE+2][2],backward[DOCPERPAGE+2][2];
            memset(forward,0,sizeof(forward));//alpha
            memset(backward,0,sizeof(backward));//beta
            forward[1][1]=1.;
            forward[1][0]=0.;
            backward[DOCPERPAGE+1][1]=1;
            backward[DOCPERPAGE+1][0]=1;
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                if(sess.click_time[i]>.1)
                {
                    forward[i+1][0]=forward[i][1]*a[sess.doc_id[i]]*(1.-gamma+gamma*s[sess.doc_id[i]]);
                    forward[i+1][1]=forward[i][1]*a[sess.doc_id[i]]*(1.-s[sess.doc_id[i]])*gamma;
                }
                else
                {
                    forward[i+1][0]=forward[i][0]+forward[i][1]*(1.-a[sess.doc_id[i]])*(1.-gamma);
                    forward[i+1][1]=forward[i][1]*(1.-a[sess.doc_id[i]])*gamma;
                }
            }
            for(int i=DOCPERPAGE+1;i>1;--i)
            {
                if(sess.click_time[i-1]>.1)
                {
                    //backward[i-1][0]=0;
                    backward[i-1][1]=backward[i][0]*a[sess.doc_id[i-1]]*(1.-gamma+gamma*s[sess.doc_id[i-1]])\
                        +backward[i][1]*a[sess.doc_id[i-1]]*(1.-s[sess.doc_id[i-1]])*gamma;
                }
                else
                {
                    backward[i-1][1]=backward[i][1]*(1.-a[sess.doc_id[i-1]])*gamma+\
                        backward[i][0]*(1.-a[sess.doc_id[i-1]])*(1.-gamma);
                    backward[i-1][0]=backward[i][0];
                }
            }
            /*cout<<sess.id<<endl<<sess.doc_id[1]<<endl;
            for(int i=1;i<=DOCPERPAGE+1;++i)
            {
                cout<<i<<"\t"<<forward[i][0]<<"\t"<<forward[i][1]<<"\t"<<backward[i][0]<<"\t"<<backward[i][1]<<"\t"<<a[sess.doc_id[i]]<<"\t"<<s[sess.doc_id[i]]<<endl;
            }*/
            //cout<<forward[DOCPERPAGE+1][0]+forward[DOCPERPAGE+1][1]<<"\t"<<backward[1][1]<<endl;
            //assert(forward[DOCPERPAGE+1][0]+forward[DOCPERPAGE+1][1]<0.00001+backward[1][1]);
            //assert(forward[DOCPERPAGE+1][0]+forward[DOCPERPAGE+1][1]>-0.00001+backward[1][1]);
            for(int i=1;i<=DOCPERPAGE+1;++i)
            {
                pr[i]=forward[i][1]*backward[i][1]/(forward[i][0]*backward[i][0]+forward[i][1]*backward[i][1]);
                //if(!(0.000001<=pr[i]||pr[i]<=0.999999))
                //    cout<<i<<"Q"<<forward[i][0]<<"X\tD"<<forward[i][1]<<"\n"<<backward[i][0]<<"\t"<<backward[i][1]<<endl;
                //assert(pr[i]<=1.);
                //assert(pr[i]>=0.);
                if(pr[i]<0.0000000001)
                    pr[i]=0.0000000001;
                if(pr[i]>0.9999999999)
                    pr[i]=0.9999999999;
            }
        }
        void get_click_prob(Session &sess,double* click_prob)
        {
            //click_prob[i]=P(C_1,C_2...C_i) X
            //click_prob[i]=P(C_i|C_1,C_2,...C_{i-1})
            //examination = P(E_k|C_1,C_2,...C_{k-1})
            click_prob[0]=1.0;
            double examination=1.0;
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                if(sess.click_time[i]>.1)
                {
                    click_prob[i]=examination*a[sess.doc_id[i]];
                    /*if(!(click_prob[i]<0.999999&&click_prob[i]>0.000001))
                    {
                        cout<<click_prob[i]<<"\tA\t"<<endl;
                        cout<<examination<<endl;
                        for(int j=1;j<=i;++j)
                            cout<<click_prob[j]<<"\t"<<a[sess.doc_id[j]]<<"\t"<<s[sess.doc_id[j]]<<"\t"<<sess.click_time[j]<<endl;
                        cout<<a[sess.doc_id[i]]<<"\t"<<sess.doc_id[i]<<endl;
                        exit(0);
                    }*/
                    examination=(1.-s[sess.doc_id[i]])*gamma;
                }
                else
                {
                    click_prob[i]=1.-examination*a[sess.doc_id[i]];
                    /*if(!(click_prob[i]<0.999999&&click_prob[i]>0.000001))
                    {
                        cout<<click_prob[i]<<"\tB\t"<<endl;
                        exit(0);
                    }*/
                    examination*=(1.-a[sess.doc_id[i]])/click_prob[i]*gamma;
                }
                /*click_prob[i]=(sess.click_time[i]<.1)+(1-2*(sess.click_time[i]<.1))*examination*a[sess.doc_id[i]];
                examination=gamma*(1.-(sess.click_time[i]>.1)*s[sess.doc_id[i]])*\
                    (1.-(sess.click_time[i]<.1)*a[sess.doc_id[i]])/(1.-(sess.click_time[i]<.1)*examination*a[sess.doc_id[i]])*((sess.click_time[i]>.1)*1.+(sess.click_time[i]<.1)*examination);*/
            }
        }
};
#endif
