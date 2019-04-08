#ifndef VCM_H
#define VCM_H
//#define double long double
class vcm:public model
{
    public:
        double gamma[DOCPERPAGE+2][DOCPERPAGE+2],cgamma[DOCPERPAGE+2][DOCPERPAGE+2];
        vector<vector<double>> phi,sigma,cphi,csigma;
        vector<double> alpha,beta,cita,calpha,cbeta,ccita;
        vector<int> first_vertical;
        vector<int> doc_cnt;
        double protect[10];//={1e-4,0.2,0.1,0.07,0.04};
        double delta=0.1,dlt=0.9,eps=1e-4;
        void init()
        {
            int i,j;
            protect[0]=eps;
            for(i=1;i<10;++i)
                protect[i]=0.5/(1.+i);
            for(i=0;i<=docs.size();++i)
            {
                alpha.push_back(0.5);
                beta.push_back(0.);
                cita.push_back(0.);
                calpha.push_back(0.);
                cbeta.push_back(0.);
                ccita.push_back(0.);
                doc_cnt.push_back(0);
            }
            for(i=0;i<=sessions.size();++i)
            {
                first_vertical.push_back(0);
                for(j=1;j<=DOCPERPAGE;++j)
                    if(docs[sessions[i].doc_id[j]].type>1)
                    {
                        first_vertical[i]=j;
                        break;
                    }
                auto &sess=sessions[i];
                if(sess.enable==false)
                    continue;
                if(!istrain(sess))
                    continue;
                for(j=1;j<=DOCPERPAGE;++j)
                    ++doc_cnt[sess.doc_id[j]];
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
            for(i=0;i<=DOCPERPAGE;++i)
                for(j=0;j<=DOCPERPAGE;++j)
                {
                    cgamma[i][j]=0.;
                    gamma[i][j]=0.5;
                }
        }
        void clear()
        {
            int i,j;
            static const double pr=1;
            for(i=0;i<=docs.size();++i)
            {
                calpha[i]=pr*(1./alpha[i]-1./(1.-alpha[i]));
                cbeta[i]=pr*(1./((beta[i]+1)/2)-1./(1.-(beta[i]+1)/2));
                ccita[i]=pr*(1./((cita[i]+1)/2)-1./(1.-(cita[i]+1)/2));
            }
            for(i=0;i<=DOCPERPAGE;++i)
            {
                for(j=0;j<=MAXVERTICLE;++j)
                {
                    cphi[i][j]=pr*(1./phi[i][j]-1./(1.-phi[i][j]));
                    csigma[i][j]=pr*(1./sigma[i][j]-1./(1.-sigma[i][j]));
                }
            }
            for(i=0;i<=DOCPERPAGE;++i)
                for(j=0;j<=DOCPERPAGE;++j)
                    cgamma[i][j]=pr*(1./gamma[i][j]-1./(1.-gamma[i][j]));
        }
        void update_num(double &p,double add,double prc=1e-6,double lb=0.,double hb=1.)
        {
            if(add>eps)
            {
                p+=delta;
                if(p>hb-prc)
                    p=hb-prc;
            }
            if(add<-eps)
            {
                p-=delta;
                if(p<lb+prc)
                    p=lb+prc;
            }
            //cout<<p<<"\t"<<add<<endl;
            //assert(p>eps/10&&p<1.-eps/10);
        }
        void update(int rnd)
        {
            //if(rnd%2==1)
            {
                for(int i=0;i<=docs.size();++i)
                {
                    update_num(alpha[i],calpha[i]);
                    update_num(beta[i],cbeta[i],-1,1);
                    update_num(cita[i],ccita[i],-1,1);
                }
                for(int i=0;i<=DOCPERPAGE;++i)
                {
                    for(int j=0;j<=DOCPERPAGE;++j)
                    {
                        update_num(gamma[i][j],cgamma[i][j]);
                    }
                }
                //return;
            }
            for(int i=0;i<=DOCPERPAGE;++i)
            {
                for(int j=0;j<=MAXVERTICLE;++j)
                {
                    update_num(phi[i][j],cphi[i][j]);
                    //cerr<<phi[i][j]<<"\t";
                    update_num(sigma[i][j],csigma[i][j]);
                   // cerr<<sigma[i][j]<<"\t";
                }
                //cerr<<endl;
            }
        }
        void get_3prob(Session &sess,double &p1,double &p2,double &p3,int v)
        {
            int last_clk,num;
            //p1:F1,B1  p2:F1,B0    p3:F0
            last_clk=0;
            num=1;
            for(int i=v;i;--i)
            {
                if(sess.click_time[i]>.1)
                {
                    p1*=rge((alpha[sess.doc_id[i]]+beta[sess.doc_id[i]]))*rge((gamma[num][num-last_clk]+cita[sess.doc_id[i]]));
                    last_clk=num;
                }
                else
                {
                    p1*=1.-rge((alpha[sess.doc_id[i]]+beta[sess.doc_id[i]]))*rge((gamma[num][num-last_clk]+cita[sess.doc_id[i]]));
                }
                ++num;
            }
            for(int i=v+1;i<=DOCPERPAGE;++i)
            {
                if(sess.click_time[i]>.1)
                {
                    p1*=rge((alpha[sess.doc_id[i]]+beta[sess.doc_id[i]]))*rge((gamma[num][num-last_clk]+cita[sess.doc_id[i]]));
                    last_clk=num;
                }
                else
                {
                    p1*=1.-rge((alpha[sess.doc_id[i]]+beta[sess.doc_id[i]]))*rge((gamma[num][num-last_clk]+cita[sess.doc_id[i]]));
                }
                ++num;
            }

            last_clk=0;
            num=1;
            if(sess.click_time[v]>.1)
            {
                p2*=rge((alpha[sess.doc_id[v]]+beta[sess.doc_id[v]]))*rge((gamma[num][num-last_clk]+cita[sess.doc_id[v]]));
                last_clk=num;
            }
            else
            {
                p2*=1.-rge((alpha[sess.doc_id[v]]+beta[sess.doc_id[v]]))*rge((gamma[num][num-last_clk]+cita[sess.doc_id[v]]));
            }
            ++num;
            for(int i=1;i<=DOCPERPAGE;++i)
                if(i!=v)
                {
                    if(sess.click_time[i]>.1)
                    {
                        p2*=rge((alpha[sess.doc_id[i]]+beta[sess.doc_id[i]]))*rge((gamma[num][num-last_clk]+cita[sess.doc_id[i]]));
                        last_clk=num;
                    }
                    else
                    {
                        p2*=1.-rge((alpha[sess.doc_id[i]]+beta[sess.doc_id[i]]))*rge((gamma[num][num-last_clk]+cita[sess.doc_id[i]]));
                    }
                    ++num;
                }
            
            last_clk=0;
            num=1;
            if(sess.click_time[v]>.1)
            {
                p3*=alpha[sess.doc_id[v]]*gamma[num][num-last_clk];
                last_clk=num;
            }
            else
            {
                p3*=1.-alpha[sess.doc_id[v]]*gamma[num][num-last_clk];
            }
            ++num;
            for(int i=1;i<=DOCPERPAGE;++i)
                if(i!=v)
                {
                    if(sess.click_time[i]>.1)
                    {
                        p3*=alpha[sess.doc_id[i]]*gamma[num][num-last_clk];
                        last_clk=num;
                    }
                    else
                    {
                        p3*=1.-alpha[sess.doc_id[i]]*gamma[num][num-last_clk];
                    }
                    ++num;
                }
        }
        void calc(Session &sess,double &p,double &ptot,int &i,int &last_clk,int &num)
        {
            static double alp,bet,gam,cit;
            alp=alpha[sess.doc_id[i]];
            bet=beta[sess.doc_id[i]];
            gam=gamma[num][num-last_clk];
            cit=cita[sess.doc_id[i]];
            if(sess.click_time[i]>.1)
            {
                if(alp+bet<1.-2*eps&&alp+bet>2*eps)
                {
                    calpha[sess.doc_id[i]]+=p/rge(alp+bet)/ptot;
                    cbeta[sess.doc_id[i]]+=p/rge(alp+bet)/ptot;
                }
                if(gam+cit<1.-2*eps&&gam+cit>2*eps)
                {
                    cgamma[num][num-last_clk]+=p/rge(gam+cit)/ptot;
                    ccita[sess.doc_id[i]]+=p/rge(gam+cit)/ptot;
                }
                last_clk=num;
            }
            else
            {
                if(alp+bet<1.-2*eps&&alp+bet>2*eps)
                {
                    calpha[sess.doc_id[i]]-=p/(1.-rge(alp+bet)*rge(gam+cit))*rge(gam+cit)/ptot;
                    cbeta[sess.doc_id[i]]-=p/(1.-rge(alp+bet)*rge(gam+cit))*rge(gam+cit)/ptot;
                }
                if(gam+cit<1.-2*eps&&gam+cit>2*eps)
                {
                    cgamma[num][num-last_clk]-=p/(1.-rge(alp+bet)*rge(gam+cit))*rge(alp+bet)/ptot;
                    ccita[sess.doc_id[i]]-=p/(1.-rge(alp+bet)*rge(gam+cit))*rge(alp+bet)/ptot;
                }
            }
            ++num;
        }
        void calc2(Session &sess,double &p,double &ptot,int &i,int &last_clk,int &num)
        {
            static double alp,gam;
            alp=alpha[sess.doc_id[i]];
            gam=gamma[num][num-last_clk];
            if(sess.click_time[i]>.1)
            {
                calpha[sess.doc_id[i]]+=p/(alp)/ptot;
                cgamma[num][num-last_clk]+=p/gam/ptot;
                last_clk=num;
            }
            else
            {
                calpha[sess.doc_id[i]]-=p/(1.-alp*gam)*(gam)/ptot;
                cgamma[num][num-last_clk]-=p/(1.-(alp)*(gam))*(alp)/ptot;
            }
            ++num;
        }
        double rge(const double x)
        {
            if(x<eps)
                return eps;
            if(x>1.-eps)
                return 1.-eps;
            return x;
        }
        void train()
        {
            name="Vcm";
            doc_rel=vector<double>(docs.size()+1);
            init();
            int rnd;
            double last_LL=-100,now_LL;
            for(rnd=1;rnd<=MAXROUND;++rnd)
            {
                clear();
                for(int idx=0;idx<sessions.size();++idx)
                {
                    auto &sess=sessions[idx];
                    if(sess.enable==false)
                        continue;
                    if(!istrain(sess))
                        continue;
                    int &ver=first_vertical[idx];
                    int last_clk=0;
                    if(first_vertical[idx]==0)
                    {
                        for(int i=1;i<=DOCPERPAGE;++i)
                            if(sess.click_time[i]>.1)
                            {
                                calpha[sess.doc_id[i]]+=1./alpha[sess.doc_id[i]];
                                cgamma[i][i-last_clk]+=1./gamma[i][i-last_clk];
                                last_clk=i;
                            }
                            else
                            {
                                calpha[sess.doc_id[i]]-=gamma[i][i-last_clk]/(1.-alpha[sess.doc_id[i]]*gamma[i][i-last_clk]);
                                cgamma[i][i-last_clk]-=alpha[sess.doc_id[i]]/(1.-alpha[sess.doc_id[i]]*gamma[i][i-last_clk]);
                            }
                    }
                    else
                    {
                        int &v=first_vertical[idx];
                        double ptot,p1=1.,p2=1.,p3=1.;
                        int last_clk,num;
                        //p1:F1,B1  p2:F1,B0    p3:F0
                        get_3prob(sess,p1,p2,p3,v);
                        p1*=phi[v][docs[sess.doc_id[v]].type]*sigma[v][docs[sess.doc_id[v]].type];
                        p2*=phi[v][docs[sess.doc_id[v]].type]*(1.-sigma[v][docs[sess.doc_id[v]].type]);
                        p3*=1.-phi[v][docs[sess.doc_id[v]].type];
                        /*if(rnd==10)
                            cerr<<p1<<'\t'<<p2<<'\t'<<p3<<'\t'<<phi[v][docs[sess.doc_id[v]].type]<<'\t'<<sigma[v][docs[sess.doc_id[v]].type]<<endl;*/
                        ptot=p1+p2+p3;
                        //if(rnd%2==0)
                        {
                            cphi[v][docs[sess.doc_id[v]].type]+=((p1+p2)/phi[v][docs[sess.doc_id[v]].type])/ptot-p3/(1.-phi[v][docs[sess.doc_id[v]].type])/ptot;
                            csigma[v][docs[sess.doc_id[v]].type]+=p1/sigma[v][docs[sess.doc_id[v]].type]/ptot-p2/(1.-sigma[v][docs[sess.doc_id[v]].type])/ptot;
                            //continue;
                        }
                        //F1,B1
                        last_clk=0;
                        num=1;
                        for(int i=v;i;--i)
                            calc(sess,p1,ptot,i,last_clk,num);
                        for(int i=v+1;i<=DOCPERPAGE;++i)
                            calc(sess,p1,ptot,i,last_clk,num);
                        //F1,B0
                        last_clk=0;
                        num=1;
                        calc(sess,p2,ptot,v,last_clk,num);
                        for(int i=1;i<=DOCPERPAGE;++i)
                            if(i^v)
                                calc(sess,p2,ptot,i,last_clk,num);
                        //F0
                        last_clk=0;
                        num=1;
                        calc2(sess,p3,ptot,v,last_clk,num);
                        for(int i=1;i<=DOCPERPAGE;++i)
                            if(i^v)
                                calc2(sess,p2,ptot,i,last_clk,num);
                    }
                }
                update(rnd);
                delta*=dlt;
		if(rnd%20==0)
		{
                	now_LL=this->test(false,2);
                	cerr<<"Round = "<<rnd<<"\tTest = "<<now_LL<<"\tDelta = "<<delta<<"\t"<<gamma[4][2]<<"\t"<<sigma[2][4]<<endl;
                }
		//now_LL2=this->test(false,2);
                //now_LL1=this->test(false,1);
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
            FILE* outfile=fopen("../output/vcm_args","w");
            assert(outfile);
            fclose(outfile);
            /*for(int i=1;i<=DOCPERPAGE;++i)
            {
                for(int j=1;j<=MAXVERTICLE;++j)
                    cerr<<phi[i][j]<<"\t";
                cerr<<endl;
            }*/
        }
        void load()
        {
            FILE* infile=fopen("../output/vcm_args","r");
            assert(infile);
            fclose(infile);
        }
        void get_click_prob(Session &sess,double* click_prob)
        {
            //click_prob[i]=P(C_1,C_2...C_i) X
            //click_prob[i]=P(C_i|C_1,C_2,...C_{i-1})
            double c1,c2,c3;
            int v,last_clkk=0,num,i;
            for(v=1;v<=DOCPERPAGE;++v)
                if(docs[sess.doc_id[v]].type>1)
                    break;
            if(v>DOCPERPAGE)
            {
                for(int i=1;i<=DOCPERPAGE;++i)
                {
                    if(sess.click_time[i]>.1)
                    {
                        click_prob[i]=alpha[sess.doc_id[i]]*gamma[i][i-last_clkk];
                        last_clkk=i;
                    }
                    else
                    {
                        click_prob[i]=1.-alpha[sess.doc_id[i]]*gamma[i][i-last_clkk];
                    }
                }
                return;
            }
            num=1;
            last_clkk=0;
            c1=phi[v][docs[sess.doc_id[v]].type]*sigma[v][docs[sess.doc_id[v]].type];
            for(i=v;i;--i)
            {
                if(sess.click_time[i]>.1)
                {
                    click_prob[i]=c1*(rge(alpha[sess.doc_id[i]]+beta[sess.doc_id[i]])*rge(gamma[num][num-last_clkk]+cita[sess.doc_id[i]]));
                    last_clkk=num;
                }
                else
                {
                    click_prob[i]=c1*(1.-rge(alpha[sess.doc_id[i]]+beta[sess.doc_id[i]])*rge(gamma[num][num-last_clkk]+cita[sess.doc_id[i]]));
                }
                ++num;
            }
            for(i=v+1;i<=DOCPERPAGE;++i)
            {
                if(sess.click_time[i]>.1)
                {
                    click_prob[i]=c1*(rge(alpha[sess.doc_id[i]]+beta[sess.doc_id[i]])*rge(gamma[num][num-last_clkk]+cita[sess.doc_id[i]]));
                    last_clkk=num;
                }
                else
                {
                    click_prob[i]=c1*(1.-rge(alpha[sess.doc_id[i]]+beta[sess.doc_id[i]])*rge(gamma[num][num-last_clkk]+cita[sess.doc_id[i]]));
                }
                ++num;
            }
            num=1;
            last_clkk=0;
            c2=phi[v][docs[sess.doc_id[v]].type]*(1.-sigma[v][docs[sess.doc_id[v]].type]);
            i=v;
            if(sess.click_time[i]>.1)
            {
                click_prob[i]+=c2*(rge(alpha[sess.doc_id[i]]+beta[sess.doc_id[i]])*rge(gamma[num][num-last_clkk]+cita[sess.doc_id[i]]));
                last_clkk=num;
            }
            else
            {
                click_prob[i]+=c2*(1.-rge(alpha[sess.doc_id[i]]+beta[sess.doc_id[i]])*rge(gamma[num][num-last_clkk]+cita[sess.doc_id[i]]));
            }
            ++num;
            for(i=1;i<=DOCPERPAGE;++i)
                if(i^v)
                {
                    if(sess.click_time[i]>.1)
                    {
                        click_prob[i]+=c2*(rge(alpha[sess.doc_id[i]]+beta[sess.doc_id[i]])*rge(gamma[num][num-last_clkk]+cita[sess.doc_id[i]]));
                        last_clkk=num;
                    }
                    else
                    {
                        click_prob[i]+=c2*(1.-rge(alpha[sess.doc_id[i]]+beta[sess.doc_id[i]])*rge(gamma[num][num-last_clkk]+cita[sess.doc_id[i]]));
                    }
                    ++num;
                }
            num=1;
            last_clkk=0;
            c3=1.-phi[v][docs[sess.doc_id[v]].type];
            i=v;
            if(sess.click_time[i]>.1)
            {
                click_prob[i]+=c3*((alpha[sess.doc_id[i]])*(gamma[num][num-last_clkk]));
                last_clkk=num;
            }
            else
            {
                click_prob[i]+=c3*(1.-(alpha[sess.doc_id[i]])*(gamma[num][num-last_clkk]));
            }
            ++num;
            for(i=1;i<=DOCPERPAGE;++i)
                if(i^v)
                {
                    if(sess.click_time[i]>.1)
                    {
                        click_prob[i]+=c3*((alpha[sess.doc_id[i]])*(gamma[num][num-last_clkk]));
                        last_clkk=num;
                    }
                    else
                    {
                        click_prob[i]+=c3*(1.-(alpha[sess.doc_id[i]])*(gamma[num][num-last_clkk]));
                    }
                    ++num;
                }
            //examination = P(E_k|C_1,C_2,...C_{k-1})
            /*click_prob[0]=1.0;
            int last_clkk=0;
            */
        }
};
#endif
