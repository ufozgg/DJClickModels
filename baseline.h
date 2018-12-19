#ifndef BASELINE_H
#define BASELINE_H
#define double long double
class baseline:public model
{
    public:
        vector<int> doc_cnt,doc_clk;
        void train()
        {
            name="Baseline:";
            doc_cnt=vector<int>(docs.size()+1);
            doc_clk=vector<int>(docs.size()+1);
            doc_rel=vector<double>(docs.size()+1);
            for(int i=0;i<docs.size();++i)
            {
                doc_cnt[i]=doc_clk[i]=0;
            }
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
                    if(sess.click_time[i]>.1)//clicked
                        ++doc_clk[sess.doc_id[i]];
                    ++doc_cnt[sess.doc_id[i]];
                }
            }
            double t1=0,t2=0;
            for(int i=0;i<docs.size();++i)
            {
                if(doc_clk[i])
                    t1+=1;
                if(doc_cnt[i])
                    t2+=1;
            }
            double K=1;
            t1=t1/t2*K;
            t2=K;
            t1=0.4*K;
            for(int i=0;i<docs.size();++i)
            {
                doc_rel[i]=(doc_clk[i]+t1)/(doc_cnt[i]+t2);
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
