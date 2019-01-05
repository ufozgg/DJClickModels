#ifndef UBM_H
#define UBM_H
#define double long double
class ubm:public model
{
    public:
        double gamma[DOCPERPAGE+2][DOCPERPAGE+2];
        vector<double> doc_rel2;
        void train()
        {
            name="Ubm:";
            doc_rel=vector<double>(docs.size()+1);
            doc_rel2=vector<double>(docs.size()+1);
            for(int i=0;i<docs.size();++i)
            {
                doc_rel[i]=0.5;
                docs[i].train_tim=1;//FOR SMOOTH
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
            double nexgamma[DOCPERPAGE+2][DOCPERPAGE+2];
            int gamma_cnt[DOCPERPAGE+2][DOCPERPAGE+2];
            int rnd;
            for(rnd=1;rnd<=MAXROUND;++rnd)
            {
                for(int i=0;i<docs.size();++i)
                    doc_rel2[i]=0.5;//FOR SMOOTH
                int last_clkk;
                for(auto &sess:sessions)
                {
                    if(sess.enable==false)
                        continue;
                    if(!istrain(sess,sess_cnt))
                        continue;
                    last_clkk=0;
                    for(int i=1;i<=DOCPERPAGE;++i)
                    {
                        if(sess.click_time[i]>.1)
                        {
                            doc_rel2[sess.doc_id[i]]+=1;
                            last_clkk=i;
                        }
                        else
                        {
                            doc_rel2[sess.doc_id[i]]+=(1.-gamma[i][i-last_clkk])*doc_rel[sess.doc_id[i]]/(1.-doc_rel[sess.doc_id[i]]*gamma[i][i-last_clkk]);
                        }
                    }
                }
                memset(nexgamma,0,sizeof(gamma));
                memset(gamma_cnt,0,sizeof(gamma_cnt));
                for(auto &sess:sessions)
                {
                    if(sess.enable==false)
                        continue;
                    if(!istrain(sess,sess_cnt))
                        continue;
                    last_clkk=0;
                    for(int i=1;i<=DOCPERPAGE;++i)
                    {
                        ++gamma_cnt[i][i-last_clkk];
                        if(sess.click_time[i]>.1)
                        {
                            nexgamma[i][i-last_clkk]+=1.;
                            last_clkk=i;
                        }
                        else
                        {
                            nexgamma[i][i-last_clkk]+=(1.-doc_rel[sess.doc_id[i]])*gamma[i][i-last_clkk]/(1.-doc_rel[sess.doc_id[i]]*gamma[i][i-last_clkk]);
                        }
                    }
                }
                for(int i=0;i<docs.size();++i)
                    if(docs[i].train_tim!=0)
                        doc_rel[i]=doc_rel2[i]/docs[i].train_tim;
                for(int i=1;i<=DOCPERPAGE;++i)
                    for(int j=1;j<=i;++j)
                        gamma[i][j]=nexgamma[i][j]/gamma_cnt[i][j];
                double now_LL,now_LL2,now_LL1;
                now_LL=this->test(false,3);
                //now_LL2=this->test(false,2);
                //now_LL1=this->test(false,1);
                if(now_LL-1e-8<last_LL)
                    break;
                //if(rnd%100==0)
                //    cout<<"UBM LL:=\t"<<rnd<<"\t"<<fixed<<setprecision(12)<<now_LL<<endl;
                //cout<<now_LL1<<","<<now_LL2<<","<<now_LL<<endl;
                last_LL=now_LL;
            }
            cout<<"ROUND:=\t"<<rnd<<endl;
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                for(int j=1;j<=i;++j)
                    cout<<gamma[i][j]<<"\t";
                puts("");
            }
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
