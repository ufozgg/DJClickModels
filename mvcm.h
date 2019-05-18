#ifndef MVCM_SGD_H
#define MVCM_SGD_H
//#define double long double
extern vector<bool> gammaenable;
extern vector<bool> phienable;
extern vector<bool> sigmaenable;
extern double pr;
class mvcm:public model
{
    public:
        vector<double> gamma,cgamma;
        vector<double> phi,cphi;
        vector<double> sigma,csigma;
        vector<int> arggamma,argphi,argsigma;
        vector<double> alpha,s_c,calpha,cs_c;
        double dlt=0.2,ddlt=0.9,eps=1e-6;
        vector<int> first_vertical;
        void train_init()
        {
            name="MVCM";
            int i,j;
            doc_rel=vector<double>(docs.size()+1);
            alpha=vector<double>(docs.size()+1);
            s_c=vector<double>(docs.size()+1);
            calpha=vector<double>(docs.size()+1);
            cs_c=vector<double>(docs.size()+1);
            for(int i=0;i<=docs.size();++i)
            {
                alpha[i]=s_c[i]=0.5;
            }
            argphi=vector<int>{DOCPERPAGE+2,DOCPERPAGE+2,MAXVERTICLE+2};/*上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型*/
            argsigma=vector<int>{DOCPERPAGE+2,DOCPERPAGE+2,MAXVERTICLE+2};/*上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型*/
            arggamma=vector<int>{DOCPERPAGE+2,DOCPERPAGE+2,MAXVERTICLE+2,4,DOCPERPAGE+2,DOCPERPAGE+2};/*上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型，当前顺序，结果位置，上一个点击位置*/
            int siz;
            siz=1;
            for(auto i:arggamma)
                siz*=i;
            gamma.resize(siz+2);
            cgamma.resize(siz+2);
            siz=1;
            for(auto i:argsigma)
                siz*=i;
            sigma.resize(siz+2);
            csigma.resize(siz+2);
            siz=1;
            for(auto i:argphi)
                siz*=i;
            phi.resize(siz+2);
            cphi.resize(siz+2);
            for(auto &i:gamma)
                i=0.5;
            for(auto &i:sigma)
                i=0.5;
            for(auto &i:phi)
                i=0.5;
            for(auto &i:cgamma)
                i=0;
            for(auto &i:csigma)
                i=0;
            for(auto &i:cphi)
                i=0;
        }
        bool is_vertical(int &x)
        {
            return docs[x].type>1&&docs[x].type<9||docs[x].type==10||docs[x].type==14||docs[x].type==15;
        }
        void clear_vec(vector<double> &arg,vector<double> &dt)
        {
            assert(arg.size()==dt.size());
            for(int i=0;i<arg.size();++i)
            {
                dt[i]=pr*(1./arg[i]-1./(1.-arg[i]));
            }
        }
        void train_clear()
        {
            clear_vec(alpha,calpha);
            clear_vec(s_c,cs_c);
            clear_vec(sigma,csigma);
            clear_vec(gamma,cgamma);
            clear_vec(phi,cphi);
        }
        void update_vec(vector<double> &arg,vector<double> &dt)
        {
            assert(arg.size()==dt.size());
            for(int i=0;i<arg.size();++i)
            {
                if(dt[i]>eps)
                    arg[i]=min(1.-eps,arg[i]+dlt);
                if(dt[i]<-eps)
                    arg[i]=max(eps,arg[i]-dlt);
            }
        }
        void train_update()
        {
            update_vec(alpha,calpha);
            update_vec(s_c,cs_c);
            update_vec(sigma,csigma);
            update_vec(gamma,cgamma);
            update_vec(phi,cphi);
        }
        double click_rate[DOCPERPAGE+2],forward[DOCPERPAGE+2][2],backward[DOCPERPAGE+2][2];
        /*顺序0：1234，顺序1：4123，顺序2：4321*/
        int view_len[DOCPERPAGE+2];
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
            cphi[phiid(idx)]+=v;
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
            csigma[sigmaid(idx)]+=v;
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
        double addcgamma(const vector<int> &idx,double v)
        {
            cgamma[gammaid(idx)]+=v;
        }
        int pos_in_sess[DOCPERPAGE+2],st[DOCPERPAGE+2],en[DOCPERPAGE+2],way[DOCPERPAGE+2];
        vector<int> ver_pos;
        vector<int> dphi_id,dsigma_id;
        vector<int> dphi,dsigma;
        /*phi sigma 上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型*/
        /*gamma 上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型，当前顺序，结果位置，上一个点击位置*/
        double calc_seq_prob(double prob,Session &sess,bool upd)
        {
            memset(forward,0,sizeof(forward));
            memset(backward,0,sizeof(backward));
            forward[0][0]=1;
            backward[DOCPERPAGE+1][0]=backward[DOCPERPAGE+1][1]=1;
            static int last_clk[DOCPERPAGE+2],last_ver=0;
            memset(last_clk,0,sizeof(last_clk));
            double ret=prob,exam;
            last_clk[0]=last_clk[1]=0;
            for(int i=1;i<=DOCPERPAGE;++i)
                if(sess.click_time[pos_in_sess[i]]>.1)
                {
                    exam=forward[i-1][0]/(forward[i-1][0]+forward[i-1][1]);
                    click_rate[pos_in_sess[i]]+=prob*exam*getgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]})*alpha[sess.doc_id[pos_in_sess[i]]];
                    forward[i][0]=forward[i-1][0]*getgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]})*alpha[sess.doc_id[pos_in_sess[i]]]*(1.-s_c[sess.doc_id[pos_in_sess[i]]]);
                    forward[i][1]=forward[i-1][0]*getgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]})*alpha[sess.doc_id[pos_in_sess[i]]]*s_c[sess.doc_id[pos_in_sess[i]]];
                    ret*=exam*getgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]})*alpha[sess.doc_id[pos_in_sess[i]]];
                    //exam=(1.-s_c[sess.doc_id[pos_in_sess[i]]]);
                    last_clk[i+1]=i;
                }
                else
                {
                    exam=forward[i-1][0]/(forward[i-1][0]+forward[i-1][1]);
                    click_rate[pos_in_sess[i]]+=prob*(1.-exam*getgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]})*alpha[sess.doc_id[pos_in_sess[i]]]);
                    forward[i][0]=forward[i-1][0]*(1.-getgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]})*alpha[sess.doc_id[pos_in_sess[i]]]);
                    forward[i][1]=forward[i-1][1];
                    ret*=(1.-exam*getgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]})*alpha[sess.doc_id[pos_in_sess[i]]]);
                    last_clk[i+1]=last_clk[i];
                }
            for(int i=DOCPERPAGE;i;--i)
                if(sess.click_time[pos_in_sess[i]]>.1)
                {
                    backward[i][1]=0;
                    backward[i][0]=getgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]})*alpha[sess.doc_id[pos_in_sess[i]]] * (backward[i+1][0]*(1.-s_c[sess.doc_id[pos_in_sess[i]]]) + backward[i+1][1]*s_c[sess.doc_id[pos_in_sess[i]]]);
                }
                else
                {
                    backward[i][1]=backward[i+1][1];
                    backward[i][0]=backward[i+1][0]*(1.-getgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]})*alpha[sess.doc_id[pos_in_sess[i]]]);
                }
            if(!upd)
                return ret;
            if(ret<eps)
                return ret;
            /*if(sess.id==1)
            {
                cerr<<"bb"<<prob<<"\t"<<ret<<"\t"<<tot_p<<endl;
                for(int i=1;i<=DOCPERPAGE;++i)
                    cerr<<pos_in_sess[i]<<"\t";cerr<<endl;
                for(int i=1;i<=DOCPERPAGE;++i)
                    cerr<<click_rate[3]<<"\t"<<pos_in_sess[3]<<"\t"<<st[i]<<"\t"<<en[i]<<"\t"<<docs[sess.doc_id[en[i]]].type<<"\t"<<way[i]<<"\t"<<exam*getgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]})*alpha[sess.doc_id[pos_in_sess[i]]]<<"\n";
                cerr<<"ee"<<endl;
            }*/
            for(int i=0;i<dphi_id.size();++i)
                cphi[dphi_id[i]]+=ret/tot_p*dphi[i];
            for(int i=0;i<dsigma_id.size();++i)
                csigma[dsigma_id[i]]+=ret/tot_p*dsigma[i];
            for(int i=1;i<=DOCPERPAGE;++i)
                if(sess.click_time[pos_in_sess[i]]>.1)
                {
                    exam=forward[i-1][0]/(forward[i-1][0]+forward[i-1][1]);
                    //click_rate[pos_in_sess[i]]+=prob*exam*getgamma((st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i))*alpha[sess.doc_id[pos_in_sess[i]]];
                    //ret*=exam*getgamma((st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i))*alpha[sess.doc_id[pos_in_sess[i]]];
                    addcgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]},ret/tot_p/getgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]}));
                    calpha[sess.doc_id[pos_in_sess[i]]]+=ret/tot_p/alpha[sess.doc_id[pos_in_sess[i]]];
                    cs_c[sess.doc_id[pos_in_sess[i]]]+=ret/tot_p/(backward[i+1][0]*(1.-s_c[sess.doc_id[pos_in_sess[i]]]) + backward[i+1][1]*s_c[sess.doc_id[pos_in_sess[i]]]) * (backward[i+1][1]-backward[i+1][0]);
                }
                else
                {
                    exam=forward[i-1][0]/(forward[i-1][0]+forward[i-1][1]);
                    //click_rate[pos_in_sess[i]]+=exam*(1.-getgamma((st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i))*alpha[sess.doc_id[pos_in_sess[i]]]);
                    addcgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]},-alpha[sess.doc_id[pos_in_sess[i]]]/tot_p*prob*(forward[i-1][0]*backward[i+1][0]));
                    calpha[sess.doc_id[pos_in_sess[i]]]+=-getgamma({st[i],en[i],docs[sess.doc_id[en[i]]].type,way[i],i,last_clk[i]})/tot_p*prob*(forward[i-1][0]*backward[i+1][0]);
                }
        }
        //UPD:cphi+=dphi*p_now/tot_p
        double dfs(int now,double prob,Session &sess,bool upd=0)
        {
            /*/Only first one
            if(now>1)
            {
                for(int i=ver_pos[1]+1;i<=DOCPERPAGE;++i)
                {
                    st[i]=ver_pos[1];
                    en[i]=DOCPERPAGE+1;
                    pos_in_sess[i]=i;
                    way[i]=0;
                }
                return calc_seq_prob(prob,sess,upd);
            }
            //Only first one*/
            if(now==ver_pos.size())
            {
                for(int i=ver_pos[now-1]+1;i<=DOCPERPAGE;++i)
                {
                    pos_in_sess[i]=i;
                    way[i]=0;
                }
                return calc_seq_prob(prob,sess,upd);
            }
            if(ver_pos[now]-ver_pos[now-1]==1)
            {
                pos_in_sess[ver_pos[now]]=ver_pos[now];
                way[ver_pos[now]]=0;
                return dfs(now+1,prob,sess,upd);
            }
            double ret=0;
            if(ver_pos[now]-ver_pos[now-1]==2)
            {
                for(int i=ver_pos[now-1]+1;i<=ver_pos[now];++i)
                {
                    pos_in_sess[i]=i;
                    way[i]=0;
                }
                dphi_id.push_back(phiid({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type}));
                dphi.push_back(-1./(1.-getphi({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type})));//
                ret+=dfs(now+1,prob*(1.-getphi({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type})),sess,upd);
                dphi_id.pop_back();
                dphi.pop_back();
                for(int i=ver_pos[now-1]+1;i<=ver_pos[now];++i)
                {
                    pos_in_sess[i]=ver_pos[now-1]+ver_pos[now]-i+1;
                    way[i]=1;
                }
                dphi_id.push_back(1./phiid({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type}));//
                dphi.push_back(1);
                ret+=dfs(now+1,prob*getphi({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type}),sess,upd);
                dphi_id.pop_back();
                dphi.pop_back();
                return ret;
            }
            for(int i=ver_pos[now-1]+1;i<=ver_pos[now];++i)
            {
                pos_in_sess[i]=i;
                way[i]=0;
            }
            dphi_id.push_back(phiid({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type}));
            dphi.push_back(-1./(1.-getphi({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type})));//
            ret+=dfs(now+1,prob*(1.-getphi({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type})),sess,upd);
            dphi_id.pop_back();
            dphi.pop_back();
            for(int i=ver_pos[now-1]+1;i<=ver_pos[now];++i)
            {
                pos_in_sess[i]=ver_pos[now-1]+ver_pos[now]-i+1;
                way[i]=1;
            }
            dphi_id.push_back(phiid({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type}));
            dphi.push_back(1./getphi({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type}));//
            dsigma_id.push_back(sigmaid({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type}));
            dsigma.push_back(1./getsigma({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type}));//
            ret+=dfs(now+1,prob*getphi({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type})*getsigma({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type}),sess,upd);
            dphi_id.pop_back();
            dphi.pop_back();
            dsigma_id.pop_back();
            dsigma.pop_back();
            way[ver_pos[now-1]+1]=2;
            pos_in_sess[ver_pos[now-1]+1]=ver_pos[now];
            for(int i=ver_pos[now-1]+2;i<=ver_pos[now];++i)
            {
                pos_in_sess[i]=i-1;
                way[i]=2;
            }
            dphi_id.push_back(phiid({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type}));
            dphi.push_back(getphi({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type}));
            dsigma_id.push_back(sigmaid({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type}));
            dsigma.push_back(-1./(1.-getsigma({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type})));
            ret+=dfs(now+1,prob*getphi({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type})*(1.-getsigma({ver_pos[now-1],ver_pos[now],docs[sess.doc_id[ver_pos[now]]].type})),sess,upd);
            dphi_id.pop_back();
            dphi.pop_back();
            dsigma_id.pop_back();
            dsigma.pop_back();
            return ret;
        }
        void sess_prework(Session &sess)
        {
            ver_pos.clear();
            ver_pos.push_back(0);
            dphi.clear();
            dphi_id.clear();
            dsigma.clear();
            dsigma_id.clear();
            for(int i=1;i<=DOCPERPAGE;++i)
                if(is_vertical(sess.doc_id[i]))
                {
                    ver_pos.push_back(i);
                    for(int j=ver_pos[ver_pos.size()-2]+1;j<=ver_pos[ver_pos.size()-1];++j)
                    {
                        st[j]=ver_pos[ver_pos.size()-2];
                        en[j]=ver_pos[ver_pos.size()-1];
                    }
                }
            for(int j=ver_pos[ver_pos.size()-1]+1;j<=DOCPERPAGE;++j)
            {
                st[j]=ver_pos[ver_pos.size()-1];
                en[j]=DOCPERPAGE+1;
            }
            memset(click_rate,0,sizeof(click_rate));
        }
        double tot_p;
        void calc_prob(Session &sess,int upd)
        {
            sess_prework(sess);
            /*if(sess.id==1)
                cerr<<"BEGIN"<<click_rate[3]<<endl;/**/
            tot_p=dfs(1,1.0,sess);
            /*if(sess.id==1)
            for(int i=1;i<=DOCPERPAGE;++i)
                cerr<<click_rate[i]<<"\t";
            if(sess.id==1)
            cerr<<"ENDDD"<<endl;/**/
            if(upd==-1)
                return;
            memset(click_rate,0,sizeof(click_rate));
            dfs(1,1.0,sess,1);
        }
        void train()
        {
            double last_ll=-100,now_ll;
            int last_clk,sess_cnt=0;
            train_init();
            for(int round=1;round<=MAXROUND;++round)
            {
                train_clear();
                double maxdlt=0;
                for(int id=0;id<sessions.size();++id)
                {
                    auto &sess=sessions[id];
                    if(sess.enable==false||!istrain(sess,sess_cnt))//在没有Overfit可能性的模型里应写作istest，在需要验证集的模型里应写作!istrain
                        continue;
                    ++sess_cnt;
                    calc_prob(sess,1);
                }
                train_update();
                //now_ll=this->test(false,1);
                cerr<<"Round:\t"<<round<<"\tLL:\t"<<"\t"<<this->test(false,2)<<"\t"<<dlt<<endl;
                last_ll=now_ll;
                dlt*=ddlt;
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
            /*for(v=1;v<=DOCPERPAGE;++v)
                if(docs[sess.doc_id[v]].type>1)
                    break;
            if(v>DOCPERPAGE)
                v=0;*/
            calc_prob(sess,-1);
            for(int i=0;i<=DOCPERPAGE;++i)
                click_prob[i]=click_rate[i];
		}
};
#endif
