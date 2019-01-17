#ifndef DCM_H
#define DCM_H
//#define double long double
class dcm:public model
{
    public:
        vector<int> doc_cnt,doc_clk;
        int pos_last_clk[DOCPERPAGE+2],pos_clk[DOCPERPAGE+2];
        double gamma[DOCPERPAGE+2];
        dcm()
        {
            name="DCM";
        }
        void train()
        {
            doc_cnt=vector<int>(docs.size()+1);
            doc_clk=vector<int>(docs.size()+1);
            doc_rel=vector<double>(docs.size()+1);
            for(int i=0;i<docs.size();++i)
            {
                doc_cnt[i]=0;
                doc_clk[i]=0;
            }
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                pos_clk[i]=0;
                pos_last_clk[i]=0;
            }
            int sess_cnt=0;
            for(auto &sess:sessions)
            {
                if(sess.enable==false)
                    continue;
                ++sess_cnt;
                if(istest(sess,sess_cnt))
                    continue;
                bool last_clk=false;
                for(int i=1;i<=DOCPERPAGE;++i)
                    if(sess.click_time[i]>.1)
                        last_clk=true;
                for(int i=DOCPERPAGE;i;--i)
                {
                    ++docs[sess.doc_id[i]].train_tim;
                    if(sess.click_time[i]>.1)//clicked
                    {
                        ++pos_clk[i];
                        if(last_clk)
                        {
                            last_clk=false;
                            ++pos_last_clk[i];
                        }
                        ++doc_clk[sess.doc_id[i]];
                    }
                    if(last_clk==false)
                    {
                        ++doc_cnt[sess.doc_id[i]];
                    }
                }
            }
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                //cout<<pos_clk[i]<<"\t"<<pos_last_clk[i]<<endl;
                if(pos_clk[i])
                    gamma[i]=((double)pos_clk[i]-pos_last_clk[i])/pos_clk[i];
                else
                    gamma[i]=1.0;
            }
            double t1=0,t2=0;
            for(int i=0;i<docs.size();++i)
            {
                if(doc_clk[i])
                    t1+=doc_clk[i];
                if(doc_cnt[i])
                    t2+=doc_cnt[i];
            }
            cout<<t1/t2<<endl;
            double K=2;
            t1=t1/t2*K;
            t2=K;
            //t1=0.4*K;
            for(int i=0;i<=docs.size();++i)
            {
                doc_rel[i]=(doc_clk[i]+t1)/(doc_cnt[i]+t2);
            }
            #ifdef DEBUG
                cout<<"Gamma:";
                for(int i=1;i<=DOCPERPAGE;++i)
                    cout<<"\t"<<gamma[i];
                cout<<endl;
            #endif
            FILE* outfile=fopen("../output/dcm_args","w");
            assert(outfile);
            for(int i=0;i<=DOCPERPAGE;++i)
            {
                fprintf(outfile,"%.8lf",gamma[i]);
                if(i==DOCPERPAGE)
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
            FILE* infile=fopen("../output/dcm_args","r");
            assert(infile);
            for(int i=0;i<=DOCPERPAGE;++i)
            {
                fscanf(infile,"%lf",gamma+i);
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
            double examination=1.0;
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                if(sess.click_time[i]>.1)
                {
                    click_prob[i]=examination*doc_rel[sess.doc_id[i]];
                    examination=gamma[i];
                    //click_prob[i]=examination*doc_rel[sess.doc_id[i]];
                    //examination=click_prob[i]*gamma[i];
                }
                else
                {
                    click_prob[i]=1.-examination*doc_rel[sess.doc_id[i]];
                    examination*=(1.-doc_rel[sess.doc_id[i]])/(1.-examination*doc_rel[sess.doc_id[i]]);
                    //click_prob[i]=click_prob[i-1]-examination*doc_rel[sess.doc_id[i]];
                    //examination*=1.-doc_rel[sess.doc_id[i]];
                }
            }
        }
        /*void test()
        {
            model:test();
            for(int i=1;i<=DOCPERPAGE;++i)
                cout<<1.0-(double)pos_last_clk[i]/pos_clk[i]<<"\t";
            
            puts("");
            for(int i=1;i<=DOCPERPAGE;++i)
                cout<<pos_clk[i]-pos_last_clk[i]<<"\t"<<pos_clk[i]<<"\n";
        }*/
};
#endif
