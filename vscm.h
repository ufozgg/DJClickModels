#ifndef VSCM_SGD_H
#define VSCM_SGD_H
//#define double long double
class vscm:public model
{
    public:
        double forward[DOCPERPAGE+2][2],backward[DOCPERPAGE+2][2];
        double gamma[DOCPERPAGE+2][DOCPERPAGE+2][DOCPERPAGE+2],cgamma[DOCPERPAGE+2][DOCPERPAGE+2][DOCPERPAGE+2];
        //alpha_cnt=train_cnt
        vector<double> alpha,s_c,calpha,cs_c;
        vector<vector<double>> phi,sigma,cphi,csigma;
        double dlt=0.5,ddlt=0.8,eps=1e-6;
        vector<int> first_vertical;
        void train_init()
        {
            name="VSCM";
            int i,j;
            doc_rel=vector<double>(docs.size()+1);
            alpha=vector<double>(docs.size()+1);
            s_c=vector<double>(docs.size()+1);
            calpha=vector<double>(docs.size()+1);
            cs_c=vector<double>(docs.size()+1);
            int vc[DOCPERPAGE+2]={0};
            for(i=0;i<sessions.size();++i)
            {
                first_vertical.push_back(0);
                for(j=1;j<=DOCPERPAGE;++j)
                    if(docs[sessions[i].doc_id[j]].type>1)
                    {
                        first_vertical[i]=j;
                        break;
                    }
                ++vc[first_vertical[i]];
            }/*
            for(int i=0;i<=DOCPERPAGE+1;++i)
                cerr<<vc[i]<<"\t";*/
            for(int k=0;k<=DOCPERPAGE;++k)
            for(int i=0;i<=DOCPERPAGE;++i)
                for(int j=0;j<=DOCPERPAGE;++j)
                    gamma[k][i][j]=0.5;
            for(int i=0;i<=docs.size();++i)
            {
                alpha[i]=s_c[i]=0.5;
            }
            for(i=0;i<=DOCPERPAGE;++i)
            {
                phi.push_back(vector<double>());
                sigma.push_back(vector<double>());
                cphi.push_back(vector<double>());
                csigma.push_back(vector<double>());
                for(j=0;j<=MAXVERTICLE;++j)
                {
                    phi[i].push_back(0.5);
                    sigma[i].push_back(0.5);
                    cphi[i].push_back(0.);
                    csigma[i].push_back(0.);
                }
            }
        }
        void train_clear()
        {
            double pr=1;
            int i,j;
            for(i=0;i<=DOCPERPAGE;++i)
            {
                for(j=0;j<=MAXVERTICLE;++j)
                {
                    cphi[i][j]=pr*(1./phi[i][j]-1./(1.-phi[i][j]));
                    csigma[i][j]=pr*(1./sigma[i][j]-1./(1.-sigma[i][j]));
                }
            }
            for(int i=0;i<=docs.size();++i)
            {
                calpha[i]=pr*(1./alpha[i]-1./(1.-alpha[i]));
                cs_c[i]=pr*(1./s_c[i]-1./(1.-s_c[i]));
            }
            for(int k=0;k<=DOCPERPAGE;++k)
            for(int i=0;i<=DOCPERPAGE;++i)
                for(int j=0;j<=DOCPERPAGE;++j)
                    cgamma[k][i][j]=pr*(1./gamma[k][i][j]-1./(1.-gamma[k][i][j]));
        }
        void train_update()
        {
            for(int i=0;i<=DOCPERPAGE;++i)
                for(int j=0;j<=MAXVERTICLE;++j)
                {
                    //phi[i][j]=1;/*
                    if(cphi[i][j]>eps)
                        phi[i][j]=min(1.0-eps,phi[i][j]+dlt);
                    if(cphi[i][j]<-eps)
                        phi[i][j]=max(eps,phi[i][j]-dlt);/**/
                }
            for(int i=0;i<docs.size();++i)
            {
                if(calpha[i]>eps)
                    alpha[i]=min(1.0-eps,alpha[i]+dlt);
                if(calpha[i]<-eps)
                    alpha[i]=max(eps,alpha[i]-dlt);
                if(cs_c[i]>eps)
                    s_c[i]=min(1.0-eps,s_c[i]+dlt);
                if(cs_c[i]<-eps)
                    s_c[i]=max(eps,s_c[i]-dlt);
            }
            for(int k=0;k<=DOCPERPAGE;++k)
            for(int i=0;i<=DOCPERPAGE;++i)
                for(int j=0;j<=DOCPERPAGE;++j)
                {
                    if(cgamma[k][i][j]>eps)
                        gamma[k][i][j]=min(1.0-eps,gamma[k][i][j]+dlt);
                    if(cgamma[k][i][j]<-eps)
                        gamma[k][i][j]=max(eps,gamma[k][i][j]-dlt);
                }
        }
        void calc_sess(Session &sess,double weight,int k)
        {
            get_prob(sess,k);
            int last_clk=0;
            double prob_cs=forward[DOCPERPAGE][0]+forward[DOCPERPAGE][1],ptot;
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                int id=sess.doc_id[i];
                int vid=docs[id].type;
                if(sess.click_time[i]>.1)
                {
                    calpha[id]+=weight*1./alpha[id];
                    cgamma[k][i][i-last_clk]+=weight*1./gamma[k][i][i-last_clk];
                    cs_c[id]+=weight*(backward[i][1]-backward[i][0])/(s_c[id]*backward[i][1]+(1.-s_c[id])*backward[i][0]);
                    last_clk=i;
                }
                else
                {
                    double &alp=alpha[id],&gam=gamma[k][i][i-last_clk];
                    ptot=forward[i-1][1]*backward[i][1]+forward[i-1][0]*((1.-alp*gam)*backward[i][0]);
                    calpha[id]+=weight*(forward[i-1][0]*(-gam)*backward[i][0])/ptot;
                    cgamma[k][i][i-last_clk]+=weight*(forward[i-1][0]*(-alp)*backward[i][0])/ptot;
                }
            }
            //return prob_cs;
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
                for(int i=0;i<sessions.size();++i)
                {
                    auto &sess=sessions[i];
                    if(sess.enable==false)
                        continue;
                    ++sess_cnt;
                    if(!istrain(sess,sess_cnt))//在没有Overfit可能性的模型里应写作istest，在需要验证集的模型里应写作!istrain
                        continue;
                    /*int c=0;
                    for(int sp=1;sp<=DOCPERPAGE;++sp)
                        if(sess.click_time[sp]>.1)
                            ++c;
                    if(c<=1)
                        continue;//JUST FOR DEBUG*/
                    if(first_vertical[i]<=1)
                    {
                        calc_sess(sess,1,1);
                        continue;
                    }
                    double ptot,p1,p2;
                    auto newsess=sess;
                    newsess.id=-newsess.id;
                    get_prob(sess,1);
                    p1=forward[DOCPERPAGE][0]+forward[DOCPERPAGE][1];
                    int &v=first_vertical[i];
                    for(int j=first_vertical[i];j>1;--j)
                    {
                        newsess.doc_id[j]=sess.doc_id[j-1];
                        newsess.click_time[j]=sess.click_time[j-1];
                        newsess.nex_sess[j]=sess.nex_sess[j-1];
                        newsess.nex_pos[j]=sess.nex_pos[j-1];
                    }
                    newsess.doc_id[1]=sess.doc_id[v];
                    newsess.click_time[1]=sess.click_time[v];
                    newsess.nex_sess[1]=sess.nex_sess[v];
                    newsess.nex_pos[1]=sess.nex_pos[v];
                    /*for(int j=first_vertical[i];j;--j)
                    {
                        newsess.doc_id[first_vertical[i]-j+1]=sess.doc_id[j];
                        newsess.click_time[first_vertical[i]-j+1]=sess.click_time[j];
                        newsess.nex_sess[first_vertical[i]-j+1]=sess.nex_sess[j];
                        newsess.nex_pos[first_vertical[i]-j+1]=sess.nex_pos[j];
                    }*/
                    get_prob(newsess,first_vertical[i]);
                    p2=forward[DOCPERPAGE][0]+forward[DOCPERPAGE][1];
                    double ph=phi[first_vertical[i]][docs[newsess.doc_id[1]].type];
                    ptot=ph*p2+(1.-ph)*p1;
                    calc_sess(newsess,ph*p2/ptot,first_vertical[i]);
                    calc_sess(sess,(1.-ph)*p1/ptot,1);
                    cphi[first_vertical[i]][docs[newsess.doc_id[1]].type]+=(p2-p1)/ptot;
                }
                train_update();
                now_ll=this->test(false,3);
                //if(round%20==0)
                    cerr<<"Round:\t"<<round<<"\tLL:\t"<<now_ll<<"\t"<<dlt<<endl;
                last_ll=now_ll;
                dlt*=ddlt;
            }
            /*for(int i=1;i<=DOCPERPAGE;++i)
            {
                for(int j=1;j<=i;++j)
                    cerr<<gamma[i][j]<<"\t";
                cerr<<endl;
            }
            for(int i=1;i<=MAXVERTICLE;++i)
                cerr<<beta[i]<<endl;*/
            /*for(int i=1;i<docs.size();++i)
                if(docs[i].train_tim>10)
                    cerr<<alpha[i]<<"\t"<<s_c[i]<<"\t"<<s_e[i]<<endl;*/
        }
        void load()
        {
        }
        void get_prob(Session &sess,int k)
        {
            if(k>DOCPERPAGE)k=0;
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
                //cerr<<(sess.click_time[i+1]>.1)<<endl;
                if(sess.click_time[i+1]>.1)
                {
                    forward[i+1][0]=forward[i][0]*gamma[k][i+1][i+1-last_clk[i]]*alpha[id]*(1.-s_c[id]);
                    forward[i+1][1]=forward[i][0]*gamma[k][i+1][i+1-last_clk[i]]*alpha[id]*s_c[id];
                    //cerr<<"CLK!\t"<<sess.doc_id[i+1]<<"\t"<<vid<<"\t"<<forward[i+1][0]<<"\t"<<forward[i+1][1]<<"\t"<<gamma[i+1][i+1-last_clk[i]]<<"\t"<<alpha[id]<<"\t"<<beta[vid]<<endl;
                    last_clk[i+1]=i+1;
                }
                else
                {
                    forward[i+1][0]=forward[i][0]*(1.-gamma[k][i+1][i+1-last_clk[i]]*alpha[id]);
                    forward[i+1][1]=forward[i][1];
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
                    backward[i-1][0]=(backward[i][0]*(1.-s_c[id])+backward[i][1]*s_c[id])*gamma[k][i][i-last_clk[i-1]]*alpha[id];
                }
                else
                {
                    backward[i-1][0]=backward[i][0]*(1.-gamma[k][i][i-last_clk[i-1]]*alpha[id]);
                    backward[i-1][1]=backward[i][1];
                }
            }
        }
        void get_seq_clp(Session &sess,double* click_prob,double w,int k)
        {
            int last_clk=0;
            get_prob(sess,k);
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                int id=sess.doc_id[i];
                int vid=docs[id].type;
                //if(w>.1)
                //    cerr<<k<<"\t"<<i<<i-last_clk<<"\t"<<gamma[k][i][i-last_clk]<<"\t"<<alpha[id]<<"\t"<<docs[id].name<<forward[i-1][0]/(forward[i-1][0]+forward[i-1][1])*gamma[k][i][i-last_clk]*alpha[id]<<"\t"<<forward[i-1][0]<<"\t"<<(forward[i-1][0]+forward[i-1][1])<<"\n";
                if(sess.click_time[i]>.1)
                {
                    click_prob[i]=w*forward[i-1][0]/(forward[i-1][0]+forward[i-1][1])*gamma[k][i][i-last_clk]*alpha[id];
                    last_clk=i;
                }
                else
                {
                    click_prob[i]=w*(1.-forward[i-1][0]/(forward[i-1][0]+forward[i-1][1])*gamma[k][i][i-last_clk]*alpha[id]);
                }
            }
            //cerr<<"E"<<endl;
        }
        void get_click_prob(Session &sess,double* click_prob)
        {
            //click_prob[i]=P(C_i|C_1,C_2,...C_{i-1})
            //examination = P(E_k|C_1,C_2,...C_{k-1})
            double s_prob=0.0,ph;
            int v;
            double tmp[DOCPERPAGE+2];
            for(int i=0;i<=DOCPERPAGE;++i)
                click_prob[i]=0;
            for(v=1;v<=DOCPERPAGE;++v)
                if(docs[sess.doc_id[v]].type>1)
                    break;
            if(v>DOCPERPAGE)
                v=0;
            if(v<=1)
            {
                get_seq_clp(sess,click_prob,1,1);
                return;
            }
            ph=phi[v][docs[sess.doc_id[v]].type];
            get_seq_clp(sess,click_prob,1-ph,1);
            Session newsess=sess;
            /*for(int j=v;j;--j)
            {
                newsess.doc_id[v-j+1]=sess.doc_id[j];
                newsess.click_time[v-j+1]=sess.click_time[j];
                newsess.nex_sess[v-j+1]=sess.nex_sess[j];
                newsess.nex_pos[v-j+1]=sess.nex_pos[j];
            }*/
            newsess.id=-newsess.id;
            for(int j=v;j>1;--j)
            {
                newsess.doc_id[j]=sess.doc_id[j-1];
                newsess.click_time[j]=sess.click_time[j-1];
                newsess.nex_sess[j]=sess.nex_sess[j-1];
                newsess.nex_pos[j]=sess.nex_pos[j-1];
            }
            newsess.doc_id[1]=sess.doc_id[v];
            newsess.click_time[1]=sess.click_time[v];
            newsess.nex_sess[1]=sess.nex_sess[v];
            newsess.nex_pos[1]=sess.nex_pos[v];
            get_seq_clp(newsess,tmp,ph,v);
            //*
            for(int h=v+1;h<=DOCPERPAGE;++h)
                click_prob[h]+=tmp[h];
            for(int h=1;h<v;++h)
                click_prob[h]+=tmp[h+1];
            click_prob[v]+=tmp[1];/**/
            /*
            for(int j=1;j<=DOCPERPAGE;++j)
                click_prob[j]+=tmp[j];/**/
            /*for(int j=v;j;--j)
                click_prob[j]+=tmp[v-j+1];
            for(int j=v+1;j<=DOCPERPAGE;++j)
                click_prob[j]+=tmp[j];*/
        }
};
#endif
