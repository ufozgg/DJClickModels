#ifndef CACM_H
#define CACM_H
//#define double long double
class cacm:public model
{
    public:
        vector<int> doc_cnt,doc_clk;
        double gamma[DOCPERPAGE+7][DOCPERPAGE+7];
        double bias[DOCPERPAGE+7][3];
        double dgamma[DOCPERPAGE+7][DOCPERPAGE+7],dbias[DOCPERPAGE+7][3];
        vector<double> satisfied;
        vector<double> drel,dsatisfied;
        double clk_bias,dclk_bias;
        double dlt=0.9,ddlt=0.85,eps=1e-6;
        double F[12][15][2][2][2][2],B[12][15][2][2][2][2];
        double cmpp[4];
        double cita=3;//for softmax

        cacm()
        {
            name="CACM";
        }
        void cmp(double rel1,double rel2,double rel3,int i,int j,int a,int b,int c,double *cmpp)
        {
            static double p0,p1,p2,p3,ptot;

            p0=exp(cita*gamma[i][j]);
            p1=exp(cita*(rel1/*+bias[i][0]*/+clk_bias*a));
            p2=exp(cita*(rel2+bias[i][1]+clk_bias*b));
            p3=exp(cita*(rel3+bias[i][2]+clk_bias*c));
            //p2=
            //p3=0;
            ptot=p0+p1+p2+p3;
            p0+=ptot*0.01;
            p1+=ptot*0.01;
            p2+=ptot*0.01;
            p3+=ptot*0.01;
            //ptot*=1.21;
            cmpp[0]=p0/ptot;
            cmpp[1]=p1/ptot;
            cmpp[2]=p2/ptot;
            cmpp[3]=p3/ptot;
            /*if(i==3)
            cerr<<"CMP\t"<<i<<"\t"<<j<<"\t"<<a<<b<<c<<"\t"<<cmpp[0]<<"\t"<<cmpp[1]<<"\t"<<cmpp[2]<<"\t"<<cmpp[3]<<endl;*/
        }
        void cmpd(int id1,int id2,int id3,int i,int j,int a,int b,int c,double P,double pc0,double pc1,double pc2,double pc3)
        {
            static double p0,p1,p2,p3,ptot;
            p0=exp(cita*gamma[i][j]);
            p1=exp(cita*(doc_rel[id1]/*+bias[i][0]*/+clk_bias*a));
            p2=exp(cita*(doc_rel[id2]+bias[i][1]+clk_bias*b));
            p3=exp(cita*(doc_rel[id3]+bias[i][2]+clk_bias*c));
            //p2=
            //p3=0;
            ptot=p0+p1+p2+p3;
            //
            /*p0+=ptot*0.05;
            p1+=ptot*0.05;
            p2+=ptot*0.05;
            p3+=ptot*0.05;
            ptot*=1.2;*/
            /*if(docs[id1].name=="AX#*$#a")
                cerr<<"CMPD\t"<<i<<"\t"<<j<<"\t"<<docs[id1].name<<docs[id2].name<<docs[id3].name<<a<<b<<c<<"\t"<<ptot<<"\t"<<P<<"\t"<<\
                F[i][j][0][a][b][c]*pc0*                      cita*p1*p0        /ptot/ptot/P<<"\t"<<\
                F[i][j][0][a][b][c]*pc1*    cita*p1*(ptot-p1)        /ptot/ptot/P<<"\t"<<\
                F[i][j][0][a][b][c]*pc2*    cita*p1*p2        /ptot/ptot/P<<"\t"<<\
                F[i][j][0][a][b][c]*pc3*    cita*p1*p3        /ptot/ptot/P<<"\t"<<\
                endl;*/
            dgamma[i][j]+=    F[i][j][0][a][b][c]*pc0*                      cita*p0*(ptot-p0) /ptot/ptot/P;
            drel[id1]-=    F[i][j][0][a][b][c]*pc0*                      cita*p1*p0        /ptot/ptot/P;
            dclk_bias-=    F[i][j][0][a][b][c]*pc0*                    a*cita*p1*p0        /ptot/ptot/P;
            drel[id2]-=    F[i][j][0][a][b][c]*pc0*                      cita*p2*p0        /ptot/ptot/P;
            dclk_bias-=    F[i][j][0][a][b][c]*pc0*                    b*cita*p2*p0        /ptot/ptot/P;
            dbias[i][1]-=  F[i][j][0][a][b][c]*pc0*                      cita*p2*p0        /ptot/ptot/P;
            drel[id3]-=    F[i][j][0][a][b][c]*pc0*                      cita*p3*p0        /ptot/ptot/P;
            dclk_bias-=    F[i][j][0][a][b][c]*pc0*                    c*cita*p3*p0        /ptot/ptot/P;
            dbias[i][2]-=  F[i][j][0][a][b][c]*pc0*                      cita*p3*p0        /ptot/ptot/P;


            drel[id1]+=    F[i][j][0][a][b][c]*pc1*                      cita*p1*(ptot-p1) /ptot/ptot/P;
            dclk_bias+=    F[i][j][0][a][b][c]*pc1*                    a*cita*p1*(ptot-p1) /ptot/ptot/P;
            dgamma[i][j]-=    F[i][j][0][a][b][c]*pc1*                      cita*p0*p1        /ptot/ptot/P;
            drel[id2]-=    F[i][j][0][a][b][c]*pc1*                      cita*p2*p1        /ptot/ptot/P;
            dclk_bias-=    F[i][j][0][a][b][c]*pc1*                    b*cita*p2*p1        /ptot/ptot/P;
            dbias[i][1]-=  F[i][j][0][a][b][c]*pc1*                      cita*p2*p1        /ptot/ptot/P;
            drel[id3]-=    F[i][j][0][a][b][c]*pc1*                      cita*p3*p1        /ptot/ptot/P;
            dclk_bias-=    F[i][j][0][a][b][c]*pc1*                    c*cita*p3*p1        /ptot/ptot/P;
            dbias[i][2]-=  F[i][j][0][a][b][c]*pc1*                      cita*p3*p1        /ptot/ptot/P;


            drel[id2]+=    F[i][j][0][a][b][c]*pc2*                      cita*p2*(ptot-p2) /ptot/ptot/P;
            dbias[i][1]+=  F[i][j][0][a][b][c]*pc2*                      cita*p2*(ptot-p2) /ptot/ptot/P;
            dclk_bias+=    F[i][j][0][a][b][c]*pc2*                    b*cita*p2*(ptot-p2) /ptot/ptot/P;
            dgamma[i][j]-=    F[i][j][0][a][b][c]*pc2*                      cita*p0*p2        /ptot/ptot/P;
            drel[id1]-=    F[i][j][0][a][b][c]*pc2*                      cita*p1*p2        /ptot/ptot/P;
            dclk_bias-=    F[i][j][0][a][b][c]*pc2*                    a*cita*p1*p2        /ptot/ptot/P;
            drel[id3]-=    F[i][j][0][a][b][c]*pc2*                      cita*p3*p2        /ptot/ptot/P;
            dclk_bias-=    F[i][j][0][a][b][c]*pc2*                    c*cita*p3*p2        /ptot/ptot/P;
            dbias[i][2]-=  F[i][j][0][a][b][c]*pc2*                      cita*p3*p2        /ptot/ptot/P;

            drel[id3]+=    F[i][j][0][a][b][c]*pc3*                      cita*p3*(ptot-p3) /ptot/ptot/P;
            dbias[i][2]+=  F[i][j][0][a][b][c]*pc3*                      cita*p3*(ptot-p3) /ptot/ptot/P;
            dclk_bias+=    F[i][j][0][a][b][c]*pc3*                    c*cita*p3*(ptot-p3) /ptot/ptot/P;
            dgamma[i][j]-=    F[i][j][0][a][b][c]*pc3*                      cita*p0*p3        /ptot/ptot/P;
            drel[id1]-=    F[i][j][0][a][b][c]*pc3*                      cita*p1*p3        /ptot/ptot/P;
            dclk_bias-=    F[i][j][0][a][b][c]*pc3*                    a*cita*p1*p3        /ptot/ptot/P;
            drel[id2]-=    F[i][j][0][a][b][c]*pc3*                      cita*p2*p3        /ptot/ptot/P;
            dclk_bias-=    F[i][j][0][a][b][c]*pc3*                    b*cita*p2*p3        /ptot/ptot/P;
            dbias[i][1]-=  F[i][j][0][a][b][c]*pc3*                      cita*p2*p3        /ptot/ptot/P;

            /*if(dgamma[1][0]!=dgamma[1][0])
            {
                cerr<<"Dgamma\t"<<i<<"\t"<<j\
                <<"\t"<<ptot<<"\t"<<P\
                <<"\t"<<F[i][j][0][a][b][c]\
                <<"\t\t"<<pc0\
                <<"\t"<<pc1\
                <<"\t"<<pc2\
                <<"\t"<<pc3\
                <<"\t\t"<<p0\
                <<"\t"<<p1\
                <<"\t"<<p2\
                <<"\t"<<p3\
                <<"\t"<<F[i][j][0][a][b][c]*pc0*                      cita*p0*(ptot-p0) /ptot/ptot/P\
                <<"\t"<<F[i][j][0][a][b][c]*pc1*                      cita*p0*p1        /ptot/ptot/P\
                <<"\t"<<F[i][j][0][a][b][c]*pc2*                      cita*p0*p2        /ptot/ptot/P\
                <<"\t"<<F[i][j][0][a][b][c]*pc3*                      cita*p0*p3        /ptot/ptot/P<<endl;
                exit(0);
            }*/
        }
        void clear_update_val(double &v,double &in_dlt,double minn=0.05,double maxx=0.95,double A=pr,double B=pr)
        {
            if(in_dlt>eps)
            {
                v=min(maxx-eps,v+dlt);
            }
            if(in_dlt<-eps)
            {
                v=max(minn+eps,v-dlt);
            }
            if(minn>-1e5)
                in_dlt=A/(v-minn)-B/(maxx-v);
            else
                in_dlt=0;
        }
        void train_update_clear()
        {
            for(int i=1;i<=DOCPERPAGE+3;++i)
            {
                for(int j=0;j<=i+2;++j)
                    clear_update_val(gamma[i][j],dgamma[i][j],-1e9,1e9);
                for(int j=0;j<=2;++j)
                    clear_update_val(bias[i][j],dbias[i][j],-1e9,1e9);
            }
            for(int i=1;i<=docs.size()+2;++i)
            {
                clear_update_val(doc_rel[i],drel[i],-1e9,1e9);
                clear_update_val(satisfied[i],dsatisfied[i]);
            }
            clear_update_val(clk_bias,dclk_bias,-1e9,1e9);
        }
        void train_init()
        {
            for(int i=1;i<=DOCPERPAGE+3;++i)
            {
                for(int j=0;j<=i+2;++j)
                {
                    gamma[i][j]=0;
                    dgamma[i][j]=0;
                }
                for(int j=0;j<=2;++j)
                {
                    bias[i][j]=-j*0.5/*magic option*/;
                    dbias[i][j]=0;
                }
            }
            for(int i=1;i<=docs.size()+2;++i)
            {
                doc_rel[i]=0;
                drel[i]=0;
                satisfied[i]=0.5;
                dsatisfied[i]=0;
            }
            for(int i=1;i<sessions.size();++i)
            {
                sessions[i].doc_id[DOCPERPAGE+1]=docs.size()+1;
                sessions[i].doc_id[DOCPERPAGE+2]=docs.size()+2;
            }
        }
        double calcf(Session &sess,double* click_prob=0)
        {
            double rlt=0;
            memset(F,0,sizeof(F));
            F[1][0][0][0][0][0]=1;
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                for(int j=0;j<=i+2;++j)
                {
                    for(int k=0;k<=1;++k)
                    {
                        for(int a=0;a<=1;++a)
                            for(int b=0;b<=1;++b)
                                for(int c=0;c<=1;++c)
                                    /*if(F[i][j][k][a][b][c]>eps)*/
                                    {
                                        /*if(F[i][j][k][a][b][c]>0.01)
                                            cerr<<i<<j<<k<<a<<b<<c<<"\t"<<F[i][j][k][a][b][c]<<"\t"<<satisfied[sess.doc_id[i]]<<"\t"<<docs[sess.doc_id[i]].name<<endl;*/
                                        if(k==1)
                                        {
                                            if(a==(sess.click_time[i]>.1))
                                            {
                                                F[i+1][j][k][b][c][0]+=F[i][j][k][a][b][c];
                                                if(click_prob!=0)
                                                    click_prob[i]+=/*(a?1:-1)*/F[i][j][k][a][b][c];
                                                if(i==DOCPERPAGE)
                                                    rlt+=F[i][j][k][a][b][c];
                                            }
                                            continue;
                                        }
                                        cmp(doc_rel[sess.doc_id[i]],doc_rel[sess.doc_id[i+1]],doc_rel[sess.doc_id[i+2]],i,j,a,b,c,cmpp);
                                        if(a==(sess.click_time[i]>.1))
                                        {
                                            F[i+1][j][0][b][c][0]+=F[i][j][0][a][b][c]*cmpp[0];
                                            if(click_prob!=0)
                                                click_prob[i]+=/*(a?1:-1)*/F[i][j][k][a][b][c]*cmpp[0];
                                            if(i==DOCPERPAGE)
                                                rlt+=F[i][j][k][a][b][c]*cmpp[0];
                                        }
                                        F[i][j+1][0][1][b][c]+=F[i][j][0][a][b][c]*cmpp[1]*(1.-satisfied[sess.doc_id[i]]);
                                        F[i][j+1][1][1][b][c]+=F[i][j][0][a][b][c]*cmpp[1]*satisfied[sess.doc_id[i]];
                                        F[i][j+1][0][a][1][c]+=F[i][j][0][a][b][c]*cmpp[2]*(1.-satisfied[sess.doc_id[i+1]]);
                                        F[i][j+1][1][a][1][c]+=F[i][j][0][a][b][c]*cmpp[2]*satisfied[sess.doc_id[i+1]];
                                        F[i][j+1][0][a][b][1]+=F[i][j][0][a][b][c]*cmpp[3]*(1.-satisfied[sess.doc_id[i+2]]);
                                        F[i][j+1][1][a][b][1]+=F[i][j][0][a][b][c]*cmpp[3]*satisfied[sess.doc_id[i+2]];
                                    }
                    }
                }
            }
            return rlt;
        }
        double calcb(Session &sess)
        {
            memset(B,0,sizeof(B));
            //F[1][0][0][0][0][0]=1;
            for(int j=0;j<=DOCPERPAGE+3;++j)
                for(int k=0;k<=1;++k)
                    for(int a=0;a<=1;++a)
                        for(int b=0;b<=1;++b)
                            for(int c=0;c<=1;++c)
                                B[DOCPERPAGE+1][j][k][a][b][c]=1;
            for(int i=DOCPERPAGE;i;--i)
            {
                for(int j=i+2;j>=0;--j)
                {
                    for(int k=0;k<=1;++k)
                    {
                        for(int a=0;a<=1;++a)
                            for(int b=0;b<=1;++b)
                                for(int c=0;c<=1;++c)
                                {
                                    if(k==1)
                                    {
                                        if(a==(sess.click_time[i]>.1))
                                        {
                                            B[i][j][k][a][b][c]+=B[i+1][j][k][b][c][0];
                                        }
                                    /*if(B[i][j][k][a][b][c]>0.01)
                                        cerr<<"B\t"<<i<<j<<k<<a<<b<<c<<"\t"<<B[i][j][k][a][b][c]<<"\t"<<satisfied[sess.doc_id[i]]<<"\t"<<docs[sess.doc_id[i]].name<<endl;*/
                                        continue;
                                    }
                                    cmp(doc_rel[sess.doc_id[i]],doc_rel[sess.doc_id[i+1]],doc_rel[sess.doc_id[i+2]],i,j,a,b,c,cmpp);
                                    if(a==(sess.click_time[i]>.1))
                                    {
                                        B[i][j][0][a][b][c]+=B[i+1][j][0][b][c][0]*cmpp[0];
                                    }
                                    B[i][j][0][a][b][c]+=B[i][j+1][0][1][b][c]*cmpp[1]*(1.-satisfied[sess.doc_id[i]]);
                                    B[i][j][0][a][b][c]+=B[i][j+1][1][1][b][c]*cmpp[1]*satisfied[sess.doc_id[i]];
                                    B[i][j][0][a][b][c]+=B[i][j+1][0][a][1][c]*cmpp[2]*(1.-satisfied[sess.doc_id[i+1]]);
                                    B[i][j][0][a][b][c]+=B[i][j+1][1][a][1][c]*cmpp[2]*satisfied[sess.doc_id[i+1]];
                                    B[i][j][0][a][b][c]+=B[i][j+1][0][a][b][1]*cmpp[3]*(1.-satisfied[sess.doc_id[i+2]]);
                                    B[i][j][0][a][b][c]+=B[i][j+1][1][a][b][1]*cmpp[3]*satisfied[sess.doc_id[i+2]];
                                    /*if(B[i][j][k][a][b][c]>0.01)
                                        cerr<<"B\t"<<i<<j<<k<<a<<b<<c<<"\t"<<B[i][j][k][a][b][c]<<"\t"<<satisfied[sess.doc_id[i]]<<"\t"<<docs[sess.doc_id[i]].name<<endl;*/
                                }
                    }
                }
            }
            return B[1][0][0][0][0][0];
        }
        void calcd(Session &sess)
        {
            double P;
            P=calcf(sess);
            calcb(sess);
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                for(int j=0;j<=i+2;++j)
                {
                    for(int k=0;k<=1;++k)
                    {
                        for(int a=0;a<=1;++a)
                            for(int b=0;b<=1;++b)
                                for(int c=0;c<=1;++c)
                                    /*if(F[i][j][k][a][b][c]>eps)*/
                                    {
                                        if(k==1)
                                        {
                                            if(a==(sess.click_time[i]>.1))
                                            {
                                                //F[i+1][j][k][b][c][0]+=F[i][j][k][a][b][c];
                                            }
                                            continue;
                                        }
                                        cmpd(sess.doc_id[i],sess.doc_id[i+1],sess.doc_id[i+2],i,j,a,b,c,P,\
                                            (a==(sess.click_time[i]>.1))?B[i+1][j][0][b][c][0]:0,\
                                            satisfied[sess.doc_id[i+0]]*B[i][j+1][1][1][b][c] + (1.-satisfied[sess.doc_id[i+0]])*B[i][j+1][0][1][b][c],\
                                            satisfied[sess.doc_id[i+1]]*B[i][j+1][1][a][1][c] + (1.-satisfied[sess.doc_id[i+1]])*B[i][j+1][0][a][1][c],\
                                            satisfied[sess.doc_id[i+2]]*B[i][j+1][1][a][b][1] + (1.-satisfied[sess.doc_id[i+2]])*B[i][j+1][0][a][b][1]);
                                        cmp(doc_rel[sess.doc_id[i]],doc_rel[sess.doc_id[i+1]],doc_rel[sess.doc_id[i+2]],i,j,a,b,c,cmpp);
                                        if(a==(sess.click_time[i]>.1))
                                        {
                                            //F[i+1][j][0][b][c][0]+=F[i][j][0][a][b][c]*cmpp[0];
                                        }
                                        dsatisfied[sess.doc_id[i]]   -= F[i][j][0][a][b][c] * cmpp[1] * B[i][j+1][0][1][b][c] /P;
                                        dsatisfied[sess.doc_id[i]]   += F[i][j][0][a][b][c] * cmpp[1] * B[i][j+1][1][1][b][c] /P;
                                        dsatisfied[sess.doc_id[i+1]] -= F[i][j][0][a][b][c] * cmpp[2] * B[i][j+1][0][a][1][c] /P;
                                        dsatisfied[sess.doc_id[i+1]] += F[i][j][0][a][b][c] * cmpp[2] * B[i][j+1][1][a][1][c] /P;
                                        dsatisfied[sess.doc_id[i+2]] -= F[i][j][0][a][b][c] * cmpp[3] * B[i][j+1][0][a][b][1] /P;
                                        dsatisfied[sess.doc_id[i+2]] += F[i][j][0][a][b][c] * cmpp[3] * B[i][j+1][1][a][b][1] /P;
                                        /*
                                        F[i][j+1][0][1][b][c]+=F[i][j][0][a][b][c]*cmpp[1]*(1.-satisfied[sess.doc_id[i]]);
                                        F[i][j+1][1][1][b][c]+=F[i][j][0][a][b][c]*cmpp[1]*satisfied[sess.doc_id[i]];
                                        F[i][j+1][0][a][1][c]+=F[i][j][0][a][b][c]*cmpp[2]*(1.-satisfied[sess.doc_id[i+1]]);
                                        F[i][j+1][1][a][1][c]+=F[i][j][0][a][b][c]*cmpp[2]*satisfied[sess.doc_id[i+1]];
                                        F[i][j+1][0][a][b][1]+=F[i][j][0][a][b][c]*cmpp[3]*(1.-satisfied[sess.doc_id[i+2]]);
                                        F[i][j+1][1][a][b][1]+=F[i][j][0][a][b][c]*cmpp[3]*satisfied[sess.doc_id[i+2]];*/
                                    }
                    }
                }
            }
        }
        void train()
        {
            doc_rel=vector<double>(docs.size()+5);
            satisfied=vector<double>(docs.size()+5);
            drel=vector<double>(docs.size()+5);
            dsatisfied=vector<double>(docs.size()+5);
            int sess_cnt=0;
            train_init();
                clk_bias=-100;
            for(int round=1;round<=MAXROUND;++round)
            {
                cerr<<"ROUND\t"<<round<<endl;
                if(round%10==0)
                    cerr<<test(false)<<endl;
                for(int id=0;id<sessions.size();id+=1)
                {
                    //cerr<<round<<"\t"<<id<<endl;
                    auto &sess=sessions[id];
                    if(sess.enable==false||!istrain(sess,sess_cnt))
                        continue;
                    ++sess_cnt;
                    calcd(sess);
                }
                /*for(int i=1;i<=DOCPERPAGE;++i)
                    cerr<<dgamma[i][0]<<"\t"<<dgamma[i][1]<<"\t"<<dbias[i][1]<<"\t"<<dbias[i][2]<<endl;*/
                train_update_clear();/*
                for(int i=1;i<docs.size();++i)
                    if((!(doc_rel[i]<1e30))||(!(doc_rel[i]>-1e30))||(!(satisfied[i]>1e-10))||(!(satisfied[i]<1.-1e-10)))
                        cerr<<docs[i].name<<"\t"<<doc_rel[i]<<"\t"<<satisfied[i]<<endl;*/
                for(int i=1;i<=DOCPERPAGE;++i)
                    bias[i][2]=-100;
                for(int i=1;i<=DOCPERPAGE;++i)
                    cerr<<gamma[i][0]<<"\t"<<gamma[i][1]<<"\t"<<bias[i][1]<<"\t"<<bias[i][2]<<endl;
                clk_bias=-100;
                dlt*=ddlt;
            }

            /*FILE* outfile=fopen("../output/cacm_args","w");
            assert(outfile);
            fclose(outfile);*/
        }
        double get_rel(int id)
        {
            return doc_rel[id]*satisfied[id];
        }
        void load()
        {
            FILE* infile=fopen("../output/cacm_args","r");
            assert(infile);
            fclose(infile);
        }
        void get_click_prob(Session &sess,double* click_prob)
        {
            //click_prob[i]=P(C_1,C_2...C_i) X
            //click_prob[i]=P(C_i|C_1,C_2,...C_{i-1})
            //examination = P(E_k|C_1,C_2,...C_{k-1})
            click_prob[0]=1.0;
            for(int i=1;i<=DOCPERPAGE;++i)
                click_prob[i]=0;
            calcf(sess,click_prob);
            for(int i=DOCPERPAGE;i;--i)
            {
                click_prob[i]/=click_prob[i-1];
                //cerr<<"CLK"<<click_prob[i]<<endl;
            }
            //click_prob is NOT P(C_i|C_1,C_2...C_i-1)
            //click_prob is just P(C_1,C_2...C_i)/P(C_1,C_2...C_i-1)
        }
};
#endif
