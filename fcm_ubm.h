#ifndef FCM_UBM_H
#define FCM_UBM_H
//#define double long double
class fcm_ubm:public model
{
    public:
        double gamma[DOCPERPAGE+2][DOCPERPAGE+2],gamma0[DOCPERPAGE+2][DOCPERPAGE+2],gamma1[DOCPERPAGE+2][DOCPERPAGE+2];
        double beta[DOCPERPAGE*2+4],beta0[DOCPERPAGE*2+4],beta1[DOCPERPAGE*2+4];
        vector<double> yita[DOCPERPAGE+2],AP[DOCPERPAGE+2],yita0[DOCPERPAGE+2],yita1[DOCPERPAGE+2],AP0[DOCPERPAGE+2],AP1[DOCPERPAGE+2];
        vector<double> alpha,alpha0,alpha1;
        vector<int> mindiss;
        double get_rel(int id)
        {
            return alpha[id];
        }
        void init()
        {
            alpha.resize(docs.size()+2);
            alpha0.resize(docs.size()+2);
            alpha1.resize(docs.size()+2);
            for(int i=0;i<DOCPERPAGE+2;++i)
            {
                yita[i].resize(MAXVERTICLE+2);
                yita0[i].resize(MAXVERTICLE+2);
                yita1[i].resize(MAXVERTICLE+2);
                AP[i].resize(MAXVERTICLE+2);
                AP0[i].resize(MAXVERTICLE+2);
                AP1[i].resize(MAXVERTICLE+2);
                for(int j=0;j<=MAXVERTICLE;++j)
                {
                    yita[i][j]=0.5;
                    AP[i][j]=0.5;
                }
            }
            for(int i=0;i<docs.size();++i)
                alpha[i]=0.5;
            for(int i=-DOCPERPAGE;i<=DOCPERPAGE;++i)
                beta[DOCPERPAGE+i]=0.5;
            for(int i=0;i<=DOCPERPAGE;++i)
                for(int j=0;j<=DOCPERPAGE;++j)
                    gamma[i][j]=0.5;
        }
        void clear()
        {
            for(int i=0;i<=DOCPERPAGE;++i)
            {
                for(int j=1;j<=DOCPERPAGE;++j)
                {
                    gamma0[i][j]=0.5;
                    gamma1[i][j]=1;
                }
                for(int j=0;j<=MAXVERTICLE;++j)
                {
                    yita0[i][j]=0.05;
                    yita1[i][j]=0.1;
                    AP0[i][j]=0.05;
                    AP1[i][j]=0.1;
                }
            }
            for(int i=-DOCPERPAGE;i<=DOCPERPAGE;++i)
            {
                beta0[DOCPERPAGE+i]=0.1;
                beta1[DOCPERPAGE+i]=0.5;
            }
            for(int i=0;i<docs.size();++i)
            {
                alpha0[i]=0.5;
                alpha1[i]=1;
            }
        }
        void update()
        {
            //cerr<<"==========gamma=============\n";
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                for(int j=1;j<=DOCPERPAGE;++j)
                {
                    gamma[i][j]=gamma0[i][j]/gamma1[i][j];
                    //cerr<<gamma[i][j]<<"\t";
                    //assert(0<=gamma[i][j]&&gamma[i][j]<=1);
                }
                //cerr<<endl;
            }
            //cerr<<"==========gamma end=========\n";
            //cerr<<"==========yita and ap=======\n";
            for(int i=0;i<=DOCPERPAGE;++i)
            {
                for(int j=0;j<=MAXVERTICLE;++j)
                {
                    yita[i][j]=yita0[i][j]/yita1[i][j];
                    //assert(0<=yita[i][j]&&yita[i][j]<=1);
                    AP[i][j]=AP0[i][j]/AP1[i][j];
                    //cerr<<AP[i][j]<<"\t";
                    //assert(0<=AP[i][j]&&AP[i][j]<=1);
                }
                //cerr<<endl;
            }
            //cerr<<"======end yita and ap=======\n";
            for(int i=-DOCPERPAGE;i<=DOCPERPAGE;++i)
            {
                beta[DOCPERPAGE+i]=beta0[DOCPERPAGE+i]/beta1[DOCPERPAGE+i];
                //cerr<<beta[DOCPERPAGE+i]<<endl;
                //assert(0<=beta[i]&&beta[i]<=1);
            }
            for(int i=0;i<docs.size();++i)
            {
                alpha[i]=alpha0[i]/alpha1[i];
                //assert(0<=alpha[i]&&alpha[i]<=1);
            }
        }
        void calc_e(Session &sess)
        {
            int la=0,lav=1;
            //yita's diss is mindiss
            //AP's v desided by last click
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                double alp,bet,gam,yit,app,C0,C1;
                alp=alpha[sess.doc_id[i]];
                if(mindiss[i])
                    bet=beta[DOCPERPAGE+i-mindiss[i]];
                else
                    bet=0;
                gam=gamma[i][i-la];
                if(mindiss[i])
                    yit=yita[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type];
                else
                    yit=0;
                app=AP[la][docs[sess.doc_id[i]].type];
                if(lav!=1)
                {
                    C1=alp*(yit*bet*(1.-gam)+gam);
                    C0=1.-C1;
                    if(sess.click_time[i]>.1)
                    {
                        alpha0[sess.doc_id[i]]+=1;
                        alpha1[sess.doc_id[i]]+=1;
                        if(mindiss[i])
                        {
                            beta0[DOCPERPAGE+i-mindiss[i]]+=bet*alp*(yit*(1.-gam)+gam) / C1;
                            beta1[DOCPERPAGE+i-mindiss[i]]+=1;
                            yita0[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type]+=yit*alp*(bet*(1.-gam)+gam) / C1;
                            yita1[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type]+=1;
                        }
                        gamma0[i][i-la]+=alp*gam / C1;
                        gamma1[i][i-la]+=1;
                    }
                    else
                    {
                        alpha0[sess.doc_id[i]]+=(1.-gam-(1-gam)*yit*bet) * alp / C0;
                        alpha1[sess.doc_id[i]]+=1;
                        if(mindiss[i])
                        {
                            beta0[DOCPERPAGE+i-mindiss[i]]+=bet*(1.-alp*(yit*(1.-gam)+gam)) / C0;
                            beta1[DOCPERPAGE+i-mindiss[i]]+=1;
                            yita0[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type]+=yit*(1.-alp*(bet*(1.-gam)+gam))/C0;
                            yita1[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type]+=1;
                        }
                        gamma0[i][i-la]+=(1.-alp)*gam / C0;
                        gamma1[i][i-la]+=1;
                    }
                }
                else
                {
                    C1=alp*(yit*bet*(1.-gam)+gam)*(1.-app);
                    C0=1.-C1;
                    if(sess.click_time[i]>.1)
                    {
                        alpha0[sess.doc_id[i]]+=1;
                        alpha1[sess.doc_id[i]]+=1;
                        if(mindiss[i])
                        {
                            beta0[DOCPERPAGE+i-mindiss[i]]+=bet*(1.-app)*alp*(yit*(1.-gam)+gam) / C1;
                            beta1[DOCPERPAGE+i-mindiss[i]]+=1;
                            yita0[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type]+=yit*(1.-app)*alp*(bet*(1.-gam)+gam)/C1;
                            yita1[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type]+=1;
                        }
                        gamma0[i][i-la]+=alp*(1.-app)*gam / C1;
                        gamma1[i][i-la]+=1;
                        //AP0[la][docs[sess.doc_id[i]].type]+=0;
                        AP1[la][docs[sess.doc_id[i]].type]+=1;
                    }
                    else
                    {
                        alpha0[sess.doc_id[i]]+=(1.-(1.-app)*(yit*bet*(1.-gam)+gam)) * alp / C0;
                        alpha1[sess.doc_id[i]]+=1;
                        if(mindiss[i])
                        {
                            beta0[DOCPERPAGE+i-mindiss[i]]+=bet*(1.-(1.-app)*alp*(yit*(1.-gam)+gam)) / C0;
                            beta1[DOCPERPAGE+i-mindiss[i]]+=1;
                            yita0[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type]+=yit*(1.-(1.-app)*alp*(bet*(1.-gam)+gam))/C0;
                            yita1[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type]+=1;
                        }
                        gamma0[i][i-la]+=(1.-alp*(1.-app))*gam / C0;
                        gamma1[i][i-la]+=1;
                        AP0[la][docs[sess.doc_id[i]].type]+=app / C0;
                        AP1[la][docs[sess.doc_id[i]].type]+=1;
                    }
                }
                if(sess.click_time[i]>.1)
                {
                    la=i;
                    lav=docs[sess.doc_id[i]].type;
                }
            }
        }
        void get_diss(Session &sess)
        {
            if(mindiss.size()<DOCPERPAGE)
                mindiss.resize(DOCPERPAGE+2);
            int la=0;
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                if(la!=0)
                    mindiss[i]=la;
                else
                    mindiss[i]=0;
                if(sess.click_time[i]>.1)
                    la=i;
            }
            la=0;
            for(int i=DOCPERPAGE;i;--i)
            {
                if(la!=0)
                {
                    if(mindiss[i]==0||abs(la-i)<abs(i-mindiss[i]))
                        mindiss[i]=la;
                }
                if(sess.click_time[i]>.1)
                    la=i;
            }
        }
        void train()
        {
            name="Fcm_Ubm";
            init();
            double last_LL=-100;
            for(int round=1;round<=MAXROUND;++round)
            {
                clear();
                for(auto &sess:sessions)
                {
                    if(sess.enable==false)
                        continue;
                    if(!istrain(sess))
                        continue;
                    get_diss(sess);
                    calc_e(sess);
                }
                update();
                double now_LL,L2;
                now_LL=this->test(false,3);
                //now_LL2=this->test(false,2);
                //now_LL1=this->test(false,1);
                if(now_LL-1e-8<last_LL)
                    break;
                cerr<<round<<"\tVali = "<<now_LL<<endl;
                if(round%10==0)
                {
                    L2=this->test(false,2);
                    cerr<<"Test = "<<L2<<endl;
                }
                //if(rnd%100==0)
                //    cout<<"UBM LL:=\t"<<rnd<<"\t"<<fixed<<setprecision(12)<<now_LL<<endl;
                //cout<<now_LL1<<","<<now_LL2<<","<<now_LL<<endl;
                last_LL=now_LL;
            }
            FILE* outfile=fopen("../output/fcm_ubm_args","w");
            assert(outfile);
            fclose(outfile);
        }
        void load()
        {
            FILE* infile=fopen("../output/fcm_ubm_args","r");
            
            fclose(infile);
        }
        void get_click_prob(Session &sess,double* click_prob)
        {
            //click_prob[i]=P(C_1,C_2...C_i) X
            //click_prob[i]=P(C_i|C_1,C_2,...C_{i-1})
            //examination = P(E_k|C_1,C_2,...C_{k-1})
            get_diss(sess);
            
            click_prob[0]=1.0;
            bool lav=false;
            int la=0;
            double cp;
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                if(!lav)
                {
                    if(mindiss[i])
                        cp=alpha[sess.doc_id[i]]*(yita[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type]*beta[DOCPERPAGE+i-mindiss[i]]*(1.-gamma[i][i-la])+gamma[i][i-la]);
                    else
                        cp=alpha[sess.doc_id[i]]*(gamma[i][i-la]);
                }
                else
                {
                    if(mindiss[i])
                        cp=(1.-AP[la][docs[sess.doc_id[i]].type])*alpha[sess.doc_id[i]]*(yita[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type]*beta[DOCPERPAGE+i-mindiss[i]]*(1.-gamma[i][i-la])+gamma[i][i-la]);
                    else
                        cp=alpha[sess.doc_id[i]]*gamma[i][i-la];
                }
                if(sess.click_time[i]>.1)
                {
                    la=i;
                    if(docs[sess.doc_id[i]].type!=1)
                        lav=true;
                    else
                        lav=false;
                }
                else
                {
                    cp=1.-cp;
                }
                click_prob[i]=cp;
            }
        }
};
#endif
