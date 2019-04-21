#ifndef UBM_H
#define UBM_H
//#define double long double
class ubm:public model
{
    public:
        double gamma[DOCPERPAGE+2][DOCPERPAGE+2];
        vector<double> doc_rel0,doc_rel1;
        void train()
        {
            name="Ubm";
            doc_rel=vector<double>(docs.size()+1);
            doc_rel0=vector<double>(docs.size()+1);
            doc_rel1=vector<double>(docs.size()+1);
            for(int i=0;i<docs.size();++i)
            {
                doc_rel[i]=0.5;
                /*docs[i].train_tim=1;//FOR SMOOTH
                */
            }
            for(int i=1;i<=DOCPERPAGE;++i)
                for(int j=1;j<=i;++j)
                    gamma[i][j]=0.5;
            int sess_cnt=0;
            double last_LL=-10,sum;
            for(auto &sess:sessions)
            {
                if(sess.enable==false)
                    continue;
                ++sess_cnt;
                if(!istrain(sess,sess_cnt))
                    continue;
                for(int i=DOCPERPAGE;i;--i)
                    ++docs[sess.doc_id[i]].train_tim;
            }
            int sess_id,pos_id,new_sess,new_pos;
            double gamma0[DOCPERPAGE+2][DOCPERPAGE+2],gamma1[DOCPERPAGE+2][DOCPERPAGE+2];
            int rnd;
            double p0,cpb[20];
            for(rnd=1;rnd<=MAXROUND;++rnd)
            {
                double maxdlt=-0.1;
                for(int i=0;i<docs.size();++i)
                {
                    doc_rel0[i]=0.5;//FOR SMOOTH
                    doc_rel1[i]=1;
                }
                for(int i=1;i<=DOCPERPAGE;++i)
                    for(int j=1;j<=i;++j)
                    {
                        gamma0[i][j]=0.5;
                        gamma1[i][j]=1;
                    }
                int last_clkk;
                for(auto &sess:sessions)
                {
                    if(sess.enable==false)
                        continue;
                    if(!istrain(sess,sess_cnt))
                        continue;
                    /*get_click_prob(sess,cpb);
                    p0=1.;
                    for(int i=1;i<=DOCPERPAGE;++i)
                        p0*=cpb[i];*/
                    last_clkk=0;
                    for(int i=1;i<=DOCPERPAGE;++i)
                    {
                        gamma1[i][i-last_clkk]+=1.;
                        if(sess.click_time[i]>.1)
                        {
                            gamma0[i][i-last_clkk]+=1.;
                            doc_rel0[sess.doc_id[i]]+=1.;
                            doc_rel1[sess.doc_id[i]]+=1.;
                            last_clkk=i;
                        }
                        else
                        {
                            gamma0[i][i-last_clkk]+=(1.-doc_rel[sess.doc_id[i]])*gamma[i][i-last_clkk]/(1.-doc_rel[sess.doc_id[i]]*gamma[i][i-last_clkk]);
                            //doc_rel0[sess.doc_id[i]]+=(1.-gamma[i][i-last_clkk])*doc_rel[sess.doc_id[i]]/(1.-doc_rel[sess.doc_id[i]]*gamma[i][i-last_clkk]);
                            //doc_rel1[sess.doc_id[i]]+=1.;
                            doc_rel1[sess.doc_id[i]]+=gamma[i][i-last_clkk]*(1.-doc_rel[sess.doc_id[i]])/(1.-doc_rel[sess.doc_id[i]]*gamma[i][i-last_clkk]);
                        }
                    }
                }
                /*for(auto &sess:sessions)
                {
                    if(sess.enable==false)
                        continue;
                    if(!istrain(sess,sess_cnt))
                        continue;
                    last_clkk=0;
                    for(int i=1;i<=DOCPERPAGE;++i)
                    {
                        ++gamma1[i][i-last_clkk];
                        if(sess.click_time[i]>.1)
                        {
                            gamma0[i][i-last_clkk]+=1.;
                            last_clkk=i;
                        }
                        else
                        {
                            gamma0[i][i-last_clkk]+=(1.-doc_rel[sess.doc_id[i]])*gamma[i][i-last_clkk]/(1.-doc_rel[sess.doc_id[i]]*gamma[i][i-last_clkk]);
                        }
                    }
                }*/
                for(int i=0;i<docs.size();++i)
                {
                    maxdlt=max(maxdlt,fabs(doc_rel[i]-doc_rel0[i]/doc_rel1[i]));
                    doc_rel[i]=doc_rel0[i]/doc_rel1[i];
                }
                for(int i=1;i<=DOCPERPAGE;++i)
                {
                    for(int j=1;j<=i;++j)
                    {
                        maxdlt=max(maxdlt,fabs(gamma[i][j]-gamma0[i][j]/gamma1[i][j]));
                        gamma[i][j]=gamma0[i][j]/gamma1[i][j];
                    }
                }
                double now_LL,now_LL2,now_LL1;
                now_LL=this->test(false,3);//ERROR
                cerr<<"Round = "<<rnd<<"\tVali = "<<now_LL<<"\t"<<maxdlt<<endl;
                //now_LL2=this->test(false,2);
                //now_LL1=this->test(false,1);
                /*if(now_LL-1e-8<last_LL)
                    break;*/
                //if(rnd%100==0)
                //    cout<<"UBM LL:=\t"<<rnd<<"\t"<<fixed<<setprecision(12)<<now_LL<<endl;
                //cout<<now_LL1<<","<<now_LL2<<","<<now_LL<<endl;
                last_LL=now_LL;
            }
            #ifdef DEBUG
                cout<<"ROUND:=\t"<<rnd<<endl;
                for(int i=1;i<=DOCPERPAGE;++i)
                {
                    for(int j=1;j<=i;++j)
                        cout<<gamma[i][j]<<"\t";
                    puts("");
                }
            #endif
            FILE* outfile=fopen("../output/ubm_args","w");
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
            fprintf(outfile,"%u\n",docs.size());
            for(int i=0;i<docs.size();++i)
                if(docs[i].name!="")
                {
                    fprintf(outfile,"%s",docs[i].name.data());
                    fprintf(outfile,"\t%.8lf\n",doc_rel[i]);
                }
            fclose(outfile);
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                for(int j=1;j<=i;++j)
                    cerr<<gamma[i][j]<<"\t";
                cerr<<endl;
            }
        }
        void load()
        {
            FILE* infile=fopen("../output/ubm_args","r");
            assert(infile);
            for(int i=0;i<=DOCPERPAGE;++i)
                for(int j=0;j<=DOCPERPAGE;++j)
                {
                    fscanf(infile,"%lf",gamma[i]+j);
                }
            char namee[100];
            double rel;
            string name;
            unsigned int cnt;
            fscanf(infile,"%u",&cnt);
            doc_rel=vector<double>(doc_name2id.size()+cnt+2);
            while(fscanf(infile,"%s%lf",namee,&rel)==2)
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
                doc_rel[w]=rel;
            }
            fclose(infile);
        }
        void get_click_prob(Session &sess,double* click_prob)
        {
            //click_prob[i]=P(C_1,C_2...C_i) X
            //click_prob[i]=P(C_i|C_1,C_2,...C_{i-1})
            //examination = P(E_k|C_1,C_2,...C_{k-1})
            click_prob[0]=1.0;
            int last_clkk=0;
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                if(sess.click_time[i]>.1)
                {
                    click_prob[i]=doc_rel[sess.doc_id[i]]*gamma[i][i-last_clkk];
                    last_clkk=i;
                }
                else
                {
                    click_prob[i]=1.-doc_rel[sess.doc_id[i]]*gamma[i][i-last_clkk];
                }
            }
        }
};
#endif
