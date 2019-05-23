#ifndef FMVCM_SGD_H
#define FMVCM_SGD_H
//#define double long double
extern vector<bool> gammaenable;
extern vector<bool> phienable;
extern vector<bool> sigmaenable;
extern double pr;
class fmvcm:public model
{
    public:
        vector<double> alpha,s_c,calpha,cs_c;
        double dlt=0.2,ddlt=0.9,eps=1e-6;
        vector<int> first_vertical;
        double phi[DOCPERPAGE+2][MAXVERTICLE+2],cphi[DOCPERPAGE+2][MAXVERTICLE+2];
        double sigma[DOCPERPAGE+2][MAXVERTICLE+2],csigma[DOCPERPAGE+2][MAXVERTICLE+2];
        double gamma[DOCPERPAGE+2][MAXVERTICLE+2][4][DOCPERPAGE+2][DOCPERPAGE+2],cgamma[DOCPERPAGE+2][MAXVERTICLE+2][4][DOCPERPAGE+2][DOCPERPAGE+2];
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
            //argphi=vector<int>{DOCPERPAGE+2,DOCPERPAGE+2,MAXVERTICLE+2};/*上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型*/
            //argsigma=vector<int>{DOCPERPAGE+2,DOCPERPAGE+2,MAXVERTICLE+2};/*上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型*/
            //arggamma=vector<int>{DOCPERPAGE+2,DOCPERPAGE+2,MAXVERTICLE+2,4,DOCPERPAGE+2,DOCPERPAGE+2};/*上一个Vertical的位置（没有则记为0），当前Vertical的位置，当前Vertical类型，当前顺序，结果位置，上一个点击位置*/
            /*对于phi和sigma保留前2维
            对于gamma保留2、5、6
            */
            for(i=0;i<=DOCPERPAGE;++i)
                for(j=0;j<=MAXVERTICLE;++j)
                {
                    phi[i][j]=sigma[i][j]=0.5;
                    cphi[i][j]=csigma[i][j]=0;
                }
            for(i=0;i<=DOCPERPAGE;++i)
                for(j=0;j<=MAXVERTICLE;++j)
                    for(int k=0;k<4;++k)
                        for(int s=0;s<=DOCPERPAGE;++s)
                            for(int d=0;d<=DOCPERPAGE;++d)
                            {
                                gamma[i][j][k][s][d]=0.5;
                                cgamma[i][j][k][s][d]=0;
                            }
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
            for(int i=0;i<=DOCPERPAGE;++i)
                for(int j=0;j<=MAXVERTICLE;++j)
                {
                    cphi[i][j]=pr*(1./phi[i][j]-1./(1.-phi[i][j]));
                    csigma[i][j]=pr*(1./sigma[i][j]-1./(1.-sigma[i][j]));
                }
        }
        void update_val(double &arg,double &dt)
        {
            if(dt>eps)
                arg=min(1.-eps,arg+dt);
            else
            {
                arg=max(eps,arg-dt);
            }
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
            for(int i=0;i<=DOCPERPAGE;++i)
                for(int j=0;j<=MAXVERTICLE;++j)
                {
                    update_val(phi[i][j],cphi[i][j]);
                    update_val(sigma[i][j],csigma[i][j]);
                }
        }
        double click_rate[DOCPERPAGE+2],forward[DOCPERPAGE+2][2],backward[DOCPERPAGE+2][2];
        /*顺序0：1234，顺序1：4123，顺序2：4321*/
        int view_len[DOCPERPAGE+2];
        int pos_in_sess[DOCPERPAGE+2],st[DOCPERPAGE+2],en[DOCPERPAGE+2],way[DOCPERPAGE+2];
        vector<int> ver_pos;
        int dphi_id[DOCPERPAGE+2][2];
        double dphi[DOCPERPAGE+2],dsigma[DOCPERPAGE+2];
        int dphi_num=0;
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
            double ga[DOCPERPAGE+2],al[DOCPERPAGE+2],sc[DOCPERPAGE+2];
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                ga[i]=gamma[en[i]][docs[sess.doc_id[en[i]]].type][way[i]][i][last_clk[i]];
                al[i]=alpha[sess.doc_id[pos_in_sess[i]]];
                sc[i]=s_c[sess.doc_id[pos_in_sess[i]]];
                if(sess.click_time[pos_in_sess[i]]>.1)
                {
                    exam=forward[i-1][0]/(forward[i-1][0]+forward[i-1][1]);
                    click_rate[pos_in_sess[i]]+=prob*exam*ga[i]*al[i];
                    forward[i][0]=forward[i-1][0]*ga[i]*al[i]*(1.-sc[i]);
                    forward[i][1]=forward[i-1][0]*ga[i]*al[i]*sc[i];
                    ret*=exam*ga[i]*al[i];
                    last_clk[i+1]=i;
                }
                else
                {
                    exam=forward[i-1][0]/(forward[i-1][0]+forward[i-1][1]);
                    click_rate[pos_in_sess[i]]+=prob*(1.-exam*ga[i]*al[i]);
                    forward[i][0]=forward[i-1][0]*(1.-ga[i]*al[i]);
                    forward[i][1]=forward[i-1][1];
                    ret*=(1.-exam*ga[i]*al[i]);
                    last_clk[i+1]=last_clk[i];
                }
            }
            for(int i=DOCPERPAGE;i;--i)
                if(sess.click_time[pos_in_sess[i]]>.1)
                {
                    backward[i][1]=0;
                    backward[i][0]=ga[i]*al[i] * (backward[i+1][0]*(1.-sc[i]) + backward[i+1][1]*sc[i]);
                }
                else
                {
                    backward[i][1]=backward[i+1][1];
                    backward[i][0]=backward[i+1][0]*(1.-ga[i]*al[i]);
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
            for(int i=0;i<dphi_num;++i)
            {
                cphi[dphi_id[i][0]][dphi_id[i][1]]+=ret/tot_p*dphi[i];
                sigma[dphi_id[i][0]][dphi_id[i][1]]+=ret/tot_p*dsigma[i];
            }
            for(int i=1;i<=DOCPERPAGE;++i)
                if(sess.click_time[pos_in_sess[i]]>.1)
                {
                    exam=forward[i-1][0]/(forward[i-1][0]+forward[i-1][1]);
                    cgamma[en[i]][docs[sess.doc_id[en[i]]].type][way[i]][i][last_clk[i]]+=ret/tot_p/ga[i];
                    calpha[sess.doc_id[pos_in_sess[i]]]+=ret/tot_p/al[i];
                    cs_c[sess.doc_id[pos_in_sess[i]]]+=ret/tot_p/(backward[i+1][0]*(1.-sc[i]) + backward[i+1][1]*sc[i]) * (backward[i+1][1]-backward[i+1][0]);
                }
                else
                {
                    exam=forward[i-1][0]/(forward[i-1][0]+forward[i-1][1]);
                    cgamma[en[i]][docs[sess.doc_id[en[i]]].type][way[i]][i][last_clk[i]]+=-al[i]/tot_p*prob*(forward[i-1][0]*backward[i+1][0]);
                    calpha[sess.doc_id[pos_in_sess[i]]]+=-ga[i]/tot_p*prob*(forward[i-1][0]*backward[i+1][0]);
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
            double &ph=phi[ver_pos[now]][docs[sess.doc_id[ver_pos[now]]].type],&si=sigma[ver_pos[now]][docs[sess.doc_id[ver_pos[now]]].type];
            if(ver_pos[now]-ver_pos[now-1]==2)
            {
                for(int i=ver_pos[now-1]+1;i<=ver_pos[now];++i)
                {
                    pos_in_sess[i]=i;
                    way[i]=0;
                }
                dphi_id[dphi_num][0]=ver_pos[now];
                dphi_id[dphi_num][1]=docs[sess.doc_id[ver_pos[now]]].type;
                dphi[dphi_num]=-1./(1.-ph);
                dsigma[dphi_num]=0;
                ++dphi_num;
                ret+=dfs(now+1,prob*(1.-ph),sess,upd);
                --dphi_num;
                for(int i=ver_pos[now-1]+1;i<=ver_pos[now];++i)
                {
                    pos_in_sess[i]=ver_pos[now-1]+ver_pos[now]-i+1;
                    way[i]=1;
                }
                dphi_id[dphi_num][0]=ver_pos[now];
                dphi_id[dphi_num][1]=docs[sess.doc_id[ver_pos[now]]].type;
                dphi[dphi_num]=1./ph;
                dsigma[dphi_num]=0;
                ++dphi_num;
                ret+=dfs(now+1,prob*ph,sess,upd);
                --dphi_num;
                return ret;
            }
            for(int i=ver_pos[now-1]+1;i<=ver_pos[now];++i)
            {
                pos_in_sess[i]=i;
                way[i]=0;
            }
            dphi_id[dphi_num][0]=ver_pos[now];
            dphi_id[dphi_num][1]=docs[sess.doc_id[ver_pos[now]]].type;
            dphi[dphi_num]=-1./(1.-ph);
            dsigma[dphi_num]=0;
            ++dphi_num;
            ret+=dfs(now+1,prob*(1.-ph),sess,upd);
            --dphi_num;
            for(int i=ver_pos[now-1]+1;i<=ver_pos[now];++i)
            {
                pos_in_sess[i]=ver_pos[now-1]+ver_pos[now]-i+1;
                way[i]=1;
            }
            dphi_id[dphi_num][0]=ver_pos[now];
            dphi_id[dphi_num][1]=docs[sess.doc_id[ver_pos[now]]].type;
            dphi[dphi_num]=1./ph;//
            dsigma[dphi_num]=1./si;//
            ++dphi_num;
            ret+=dfs(now+1,prob*ph*si,sess,upd);
            --dphi_num;
            way[ver_pos[now-1]+1]=2;
            pos_in_sess[ver_pos[now-1]+1]=ver_pos[now];
            for(int i=ver_pos[now-1]+2;i<=ver_pos[now];++i)
            {
                pos_in_sess[i]=i-1;
                way[i]=2;
            }
            dphi_id[dphi_num][0]=ver_pos[now];
            dphi_id[dphi_num][1]=docs[sess.doc_id[ver_pos[now]]].type;
            dphi[dphi_num]=1./ph;//
            dsigma[dphi_num]=-1./(1.-si);//
            ++dphi_num;
            ret+=dfs(now+1,prob*ph*(1.-si),sess,upd);
            --dphi_num;
            return ret;
        }
        void sess_prework(Session &sess)
        {
            ver_pos.clear();
            ver_pos.push_back(0);
            dphi_num=0;
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
