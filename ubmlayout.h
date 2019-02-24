#ifndef UBMLAYOUT_H
#define UBMLAYOUT_H
//#define double long double
class ubmlayout:public model
{
    public:
        vector<double> gamma[DOCPERPAGE+2][DOCPERPAGE+2];
        vector<double> doc_rel2;
        void train()
        {
            name="Ubm_layout";
            doc_rel=vector<double>(docs.size()+1);
            doc_rel2=vector<double>(docs.size()+1);
            vector<double> nexgamma[DOCPERPAGE+2][DOCPERPAGE+2];
            vector<int> gamma_cnt[DOCPERPAGE+2][DOCPERPAGE+2];
            for(int i=0;i<=DOCPERPAGE;++i)
            {
                for(int j=0;j<=DOCPERPAGE;++j)
                {
                    gamma[i][j]=vector<double>(MAXVERTICLE+1);
                    nexgamma[i][j]=vector<double>(MAXVERTICLE+1);
                    gamma_cnt[i][j]=vector<int>(MAXVERTICLE+1);
                }
            }
            for(int i=0;i<docs.size();++i)
            {
                doc_rel[i]=0.5;
                docs[i].train_tim=1;//FOR SMOOTH
            }
            for(int i=1;i<=DOCPERPAGE;++i)
                for(int j=1;j<=i;++j)
                    for(int k=0;k<=MAXVERTICLE;++k)
                        gamma[i][j][k]=0.5;
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
                            doc_rel2[sess.doc_id[i]]+=(1.-gamma[i][i-last_clkk][docs[sess.doc_id[i]].type])*doc_rel[sess.doc_id[i]]/(1.-doc_rel[sess.doc_id[i]]*gamma[i][i-last_clkk][docs[sess.doc_id[i]].type]);
                        }
                    }
                }
                for(int i=1;i<=DOCPERPAGE;++i)
                    for(int j=1;j<=i;++j)
                        for(int k=0;k<=MAXVERTICLE;++k)
                        {
                            gamma_cnt[i][j][k]=1;
                            nexgamma[i][j][k]=0.5;
                        }
                for(auto &sess:sessions)
                {
                    if(sess.enable==false)
                        continue;
                    if(!istrain(sess,sess_cnt))
                        continue;
                    last_clkk=0;
                    for(int i=1;i<=DOCPERPAGE;++i)
                    {
                        ++gamma_cnt[i][i-last_clkk][docs[sess.doc_id[i]].type];
                        if(sess.click_time[i]>.1)
                        {
                            nexgamma[i][i-last_clkk][docs[sess.doc_id[i]].type]+=1.;
                            last_clkk=i;
                        }
                        else
                        {
                            nexgamma[i][i-last_clkk][docs[sess.doc_id[i]].type]+=(1.-doc_rel[sess.doc_id[i]])*gamma[i][i-last_clkk][docs[sess.doc_id[i]].type]/(1.-doc_rel[sess.doc_id[i]]*gamma[i][i-last_clkk][docs[sess.doc_id[i]].type]);
                        }
                    }
                }
                for(int i=0;i<docs.size();++i)
                    if(docs[i].train_tim!=0)
                        doc_rel[i]=doc_rel2[i]/docs[i].train_tim;
                for(int i=1;i<=DOCPERPAGE;++i)
                    for(int j=1;j<=i;++j)
                        for(int k=0;k<=MAXVERTICLE;++k)
                            gamma[i][j][k]=nexgamma[i][j][k]/gamma_cnt[i][j][k];
                double now_LL,now_LL2,now_LL1;
                now_LL=this->test(false,3);
                cerr<<rnd<<"\t"<<now_LL<<endl;
                //now_LL2=this->test(false,2);
                //now_LL1=this->test(false,1);
                if(now_LL-1e-8<last_LL)
                    break;
                //if(rnd%100==0)
                //    cout<<"UBM LL:=\t"<<rnd<<"\t"<<fixed<<setprecision(12)<<now_LL<<endl;
                //cout<<now_LL1<<","<<now_LL2<<","<<now_LL<<endl;
                last_LL=now_LL;
            }
            #ifdef DEBUG
                cout<<"ROUND:=\t"<<rnd<<endl;
                for(int k=0;k<=MAXVERTICLE;++k)
                    for(int i=1;i<=DOCPERPAGE;++i)
                    {
                        for(int j=1;j<=i;++j)
                            cout<<gamma[i][j][k]<<"\t";
                        puts("");
                    }
            #endif
            FILE* outfile=fopen("../output/ubmlayout_args","w");
            assert(outfile);
            for(int k=0;k<=MAXVERTICLE;++k)
            for(int i=0;i<=DOCPERPAGE;++i)
            for(int j=0;j<=DOCPERPAGE;++j)
            {
                fprintf(outfile,"%.8lf",gamma[i][j][k]);
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
        }
        void load()
        {
            FILE* infile=fopen("../output/ubmlayout_args","r");
            assert(infile);
            for(int i=0;i<=DOCPERPAGE;++i)
            {
                for(int j=0;j<=DOCPERPAGE;++j)
                {
                    gamma[i][j]=vector<double>(MAXVERTICLE+1);
                }
            }
            cerr<<MAXVERTICLE<<endl;
            for(int k=0;k<=MAXVERTICLE;++k)
            for(int i=0;i<=DOCPERPAGE;++i)
                for(int j=0;j<=DOCPERPAGE;++j)
                {
                    fscanf(infile,"%lf",&gamma[i][j][k]);
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
                    click_prob[i]=doc_rel[sess.doc_id[i]]*gamma[i][i-last_clkk][docs[sess.doc_id[i]].type];
                    last_clkk=i;
                }
                else
                {
                    click_prob[i]=1.-doc_rel[sess.doc_id[i]]*gamma[i][i-last_clkk][docs[sess.doc_id[i]].type];
                }
            }
        }
};
#endif
