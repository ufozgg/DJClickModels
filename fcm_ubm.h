#ifndef FCM_UBM_H
#define FCM_UBM_H
//#define double long double
class fcm_ubm:public model
{
    public:
        double gamma[DOCPERPAGE+2][DOCPERPAGE+2];
        double beta[DOCPERPAGE*2+4];
        vector<double> yita[DOCPERPAGE+2],AP[DOCPERPAGE+2];
        vector<double> alpha;
        vector<int> mindiss;
        void init()
        {
            alpha.resize(docs.size()+2);
            for(int i=0;i<yita.size();++i)
            {
                yita[i].resize(MAXVERTICLE+2);
                AP[i].resize(MAXVERTICLE+2);
            }
        }
        void get_diss(Session &sess)
        {
            if(mindiss.size()<DOCPERPAGE)
                mindiss.resize(DOCPERPAGE+2);
            int la=100;
            for(int i=1;i<=DOCPERPAGE;++i)
            {
                if(la!=100)
                    mindiss[i]=la;
                else
                    mindiss[i]=100;
                if(sess.click_time[i]>.1)
                    la=i;
            }
            la=100;
            for(int i=DOCPERPAGE;i;--i)
            {
                if(la!=100)
                {
                    if(mindiss[i]==100||abs(la-i)<abs(i-mindiss[i]))
                        mindiss[i]=la;
                }
                if(sess.click_time[i]>.1)
                    la=i;
            }
        }
        void train()
        {
            name="Fcm_Ubm";
            
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
                    cp=alpha[sess.doc_id[i]]*(yita[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type]*beta[DOCPERPAGE+i-mindiss[i]]*(1.-gamma[i][i-la])+gamma[i][i-la]);
                }
                else
                {
                    cp=(1.-AP[la][docs[sess.doc_id[i]].type])*alpha[sess.doc_id[i]]*(yita[mindiss[i]][docs[sess.doc_id[mindiss[i]]].type]*beta[DOCPERPAGE+i-mindiss[i]]*(1.-gamma[i][i-la])+gamma[i][i-la]);
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
