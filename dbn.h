#ifndef DBN_H
#define DBN_H
//#define double long double
class dbn:public model
{
    public:
        vector<double> a,s;
        vector<double> Qa,Qs;
        vector<int> clk_tim;
        double gamma=0.9;
        void train()
        {
            name="Dbn:";
            a=vector<double>(docs.size()+1);
            s=vector<double>(docs.size()+1);
            Qa=vector<double>(docs.size()+1);
            Qs=vector<double>(docs.size()+1);
            clk_tim=vector<int>(docs.size()+1);
            int sess_cnt=0;
            for(int i=1;i<docs.size();++i)
                docs[i].train_tim=1,clk_tim[i]=1;
            for(auto &sess:sessions)
            {
                if(sess.enable==false)
                    continue;
                ++sess_cnt;
                if(istest(sess,sess_cnt))//在没有Overfit可能性的模型里应写作istest，在需要验证集的模型里应写作!istrain
                    continue;
                for(int i=DOCPERPAGE;i;--i)
                {
                    ++docs[sess.doc_id[i]].train_tim;
                    if(sess.click_time[i]>.1)
                        ++clk_tim[sess.doc_id[i]];
                }
            }
            for(int i=1;i<docs.size();++i)
            {
                a[i]=s[i]=0.5;
            }
            int rnd;
            double pr0[DOCPERPAGE+2];
            double last_LL=-100,now_LL;
            for(rnd=1;rnd<=20;++rnd)
            {
                for(int i=1;i<docs.size();++i)
                    Qa[i]=Qs[i]=0.500;
                for(auto &sess:sessions)
                {
                    if(sess.enable==false)
                        continue;
                    ++sess_cnt;
                    if(istest(sess,sess_cnt))//在没有Overfit可能性的模型里应写作istest，在需要验证集的模型里应写作!istrain
                        continue;
                    get_examine_prob(sess,pr0);
                    for(int i=1;i<=DOCPERPAGE;++i)
                    {
                        assert(pr0[i]<=1.);
                        Qa[sess.doc_id[i]]+=pr0[i]*a[sess.doc_id[i]];
                        if(sess.click_time[i]>.1)
                            Qs[sess.doc_id[i]]+=pr0[i]*a[sess.doc_id[i]]*s[sess.doc_id[i]];
                    }
                }

                for(int i=1;i<docs.size();++i)
                {
                    a[i]=Qa[i]/docs[i].train_tim;
                    assert(Qa[i]<docs[i].train_tim);
                    assert(Qs[i]<clk_tim[i]);
                    if(clk_tim[i])
                        s[i]=Qs[i]/clk_tim[i];
                    //(tim-x)/(a-1)+x/a=0
                    //a*tim=x
                    //a=x/tim
                }
                now_LL=this->test(false,3);
                if(now_LL-1e-5<last_LL)
                    break;
                cerr<<"LL:=\t"<<now_LL<<endl;
                last_LL=now_LL;
            }
        }
        void get_examine_prob(Session &sess,double *pr)
        {
            static double forward[DOCPERPAGE+2][2],backward[DOCPERPAGE+2][2];
            memset(forward,0,sizeof(forward));//alpha
            memset(backward,0,sizeof(backward));//beta
            forward[1][1]=1;
            forward[1][0]=0;
            backward[DOCPERPAGE+1][1]=1;
            backward[DOCPERPAGE+1][0]=1;
            for(int i=1;i<DOCPERPAGE;++i)
            {
                if(sess.click_time[i]>.1)
                {
                    forward[i+1][0]=forward[i][1]*a[sess.doc_id[i]]*(1.-gamma+gamma*s[sess.doc_id[i]]);
                    forward[i+1][1]=forward[i][1]*a[sess.doc_id[i]]*(1-s[sess.doc_id[i]])*gamma;
                }
                else
                {
                    forward[i+1][0]=forward[i][0]+forward[i][1]*(1.-a[sess.doc_id[i]])*(1.-gamma);
                    forward[i+1][1]=forward[i][1]*(1.-a[sess.doc_id[i]])*gamma;
                }
            }
            for(int i=DOCPERPAGE;i>1;--i)
            {
                if(sess.click_time[i-1]>.1)
                {
                    //backward[i-1][0]=0;
                    backward[i-1][1]=backward[i][0]*a[sess.doc_id[i-1]]*(1.-gamma+gamma*s[sess.doc_id[i-1]])\
                        +backward[i][1]*a[sess.doc_id[i-1]]*(1.-s[sess.doc_id[i-1]])*gamma;
                }
                else
                {
                    backward[i-1][1]=backward[i][1]*(1.-a[sess.doc_id[i-1]])*gamma+\
                        backward[i][0]*(1.-a[sess.doc_id[i-1]])*(1.-gamma);
                    backward[i-1][0]=backward[i][0];
                }
            }
            for(int i=1;i<=DOCPERPAGE;++i)
                pr[i]=forward[i][1]*backward[i][1]/(forward[i][0]*backward[i][0]+forward[i][1]*backward[i][1]);
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
                    click_prob[i]=examination*a[sess.doc_id[i]];
                    examination=(1.-s[sess.doc_id[i]])*gamma;
                }
                else
                {
                    click_prob[i]=1.-examination*a[sess.doc_id[i]];
                    examination*=(1.-a[sess.doc_id[i]])/(1.-examination*a[sess.doc_id[i]])*gamma;
                }
            }
        }
};
#endif
