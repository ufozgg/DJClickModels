#ifndef DBN_H
#define DBN_H
//#define double long double
class dbn:public model
{
    public:
        vector<double> a,s;
        vector<double> sum_sd,sum_ad;
        double gamma=0.9;
        void train()
        {
            name="Dbn:";
            a=vector<int>(docs.size()+1);
            s=vector<int>(docs.size()+1);
            int sess_cnt=0;
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
                }
            }
            for(int i=1;i<docs.size();++i)
            {
                a[i]=s[i]=0.5;
            }
            int rnd;
            double pr[DOCPERPAGE+2];
            for(rnd=1;rnd<=20;++rnd)
            {
                for(auto &sess:sessions)
                {
                    if(sess.enable==false)
                        continue;
                    ++sess_cnt;
                    if(istest(sess,sess_cnt))//在没有Overfit可能性的模型里应写作istest，在需要验证集的模型里应写作!istrain
                        continue;
                    get_click_prob(sess,pr);
                    for(int i=1;i<=DOCPERPAGE;++i)
                    {
                        
                    }
                }
            }
        }
        void get_click_prob(Session &sess,double* click_prob)
        {
            //click_prob[i]=P(C_1,C_2...C_i) X
            //click_prob[i]=P(C_i|C_1,C_2,...C_{i-1})
            //examination = P(E_k|C_1,C_2,...C_{k-1})
            click_prob[0]=1.0;
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                if(sess.click_time[i]>.1)
                {
                    click_prob[i]=doc_rel[sess.doc_id[i]];
                }
                else
                {
                    click_prob[i]=1.-doc_rel[sess.doc_id[i]];
                }
            }
        }
};
#endif
