#ifndef MVCM2_SGD_H
#define MVCM2_SGD_H
//#define double long double
extern vector<bool> gammaenable;
extern vector<bool> phienable;
extern vector<bool> sigmaenable;
extern double pr;
extern double pr0[10];
/*class mvcm_sess_data
{
    public:
    double click_rate[DOCPERPAGE+2],forward[DOCPERPAGE+2][2],backward[DOCPERPAGE+2][2];
    int pos_in_sess[DOCPERPAGE+2],st[DOCPERPAGE+2],en[DOCPERPAGE+2],way[DOCPERPAGE+2];
    vector<int> ver_pos;
    vector<int> dphi_id,dsigma_id;
    vector<double> dphi,dsigma;
    double tot_p;
};*/
class mvcm2:public model
{
    public:
        vector<double> gamma;
        vector<double> phi;
        vector<double> sigma;
        vector<int> arggamma,argphi,argsigma;
        vector<double> alpha,s_c,beta;
        vector<double> Lalpha,Lbeta,Ls_c,Lgamma,Lphi,Lsigma;
        vector<shared_ptr<atomic<long long>>> cgamma,cphi,csigma,calpha,cbeta,cs_c;
        //vector<double> cgamma,cphi,csigma,calpha,cs_c;
        double maxd;
        double dlt=0.4,ddlt=0.7,eps=1e-6;
        vector<int> first_vertical;
        double get_rel(int id)
        {
            //return alpha[id]+s_c[id];
            //return s_c[id];
            return alpha[id]*s_c[id];
        }
        void train_init()
        {
            name="MVCM_sconly_add";
            int i,j;
            doc_rel=vector<double>(docs.size()+1);
            alpha=vector<double>(docs.size()+1);
            beta=vector<double>(docs.size()+1);
            s_c=vector<double>(docs.size()+1);
            Lalpha=vector<double>(docs.size()+1);
            Lbeta=vector<double>(docs.size()+1);
            Ls_c=vector<double>(docs.size()+1);
            for(int i=0;i<=docs.size();++i)
            {
                alpha[i]=beta[i]=s_c[i]=0.5;
                calpha.push_back(shared_ptr<atomic<long long> >(new atomic<long long>(0)));
                cbeta.push_back(shared_ptr<atomic<long long> >(new atomic<long long>(0)));
                cs_c.push_back(shared_ptr<atomic<long long> >(new atomic<long long>(0)));/**/
                //calpha.push_back(0);
                //cs_c.push_back(0);
            }
            argphi=vector<int>{DOCPERPAGE+2,DOCPERPAGE+2,MAXVERTICLE+2};/*上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型*/
            argsigma=vector<int>{DOCPERPAGE+2,DOCPERPAGE+2,MAXVERTICLE+2};/*上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型*/
            arggamma=vector<int>{DOCPERPAGE+2,DOCPERPAGE+2,MAXVERTICLE+2,4,DOCPERPAGE+2,DOCPERPAGE+2};/*上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型，当前顺序，结果位置，上一个点击位置*/
            int siz;
            siz=1;
            for(auto i:arggamma)
                siz*=i;
            gamma.resize(siz+2);
            Lgamma.resize(siz+2);
            for(int i=0;i<=siz+2;++i)
            {
                cgamma.push_back(shared_ptr<atomic<long long> >(new atomic<long long>(0)));
                //cgamma.push_back(0);
            }
            siz=1;
            for(auto i:argsigma)
                siz*=i;
            sigma.resize(siz+2);
            Lsigma.resize(siz+2);
            for(int i=0;i<=siz+2;++i)
            {
                csigma.push_back(shared_ptr<atomic<long long> >(new atomic<long long>(0)));
                //csigma.push_back(0);
            }
            siz=1;
            for(auto i:argphi)
                siz*=i;
            phi.resize(siz+2);
            Lphi.resize(siz+2);
            for(int i=0;i<=siz+2;++i)
            {
                cphi.push_back(shared_ptr<atomic<long long> >(new atomic<long long>(0)));
                //cphi.push_back(0);
            }
            for(auto &i:gamma)
                i=0.5;
            for(auto &i:sigma)
                i=0.5;
            for(auto &i:phi)
                i=0.5;
        }
        bool is_vertical(int &x)
        {
            return docs[x].type>1;//||docs[x].type==7||docs[x].type==8;//||docs[x].type==14||docs[x].type>=16;
            //return docs[x].type>1&&docs[x].type<9||docs[x].type==10||docs[x].type==14||docs[x].type==15;
        }
        void clear_vec(vector<double> &arg,vector<shared_ptr<atomic<long long>>> &dt,double A=pr,double B=pr)
        {
            //assert(arg.size()==dt.size());
            #pragma omp parallel for
            for(int i=0;i<arg.size();++i)
            {
                //dt[i]=(long long)(1e6*(A/arg[i]-B/(1.-arg[i])));
                //atomic_init(dt[i].get(),(long long)(1e6*(A/arg[i]-B/(1.-arg[i]))));
                atomic_init(dt[i].get(),(long long)(1e6*((A/arg[i]-B/(1.-arg[i])))));
            }
        }
        void train_clear()
        {
            clear_vec(alpha,calpha,pr0[0],pr0[1]);
            clear_vec(beta,cbeta,pr0[0],pr0[1]);
            clear_vec(s_c,cs_c,pr0[2],pr0[3]);
            clear_vec(sigma,csigma,pr0[4],pr0[5]);
            clear_vec(gamma,cgamma,pr0[6],pr0[7]);
            clear_vec(phi,cphi,pr0[8],pr0[9]);
        }
        void update_vec(vector<double> &arg,vector<shared_ptr<atomic<long long>>> &dt)
        {
            //assert(arg.size()==dt.size());
            #pragma omp parallel for
            for(int i=0;i<arg.size();++i)
            {
                /*if(fabs(dt[i])>maxd)
                    maxd=fabs(dt[i]);*/
                if(*dt[i].get()>0)
                    arg[i]=min(1.-eps*1e-3,arg[i]+dlt);
                else
                    arg[i]=max(eps*1e-3,arg[i]-dlt);
            }
        }
        void train_update()
        {
            update_vec(alpha,calpha);
            update_vec(beta,cbeta);
            update_vec(s_c,cs_c);
            update_vec(sigma,csigma);
            update_vec(gamma,cgamma);
            update_vec(phi,cphi);
        }
        /*顺序0：1234，顺序1：4123，顺序2：4321*/
        int phiid(const vector<int> &idx)
        {
            assert(idx.size()==argphi.size());
            int tid=0;
            for(int i=0;i<idx.size();++i)
                if(phienable[i])
                    tid=tid*argphi[i]+idx[i];
            return tid;
        }
        double getphi(const vector<int> &idx)
        {
            return phi[phiid(idx)];
        }
        double addcphi(const vector<int> &idx,double v)
        {
            if(v>1e5)
                v=1e5;
            if(v<-1e5)
                v=-1e5;
            atomic_fetch_add(cphi[phiid(idx)].get(),(long long)(1e6*v));
        }
        int sigmaid(const vector<int> &idx)
        {
            assert(idx.size()==argsigma.size());
            int tid=0;
            for(int i=0;i<idx.size();++i)
                if(sigmaenable[i])
                    tid=tid*argsigma[i]+idx[i];
            return tid;
        }
        double getsigma(const vector<int> &idx)
        {
            return sigma[sigmaid(idx)];
        }
        double addcsigma(const vector<int> &idx,double v)
        {
            if(v>1e5)
                v=1e5;
            if(v<-1e5)
                v=-1e5;
            atomic_fetch_add(csigma[sigmaid(idx)].get(),(long long)(1e6*v));
        }
        int gammaid(const vector<int> &idx)
        {
            assert(idx.size()==arggamma.size());
            int tid=0;
            for(int i=0;i<idx.size();++i)
                if(gammaenable[i])
                    tid=tid*arggamma[i]+idx[i];
            return tid;
        }
        double getgamma(const vector<int> &idx)
        {
            return gamma[gammaid(idx)];
        }
        void addcgamma(const vector<int> &idx,double v)
        {
            if(v>1e5)
                v=1e5;
            if(v<-1e5)
                v=-1e5;
            //cerr<<*cgamma[gammaid(idx)].get()<<"\t"<<v<<endl;
            atomic_fetch_add(cgamma[gammaid(idx)].get(),(long long)(1e6*v));
            //cerr<<*cgamma[gammaid(idx)].get()<<"\t"<<v<<endl;
        }
        /*phi sigma 上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型*/
        /*gamma 上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型，当前顺序，结果位置，上一个点击位置*/
        double calc_seq_prob(double prob,Session &sess,bool upd,mvcm_sess_data &sess_data)
        {
            memset(sess_data.forward,0,sizeof(sess_data.forward));
            memset(sess_data.backward,0,sizeof(sess_data.backward));
            sess_data.forward[0][0]=1;
            sess_data.backward[DOCPERPAGE+1][0]=sess_data.backward[DOCPERPAGE+1][1]=1;
            int last_clk[DOCPERPAGE+2],last_ver=0;
            memset(last_clk,0,sizeof(last_clk));
            double ret=prob,exam;
            last_clk[0]=last_clk[1]=0;
            double ga[DOCPERPAGE+2],al[DOCPERPAGE+2],sc[DOCPERPAGE+2],be[DOCPERPAGE+2];
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                ga[i]=getgamma({sess_data.st[i],sess_data.en[i],docs[sess.doc_id[sess_data.en[i]]].type,sess_data.way[i],i,last_clk[i]});
                al[i]=(sess_data.way[i]==0?alpha[sess.doc_id[sess_data.pos_in_sess[i]]]:beta[sess.doc_id[sess_data.pos_in_sess[i]]]);
                sc[i]=s_c[sess.doc_id[sess_data.pos_in_sess[i]]];
                exam=sess_data.forward[i-1][0]/(sess_data.forward[i-1][0]+sess_data.forward[i-1][1]);
                if(sess.click_time[sess_data.pos_in_sess[i]]>.1)
                {
                    sess_data.click_rate[sess_data.pos_in_sess[i]]+=prob*exam*ga[i]*al[i];
                    sess_data.forward[i][0]=sess_data.forward[i-1][0]*ga[i]*al[i]*(1.-sc[i]);
                    sess_data.forward[i][1]=sess_data.forward[i-1][0]*ga[i]*al[i]*sc[i];
                    ret*=exam*ga[i]*al[i];
                    //exam=(1.-s_c[sess.doc_id[pos_in_sess[i]]]);
                    last_clk[i+1]=i;
                }
                else
                {
                    sess_data.click_rate[sess_data.pos_in_sess[i]]+=prob*(1.-exam*ga[i]*al[i]);
                    sess_data.forward[i][0]=sess_data.forward[i-1][0]*(1.-ga[i]*al[i]);
                    sess_data.forward[i][1]=sess_data.forward[i-1][1];
                    ret*=(1.-exam*ga[i]*al[i]);
                    last_clk[i+1]=last_clk[i];
                }
            }
            for(int i=DOCPERPAGE;i;--i)
                if(sess.click_time[sess_data.pos_in_sess[i]]>.1)
                {
                    sess_data.backward[i][1]=0;
                    sess_data.backward[i][0]=ga[i]*al[i] * (sess_data.backward[i+1][0]*(1.-sc[i]) + sess_data.backward[i+1][1]*sc[i]);
                }
                else
                {
                    sess_data.backward[i][1]=sess_data.backward[i+1][1];
                    sess_data.backward[i][0]=sess_data.backward[i+1][0]*(1.-ga[i]*al[i]);
                }
            if(!upd)
                return ret;
            if(ret<eps)
                return ret;
            for(int i=0;i<sess_data.dphi_id.size();++i)
                atomic_fetch_add(cphi[sess_data.dphi_id[i]].get(),(long long)(1e6*ret/sess_data.tot_p*sess_data.dphi[i]));
            for(int i=0;i<sess_data.dsigma_id.size();++i)
                atomic_fetch_add(csigma[sess_data.dsigma_id[i]].get(),(long long)(1e6*ret/sess_data.tot_p*sess_data.dsigma[i]));
            for(int i=1;i<=10;++i)
            {
                exam=sess_data.forward[i-1][0]/(sess_data.forward[i-1][0]+sess_data.forward[i-1][1]);
                if(sess.click_time[sess_data.pos_in_sess[i]]>.1)
                {
                    addcgamma({sess_data.st[i],sess_data.en[i],docs[sess.doc_id[sess_data.en[i]]].type,sess_data.way[i],i,last_clk[i]},ret/sess_data.tot_p/ga[i]);
                    if(sess_data.way[i]==0)
                        atomic_fetch_add(calpha[sess.doc_id[sess_data.pos_in_sess[i]]].get(),(long long)(1e6*ret/sess_data.tot_p/al[i]));
                    else
                        atomic_fetch_add(cbeta[sess.doc_id[sess_data.pos_in_sess[i]]].get(),(long long)(1e6*ret/sess_data.tot_p/al[i]));
                    atomic_fetch_add(cs_c[sess.doc_id[sess_data.pos_in_sess[i]]].get(),(long long)(1e6*(ret/sess_data.tot_p/(sess_data.backward[i+1][0]*(1.-sc[i]) + sess_data.backward[i+1][1]*sc[i]) * (sess_data.backward[i+1][1]-sess_data.backward[i+1][0]))));
                }
                else
                {
                    if(sess_data.way[i]==0)
                        atomic_fetch_add(calpha[sess.doc_id[sess_data.pos_in_sess[i]]].get(),(long long)(1e6*-ga[i]/sess_data.tot_p*prob*(sess_data.forward[i-1][0]*sess_data.backward[i+1][0])));
                    else
                        atomic_fetch_add(cbeta[sess.doc_id[sess_data.pos_in_sess[i]]].get(),(long long)(1e6*-ga[i]/sess_data.tot_p*prob*(sess_data.forward[i-1][0]*sess_data.backward[i+1][0])));
                    addcgamma({sess_data.st[i],sess_data.en[i],docs[sess.doc_id[sess_data.en[i]]].type,sess_data.way[i],i,last_clk[i]},-al[i]/sess_data.tot_p*prob*(sess_data.forward[i-1][0]*sess_data.backward[i+1][0]));
                }
            }
            return ret;
        }
        double dfs(int now,double prob,Session &sess,bool upd,mvcm_sess_data &sess_data)
        {
            if(now==sess_data.ver_pos.size())
            {
                for(int i=sess_data.ver_pos[now-1]+1;i<=DOCPERPAGE;++i)
                {
                    sess_data.pos_in_sess[i]=i;
                    sess_data.way[i]=0;
                }
                return calc_seq_prob(prob,sess,upd,sess_data);
            }
            if(sess_data.ver_pos[now]-sess_data.ver_pos[now-1]==1)
            {
                sess_data.pos_in_sess[sess_data.ver_pos[now]]=sess_data.ver_pos[now];
                sess_data.way[sess_data.ver_pos[now]]=0;
                return dfs(now+1,prob,sess,upd,sess_data);
            }
            double ret=0;
            if(sess_data.ver_pos[now]-sess_data.ver_pos[now-1]==2)
            {
                for(int i=sess_data.ver_pos[now-1]+1;i<=sess_data.ver_pos[now];++i)
                {
                    sess_data.pos_in_sess[i]=i;
                    sess_data.way[i]=0;
                }
                sess_data.dphi_id.push_back(phiid({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}));
                sess_data.dphi.push_back(-1./(1.-getphi({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type})));//
                ret+=dfs(now+1,prob*(1.-getphi({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type})),sess,upd,sess_data);
                sess_data.dphi_id.pop_back();
                sess_data.dphi.pop_back();
                for(int i=sess_data.ver_pos[now-1]+1;i<=sess_data.ver_pos[now];++i)
                {
                    sess_data.pos_in_sess[i]=sess_data.ver_pos[now-1]+sess_data.ver_pos[now]-i+1;
                    sess_data.way[i]=1;
                }
                sess_data.dphi_id.push_back(phiid({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}));//
                sess_data.dphi.push_back(1./getphi({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}));
                ret+=dfs(now+1,prob*getphi({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}),sess,upd,sess_data);
                sess_data.dphi_id.pop_back();
                sess_data.dphi.pop_back();
                return ret;
            }
            for(int i=sess_data.ver_pos[now-1]+1;i<=sess_data.ver_pos[now];++i)
            {
                sess_data.pos_in_sess[i]=i;
                sess_data.way[i]=0;
            }
            sess_data.dphi_id.push_back(phiid({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}));
            sess_data.dphi.push_back(-1./(1.-getphi({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type})));//
            ret+=dfs(now+1,prob*(1.-getphi({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type})),sess,upd,sess_data);
            sess_data.dphi_id.pop_back();
            sess_data.dphi.pop_back();
            for(int i=sess_data.ver_pos[now-1]+1;i<=sess_data.ver_pos[now];++i)
            {
                sess_data.pos_in_sess[i]=sess_data.ver_pos[now-1]+sess_data.ver_pos[now]-i+1;
                sess_data.way[i]=1;
            }
            sess_data.dphi_id.push_back(phiid({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}));
            sess_data.dphi.push_back(1./getphi({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}));//
            sess_data.dsigma_id.push_back(sigmaid({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}));
            sess_data.dsigma.push_back(1./getsigma({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}));//
            ret+=dfs(now+1,prob*getphi({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type})*getsigma({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}),sess,upd,sess_data);
            sess_data.dphi_id.pop_back();
            sess_data.dphi.pop_back();
            sess_data.dsigma_id.pop_back();
            sess_data.dsigma.pop_back();
            sess_data.way[sess_data.ver_pos[now-1]+1]=2;
            sess_data.pos_in_sess[sess_data.ver_pos[now-1]+1]=sess_data.ver_pos[now];
            for(int i=sess_data.ver_pos[now-1]+2;i<=sess_data.ver_pos[now];++i)
            {
                sess_data.pos_in_sess[i]=i-1;
                sess_data.way[i]=2;
            }
            sess_data.dphi_id.push_back(phiid({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}));
            sess_data.dphi.push_back(1./getphi({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}));
            sess_data.dsigma_id.push_back(sigmaid({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type}));
            sess_data.dsigma.push_back(-1./(1.-getsigma({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type})));
            ret+=dfs(now+1,prob*getphi({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type})*(1.-getsigma({sess_data.ver_pos[now-1],sess_data.ver_pos[now],docs[sess.doc_id[sess_data.ver_pos[now]]].type})),sess,upd,sess_data);
            sess_data.dphi_id.pop_back();
            sess_data.dphi.pop_back();
            sess_data.dsigma_id.pop_back();
            sess_data.dsigma.pop_back();
            return ret;
        }
        void sess_prework(Session &sess,mvcm_sess_data &sess_data)
        {
            sess_data.ver_pos.clear();
            sess_data.ver_pos.push_back(0);
            sess_data.dphi.clear();
            sess_data.dphi_id.clear();
            sess_data.dsigma.clear();
            sess_data.dsigma_id.clear();
            for(int i=1;i<=DOCPERPAGE;++i)
                if(is_vertical(sess.doc_id[i]))
                {
                    sess_data.ver_pos.push_back(i);
                    for(int j=sess_data.ver_pos[sess_data.ver_pos.size()-2]+1;j<=sess_data.ver_pos[sess_data.ver_pos.size()-1];++j)
                    {
                        sess_data.st[j]=sess_data.ver_pos[sess_data.ver_pos.size()-2];
                        sess_data.en[j]=sess_data.ver_pos[sess_data.ver_pos.size()-1];
                    }
                }
            for(int j=sess_data.ver_pos[sess_data.ver_pos.size()-1]+1;j<=DOCPERPAGE;++j)
            {
                sess_data.st[j]=sess_data.ver_pos[sess_data.ver_pos.size()-1];
                sess_data.en[j]=DOCPERPAGE+1;
            }
            memset(sess_data.click_rate,0,sizeof(sess_data.click_rate));
        }
        void calc_prob(Session &sess,int upd,mvcm_sess_data &sess_data)
        {
            sess_prework(sess,sess_data);
            sess_data.tot_p=dfs(1,1.0,sess,0,sess_data);
            if(upd==-1)
                return;
            dfs(1,1.0,sess,1,sess_data);
        }
        void train()
        {
            double last_ll=-100,now_ll;
            int last_clk,sess_cnt=0;
            train_init();
            double prd=1.,prdlt=0.98,ppr[10];
            int tag=0;
            for(int round=1;round<=MAXROUND;++round)
            {
                train_clear();
                double maxdlt=0;
                #pragma omp parallel for
                for(int id=0;id<sessions.size();++id)
                {
                    auto &sess=sessions[id];
                    if(sess.enable==false||!istrain(sess,sess_cnt))//在没有Overfit可能性的模型里应写作istest，在需要验证集的模型里应写作!istrain
                        continue;
                    ++sess_cnt;
                    mvcm_sess_data sess_data;
                    calc_prob(sess,1,sess_data);
                }
                maxd=0;
                train_update();
                //now_ll=this->test(false,1);
                //if(round%20==0)
                //    cerr<<"Round:\t"<<round<<"\tLL:\t"<<"\t"<<this->test(false,2)<<"\t"<<dlt<<"\t"<<maxd<<endl;
                //cerr<<round<<"\t"<<dlt<<endl;
                if(false)
                {
                    dlt=0.1;
                    if(tag==0)
                    {
                        last_ll=this->test(false,2);
                        tag=1;
                    }
                    else
                    {
                        now_ll=this->test(false,2);
                        if(now_ll<=last_ll)
                        {
                            for(int i=0;i<10;++i)
                                pr0[i]=ppr[i];
                            alpha.assign(Lalpha.begin(),Lalpha.end());
                            beta.assign(Lbeta.begin(),Lbeta.end());
                            gamma.assign(Lgamma.begin(),Lgamma.end());
                            s_c.assign(Ls_c.begin(),Ls_c.end());
                            phi.assign(Lphi.begin(),Lphi.end());
                            sigma.assign(Lsigma.begin(),Lsigma.end());
                            cerr<<"Not ok"<<prd<<endl;
                        }
                        else
                        {
                            for(int i=0;i<10;++i)
                                cerr<<pr0[i]<<"\t";
                            cerr<<now_ll<<prd<<endl;
                            last_ll=now_ll;
                        }
                    }
                    
                    for(int i=0;i<10;++i)
                    {
                        ppr[i]=pr0[i];
                        if(rand()&1)
                        {
                            if(rand()&1)
                                pr0[i]+=prd;
                            else
                                pr0[i]=max(pr0[i]-prd,1e-2);
                        }
                    }
                    Lalpha.assign(alpha.begin(),alpha.end());
                    Lbeta.assign(beta.begin(),beta.end());
                    Lgamma.assign(gamma.begin(),gamma.end());
                    Ls_c.assign(s_c.begin(),s_c.end());
                    Lphi.assign(phi.begin(),phi.end());
                    Lsigma.assign(sigma.begin(),sigma.end());
                    
                    prd*=prdlt;
                }
                dlt*=ddlt;
                /*last_ll=now_ll;
                for(int k=0;k<=MAXVERTICLE;++k)
                {
                    double summ=0;
                    for(int i=0;i<DOCPERPAGE;++i)
                        for(int j=1;j<=DOCPERPAGE;++j)
                        {
                            summ+=getphi({i,j,k});
                        }
                    cerr<<summ<<"\t";
                }
                cerr<<endl;*/
            }
        }
        void load()
        {
        }
        void get_click_prob(Session &sess,double* click_prob)
        {
            //click_prob[i]=P(C_i|C_1,C_2,...C_{i-1})
            //examination = P(E_k|C_1,C_2,...C_{k-1})
            double s_prob=0.0,ph,sg;
            int v;
            double tmp[DOCPERPAGE+2];
            for(int i=0;i<=DOCPERPAGE;++i)
                click_prob[i]=0;
            mvcm_sess_data sess_data;
            calc_prob(sess,-1,sess_data);
            for(int i=0;i<=DOCPERPAGE;++i)
                click_prob[i]=sess_data.click_rate[i];
		}
};
#endif
