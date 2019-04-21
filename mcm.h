#ifndef MCM_H
#define MCM_H
//#define double long double
class mcm:public model
{
    public:
        double forward[DOCPERPAGE+2][2],backward[DOCPERPAGE+2][2];
        double gamma1[DOCPERPAGE+2][DOCPERPAGE+2],gamma0[DOCPERPAGE+2][DOCPERPAGE+2],gamma[DOCPERPAGE+2][DOCPERPAGE+2];
        //alpha_cnt=train_cnt
        vector<double> alpha1,alpha0,beta1,beta0,s_c1,s_c0,s_e1,s_e0;
        vector<double> alpha,beta,s_c,s_e;
        void train_init()
        {
            name="Mcm";
            doc_rel=vector<double>(docs.size()+1);
            alpha=vector<double>(docs.size()+1);
            alpha1=vector<double>(docs.size()+1);
            alpha0=vector<double>(docs.size()+1);
            beta=vector<double>(MAXVERTICLE+1);
            beta1=vector<double>(MAXVERTICLE+1);
            beta0=vector<double>(MAXVERTICLE+1);
            s_e=vector<double>(docs.size()+1);
            s_e1=vector<double>(docs.size()+1);
            s_e0=vector<double>(docs.size()+1);
            s_c=vector<double>(docs.size()+1);
            s_c1=vector<double>(docs.size()+1);
            s_c0=vector<double>(docs.size()+1);
            for(int i=0;i<=DOCPERPAGE;++i)
                for(int j=0;j<=DOCPERPAGE;++j)
                    gamma[i][j]=0.5;
            for(int i=0;i<=docs.size();++i)
            {
                alpha[i]=s_c[i]=s_e[i]=0.5;
            }
            for(int i=0;i<=MAXVERTICLE;++i)
                beta[i]=0.5;
            //train_clear();
        }
        void train_clear()
        {
            for(int i=0;i<=docs.size();++i)
            {
                //alpha1[i]=alpha0[i]=s_c1[i]=s_c0[i]=s_e1[i]=s_e0[i]=1;
                alpha1[i]=1;
                alpha0[i]=.5;
                s_c1[i]=1;
                s_c0[i]=.2;
                s_e1[i]=.5;
                s_e0[i]=1;
            }
            for(int i=0;i<=MAXVERTICLE;++i)
                beta0[i]=beta1[i]=1;
            for(int i=0;i<=DOCPERPAGE;++i)
                for(int j=0;j<=DOCPERPAGE;++j)
                    gamma1[i][j]=gamma0[i][j]=.0001;
        }
        void train()
        {
            for(int i=0;i<docs.size();++i)
            {
                docs[i].train_tim=0;
            }
            int sess_cnt=0;
            for(auto &sess:sessions)
            {
                if(sess.enable==false)
                    continue;
                ++sess_cnt;
                if(!istrain(sess,sess_cnt))//在没有Overfit可能性的模型里应写作istest，在需要验证集的模型里应写作!istrain
                    continue;
                for(int i=DOCPERPAGE;i;--i)
                    ++docs[sess.doc_id[i]].train_tim;
            }
            double last_ll=-100,now_ll;
            int last_clk;
            train_init();
            for(int round=1;round<=MAXROUND;++round)
            {
                train_clear();
                double maxdlt=0;
                for(auto &sess:sessions)
                {
                    if(sess.enable==false)
                        continue;
                    ++sess_cnt;
                    if(!istrain(sess,sess_cnt))//在没有Overfit可能性的模型里应写作istest，在需要验证集的模型里应写作!istrain
                        continue;
                    get_prob(sess);
                    last_clk=0;
                    double prob_cs=forward[DOCPERPAGE][0]+forward[DOCPERPAGE][1];
                    for(int i=1;i<=DOCPERPAGE;++i)
                    {
                        int id=sess.doc_id[i];
                        int vid=docs[id].type;
                        if(sess.click_time[i]>.1)
                        {
                            alpha1[id]+=1;
                            //alpha0[id]+=;
                            beta1[vid]+=1;
                            //beta0[vid]+=1;
                            gamma1[i][i-last_clk]+=1;
                            //gamma0[i][i-last_clk]+=;
                            //s_c1[id]+=forward[i-1][0]*gamma[i][i-last_clk]*alpha[id]*beta[vid]*backward[i][1];要转成条件概率计算
                            s_c1[id]+=forward[i-1][0]*gamma[i][i-last_clk]*alpha[id]*beta[vid]*s_c[id]*backward[i][1]/prob_cs;
                            s_c0[id]+=forward[i-1][0]*gamma[i][i-last_clk]*alpha[id]*beta[vid]*(1.-s_c[id])*backward[i][0]/prob_cs;
                            //s_e1[id]+=forward[i-1][0]*gamma[i][i-last_clk]*alpha[id]*beta[vid]*s_e[id]*backward[i][0]/prob_cs;
                            //s_e0[id]+=forward[i-1][0]*gamma[i][i-last_clk]*alpha[id]*beta[vid]*(1.-s_e[id])*backward[i][0]/prob_cs;
                            last_clk=i;
                        }
                        else
                        {
                            alpha1[id]+=(forward[i-1][0]*alpha[id]*backward[i][0]*(1.-gamma[i][i-last_clk])\
                                +forward[i-1][1]*backward[i][1]*alpha[id]+forward[i-1][0]*gamma[i][i-last_clk]*alpha[id]*\
                                (1.-beta[vid])*(s_e[id]*backward[i][1]+(1.-s_e[id])*backward[i][0]))/prob_cs;
                            alpha0[id]+=(forward[i-1][0]*(1.-alpha[id])*backward[i][0]+\
                                forward[i-1][1]*backward[i][1]*(1.-alpha[id]))/prob_cs;
                            beta1[vid]+=beta[vid]*(forward[i-1][0]*backward[i][0]*(1.-alpha[id]*gamma[i][i-last_clk])+forward[i-1][1]*backward[i][1])/prob_cs;
                            beta0[vid]+=((1.-beta[vid])*(forward[i-1][0]*backward[i][0]*(1.-alpha[id]*gamma[i][i-last_clk])+forward[i-1][1]*backward[i][1])+\
                                forward[i-1][0]*gamma[i][i-last_clk]*alpha[id]*(1.-beta[vid])*(s_e[id]*backward[i][1]+(1.-s_e[id])*backward[i][0]))/prob_cs;
                            //beta0[vid]+=1.-beta[vid]*(forward[i-1][0]*backward[i][0]*(1.-alpha[id]*gamma[i][i-last_clk])+forward[i-1][1]*backward[i][1])/prob_cs;
                            gamma1[i][i-last_clk]+=(forward[i-1][0]*backward[i][0]*gamma[i][i-last_clk]*(1.-alpha[id]+alpha[id]*(1.-beta[vid])*(1.-s_e[id]))+\
                                forward[i-1][0]*backward[i][1]*gamma[i][i-last_clk]*alpha[id]*(1.-beta[vid])*s_e[id])/prob_cs;
                            gamma0[i][i-last_clk]+=(forward[i-1][0]*backward[i][0]*(1.-gamma[i][i-last_clk]))/prob_cs;
                            /*double w=0;
                            w+=(forward[i-1][0]*backward[i][0]*gamma[i][i-last_clk]*(1.-alpha[id]+alpha[id]*(1.-beta[vid])*(1.-s_e[id]))+\
                                forward[i-1][0]*backward[i][1]*gamma[i][i-last_clk]*alpha[id]*(1.-beta[vid])*s_e[id])/prob_cs;
                            w+=(forward[i-1][0]*backward[i][0]*(1.-gamma[i][i-last_clk]))/prob_cs;
                            w-=forward[i-1][0]*backward[i-1][0]/prob_cs;
                            if(w>1e-7||w<-1e-7)
                            {
                                cerr<<forward[i-1][0]<<"\t"<<backward[i][0]<<"\t"<<forward[i-1][1]<<"\t"<<backward[i][1]<<"\t"<<prob_cs<<"\t"<<forward[i-1][0]*backward[i-1][0]<<endl;
                                cerr<<gamma[i][i-last_clk]<<"\t"<<alpha[id]<<"\t"<<s_e[id]<<"\t"<<beta[vid]<<endl;
                                cerr<<(forward[i-1][0]*backward[i][0]*gamma[i][i-last_clk]*(1.-alpha[id]+alpha[id]*(1.-beta[vid])*(1.-s_e[id]))+\
                                forward[i-1][0]*backward[i][1]*gamma[i][i-last_clk]*alpha[id]*(1.-beta[vid])*s_e[id])/prob_cs<<endl;
                                cerr<<(forward[i-1][0]*backward[i][0]*(1.-gamma[i][i-last_clk]))/prob_cs<<endl;
                                assert(w<1e-4&&-1e-4<w);
                            }
                            */
                            //s_c1[id]+=;
                            //s_c0[id]+=;
                            s_e1[id]+=(forward[i-1][0]*gamma[i][i-last_clk]*alpha[id]*(1.-beta[vid])*s_e[id]*backward[i][1])/prob_cs;
                            s_e0[id]+=(forward[i-1][0]*gamma[i][i-last_clk]*alpha[id]*(1.-beta[vid])*(1.-s_e[id])*backward[i][0])/prob_cs;
                        }
                    }
                }
                for(int i=0;i<docs.size();++i)
                {
                    maxdlt=max(maxdlt,fabs(alpha[i]-alpha1[i]/(alpha1[i]+alpha0[i])));
                    maxdlt=max(maxdlt,fabs(s_c[i]-s_c1[i]/(s_c1[i]+s_c0[i])));
                    maxdlt=max(maxdlt,fabs(s_e[i]-s_e1[i]/(s_e1[i]+s_e0[i])));
                    alpha[i]=alpha1[i]/(alpha1[i]+alpha0[i]);
                    s_c[i]=s_c1[i]/(s_c1[i]+s_c0[i]);
                    s_e[i]=s_e1[i]/(s_e1[i]+s_e0[i]);
                    //cerr<<alpha[i]<<"\t"<<s_c[i]<<"\t"<<s_e[i]<<endl;
                    //assert(s_e[i]<1&&s_e[i]>0);
                }
                for(int i=0;i<=MAXVERTICLE;++i)
                {
                    maxdlt=max(maxdlt,fabs(beta[i]-beta1[i]/(beta0[i]+beta1[i])));
                    beta[i]=beta1[i]/(beta0[i]+beta1[i]);
                    //cerr<<i<<"\t"<<beta[i]<<"\t"<<beta0[i]<<"\t"<<beta1[i]<<endl;
                }
                for(int i=0;i<=DOCPERPAGE;++i)
                    for(int j=0;j<=i;++j)
                    {
                        maxdlt=max(maxdlt,fabs(gamma[i][j]-gamma1[i][j]/(gamma0[i][j]+gamma1[i][j])));
                        gamma[i][j]=gamma1[i][j]/(gamma0[i][j]+gamma1[i][j]);
                        //assert(gamma[i][j]<1&&gamma[i][j]>0);
                    }
                now_ll=this->test(false,3);
                //now_LL2=this->test(false,2);
                //now_LL1=this->test(false,1);
                if(round%50==0)
                    cerr<<"Round:\t"<<round<<"\tLL:\t"<<now_ll<<"\t"<<maxdlt<<endl;
                /*if(round%10==0)
                    cerr<<"Test:\t"<<test(false,2)<<endl;*/
                /*if(now_ll-1e-8<last_ll)
                    break;*/
                last_ll=now_ll;
            }

            FILE* outfile=fopen("../output/mcm2_args","w");
            assert(outfile);
            for(int i=0;i<=DOCPERPAGE;++i)
            for(int j=0;j<=DOCPERPAGE;++j)
            {
                fprintf(outfile,"%.8lf",gamma[i][j]);
                if(j==DOCPERPAGE)
                    fprintf(outfile,"\n");
                else
                    fprintf(outfile,"\t");
            }
            fprintf(outfile,"%u\n",MAXVERTICLE);
            for(int i=0;i<=MAXVERTICLE;++i)
            {
                fprintf(outfile,"%.8lf",beta[i]);
                if(i==MAXVERTICLE)
                    fprintf(outfile,"\n");
                else
                    fprintf(outfile,"\t");
            }
            fprintf(outfile,"%u\n",docs.size());
            for(int i=0;i<docs.size();++i)
                if(docs[i].name!="")
                {
                    fprintf(outfile,"%s",docs[i].name.data());
                    fprintf(outfile,"\t%.8lf\t%.8lf\t%.8lf\n",alpha[i],s_c[i],s_e[i]);
                }
            fclose(outfile);
        }
        void load()
        {
            FILE* infile=fopen("../output/mcm2_args","r");
            assert(infile);
            for(int i=0;i<=DOCPERPAGE;++i)
                for(int j=0;j<=DOCPERPAGE;++j)
                {
                    fscanf(infile,"%lf",gamma[i]+j);
                }
            char namee[100];
            double r1,r2,r3;
            string name;
            unsigned int cnt;
            fscanf(infile,"%u",&cnt);
            MAXVERTICLE=cnt;
            beta=vector<double>(cnt+1);
            for(int i=0;i<=MAXVERTICLE;++i)
                fscanf(infile,"%lf",&beta[i]);
            //cerr<<cnt<<endl;
            fscanf(infile,"%u",&cnt);
            alpha=vector<double>(doc_name2id.size()+cnt+2);
            s_c=vector<double>(doc_name2id.size()+cnt+2);
            s_e=vector<double>(doc_name2id.size()+cnt+2);
            while(fscanf(infile,"%s%lf%lf%lf",namee,&r1,&r2,&r3)==4)
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
                alpha[w]=r1;
                s_c[w]=r2;
                s_e[w]=r3;
            }
            fclose(infile);
        }
        void get_prob(Session &sess)
        {
            static int last_calc_sess=-1;
            if(sess.id==last_calc_sess)
                return;
            last_calc_sess=sess.id;
            memset(forward,0,sizeof(forward));//alpha
            memset(backward,0,sizeof(backward));//beta
            forward[0][0]=1.0;
            forward[0][1]=0.0;
            int last_clk[DOCPERPAGE+2];
            last_clk[0]=0;//last_clk[i]<=i
            //    cerr<<"BEGIN\t"<<sess.id<<endl;
            for(int i=0;i<DOCPERPAGE;++i)
            {
                int id=sess.doc_id[i+1];
                int vid=docs[id].type;
                if(sess.click_time[i+1]>.1)
                {
                    forward[i+1][0]=forward[i][0]*gamma[i+1][i+1-last_clk[i]]*alpha[id]*beta[vid]*(1.-s_c[id]);
                    forward[i+1][1]=forward[i][0]*gamma[i+1][i+1-last_clk[i]]*alpha[id]*beta[vid]*s_c[id];
                    //cerr<<"CLK!\t"<<sess.doc_id[i+1]<<"\t"<<vid<<"\t"<<forward[i+1][0]<<"\t"<<forward[i+1][1]<<"\t"<<gamma[i+1][i+1-last_clk[i]]<<"\t"<<alpha[id]<<"\t"<<beta[vid]<<endl;
                    last_clk[i+1]=i+1;
                }
                else
                {
                    forward[i+1][0]=forward[i][0]*(1.+gamma[i+1][i+1-last_clk[i]]*alpha[id]*((1.-beta[vid])*(1.-s_e[id])-1.));
                    forward[i+1][1]=forward[i][0]*gamma[i+1][i+1-last_clk[i]]*alpha[id]*(1.-beta[vid])*s_e[id]+forward[i][1];
                    //cerr<<"\t\t"<<forward[i+1][0]<<"\t"<<forward[i+1][1]<<"\t"<<gamma[i+1][i+1-last_clk[i]]<<"\t"<<alpha[id]<<"\t"<<beta[vid]<<endl;
                    last_clk[i+1]=last_clk[i];
                }
            }
            backward[DOCPERPAGE][0]=1.0;
            backward[DOCPERPAGE][1]=1.0;
            for(int i=DOCPERPAGE;i;--i)
            {
                int id=sess.doc_id[i];
                int vid=docs[id].type;
                if(sess.click_time[i]>.1)
                {
                    backward[i-1][0]=(backward[i][0]*(1.-s_c[id])+backward[i][1]*s_c[id])*gamma[i][i-last_clk[i-1]]*alpha[id]*beta[vid];
                }
                else
                {
                    backward[i-1][0]=backward[i][0]*(1.+gamma[i][i-last_clk[i-1]]*alpha[id]*((1.-beta[vid])*(1.-s_e[id])-1.))\
                        +backward[i][1]*gamma[i][i-last_clk[i-1]]*alpha[id]*(1.-beta[vid])*s_e[id];
                    backward[i-1][1]=backward[i][1];
                }
            }
            /*cerr<<"BEG"<<endl;
            for(int i=0;i<=DOCPERPAGE;++i)
                cerr<<forward[i][0]*backward[i][0]+forward[i][1]*backward[i][1]<<endl;
            for(int i=0;i<=DOCPERPAGE;++i)
                cerr<<forward[i][0]<<"\t"<<forward[i][1]<<"\t"<<backward[i][0]<<"\t"<<backward[i][1]<<"\t"<<alpha[sess.doc_id[i]]<<"\t"<<beta[docs[sess.doc_id[i]].type]<<"\t"\
                    <<gamma[i][i-last_clk[i-1]]<<"\t"<<s_c[sess.doc_id[i]]<<"\t"<<s_e[sess.doc_id[i]]<<endl;
            cerr<<forward[DOCPERPAGE][0]<<"\t"<<forward[DOCPERPAGE][1]<<endl;*/
            //assert(forward[DOCPERPAGE][0]+forward[DOCPERPAGE][1]>0&&forward[DOCPERPAGE][0]+forward[DOCPERPAGE][1]<1);
        }
        void get_click_prob(Session &sess,double* click_prob)
        {
            //click_prob[i]=P(C_i|C_1,C_2,...C_{i-1})
            //examination = P(E_k|C_1,C_2,...C_{k-1})
            click_prob[0]=1.0;
            double s_prob=0.0;
            int last_clk=0;
            get_prob(sess);
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                int id=sess.doc_id[i];
                int vid=docs[id].type;
                if(sess.click_time[i]>.1)
                {
                    click_prob[i]=forward[i-1][0]/(forward[i-1][0]+forward[i-1][1])*gamma[i][i-last_clk]*alpha[id]*beta[vid];
                    last_clk=i;
                }
                else
                {
                    click_prob[i]=1.-forward[i-1][0]/(forward[i-1][0]+forward[i-1][1])*gamma[i][i-last_clk]*alpha[id]*beta[vid];
                }
            }
            /*for(int i=1;i<=DOCPERPAGE;++i)
            {
                int id=sess.doc_id[i];
                int vid=docs[id].type;
                if(sess.click_time[i]>.1)
                {
                    click_prob[i]=(1.-s_prob)*gamma[i][i-last_clk]*alpha[id]*beta[vid];
                    s_prob=s_c[id];
                    last_clk=i;
                }
                else
                {
                    click_prob[i]=1.-(1.-s_prob)*gamma[i][i-last_clk]*alpha[id]*beta[vid];
                    s_prob+=(1.-s_prob)*s_e[id]*gamma[i][i-last_clk]*alpha[id]*(1.-beta[vid])\
                        /(1.-alpha[id]*beta[vid]*gamma[i][i-last_clk]);
                }
            }*/
        }
};
#endif
