#!/usr/bin/python3
import os, shutil
import json
import math
import random
import numpy as np
from scipy import stats
from urllib.parse import *
n = 1 #n of ndcg
signed_query={}
root_dir='../biaozhu/'

json_str = input()
SRR = json.loads(json_str)
flag=0
cc = [0 for i in range(30)]
for query in SRR:
	try:
		query_word=unquote(quote(query["query"].encode('gb2312',errors='ignore'), 'gb2312').strip(),'gb2312')
		if query_word not in signed_query:
			signed_query[query_word] = {"sign":{}}
			#print(query_word)
		for i in range(1000):
			if str(i) not in query["results"]:
				break
			doc_name=str(query["results"][str(i)]["url"]).strip()
			doc_rel=int(query["results"][str(i)]["relevance"])
			if len(doc_name) > 60:
				doc_name = doc_name[:60]
			signed_query[query_word]["sign"][doc_name] = doc_rel
	except:
		pass
	cc[len(query["results"])]+=1
print(cc)
id2query={}
id2url={}
in_file = open(root_dir+'query_id','r',errors='ignore')
line = in_file.readline()
while line:
	[query,iid]=line.strip().split('\t')
	id2query[iid.strip()]=query#quote(query.encode('gb2312',errors='ignore'), 'gb2312').strip()
	#print(query)
	line = in_file.readline()

in_file = open(root_dir+'url_id','r',errors='ignore')
line = in_file.readline()
while line:
	[url,iid]=line.strip().split('\t')
	if len(url) > 60:
		url = url[:60]
	id2url[iid.strip()]=url
	line = in_file.readline()

work_dir = '../rel'

model_name=[]
ndcg={}
ndcg["random"]=[]
cnt=0
for parent, dirnames, filenames in os.walk(work_dir,  followlinks=True):
	for filename in filenames:
		if filename[0] == '.':
			continue
		file_path = os.path.join(parent, filename)
		in_file = open(file_path,'r',errors='ignore')
		print(filename[:-4])
		model_name.append(filename[:-4])
		filename=filename[:-4]
		ndcg[filename]=[]
		line = in_file.readline()
		while line:
			try:
				[doc_name,doc_rel] = line.strip().split('\t')
				doc_name=doc_name.strip()
				doc_rel=doc_rel.strip()
				p = doc_name.find("#*$#")
				query_word=doc_name[:p]
				doc_name=doc_name[p+4:]
				#[query_word,doc_name] = doc_name.split('#')
				query_word=query_word.strip()
				doc_name=doc_name.strip()
				if query_word in id2query:
					query_word=id2query[query_word]
				if doc_name in id2url:
					doc_name=id2url[doc_name]
				#print(query_word,doc_name)
				if query_word in signed_query:
					if filename not in signed_query[query_word]:
						signed_query[query_word][filename]={}
					signed_query[query_word][filename][doc_name] = float(doc_rel)
					cnt += 1
			except:
				print(filename,line)
			#print(doc_name,doc_rel)
			line = in_file.readline()
print(cnt)
def takeSecond(elem):
	return elem[1]
to_be_mark = 0
def calc_queryA(query,mods):#if any results didn't signed return false
	#print(query)
	global to_be_mark
	for mod in mods:
		#print(query.keys())
		if mod not in query:
			return False
		seq=[]
		for [doc,rel] in query[mod].items():
			seq.append([doc,rel])
		if len(query[mod]) < n:
			return False
		seq.sort(key=takeSecond,reverse=True)
		for i in range(n):
			if seq[i][0] not in query["sign"]:
				return False
	indcg=0
	seq=[]
	for [doc,rel] in query["sign"].items():
		seq.append([doc,rel])
	seq.sort(key=takeSecond,reverse=True)
	for i in range(n):
		indcg+=(pow(2.0,max(1,seq[i][1]))-1.)/math.log(i+2,2)
		#indcg+=(seq[i][1])/math.log(i+2,2)
	#print(indcg)
	if indcg<1e-9:
		return False
	random.shuffle(seq)
	rndcg=0
	for i in range(n):
		rndcg+=(pow(2.0,max(1,seq[i][1]))-1.)/math.log(i+2,2)
	ndcg["random"]+=rndcg/indcg

	for mod in mods:
		#print(query.keys())
		if mod not in query:
			return False
		seq=[]
		nndcg=0
		for [doc,rel] in query[mod].items():
			seq.append([doc,rel])
		seq.sort(key=takeSecond,reverse=True)
		for i in range(n):
			nndcg+=(pow(2.0,max(1,query["sign"][seq[i][0]]))-1.)/math.log(i+2,2)
			#nndcg+=query["sign"][seq[i][0]])/math.log(i+2,2)
		ndcg[mod]+=nndcg/indcg
	to_be_mark += len(seq)
	return True
w = [0 for i in range(100)]
def calc_queryB(query,mods,K=100):#if no more than K results didn't signed , set to 1 else return false
	#print(query)
	global to_be_mark
	VALUE_FOR_UNSIGNED = 1#2.70768
	indcg=0
	seq=[]
	loss=0
	for mod in mods:
		#print(query.keys())
		if mod not in query:
			return False
	for [doc,rel] in query["sign"].items():
		seq.append([doc,rel])
	seq.sort(key=takeSecond,reverse=True)
	while len(seq)<n:
		seq.append([0,1.])
	for i in range(n):
		#print(i)
		#print(seq[i])
		if seq[i][1]==0:
			loss += 1
		indcg+=(pow(2.0,VALUE_FOR_UNSIGNED if seq[i][1]==0 else seq[i][1])-1.)/math.log(i+2,2)
		#indcg+=(seq[i][1])/math.log(i+2,2)
	#print(indcg)
	#if indcg<1e-9:
	#	return False
	for mod in mods:
		seq=[]
		nndcg=0
		for [doc,rel] in query[mod].items():
			seq.append([doc,rel])
		seq.sort(key=takeSecond,reverse=True)
		for i in range(n):
			if i < len(seq) and seq[i][0] in query["sign"]:
				if query["sign"][seq[i][0]]==0:
					loss += 1 
			else:
				loss += 1
			#nndcg+=query["sign"][seq[i][0]])/math.log(i+2,2)
	if loss > K:
		return False
	random.shuffle(seq)
	rndcg=0
	for i in range(n):
		rndcg+=(pow(2.0,max(1,seq[i][1]))-1.)/math.log(i+2,2)
	ndcg["random"].append(rndcg/indcg)
	for mod in mods:
		seq=[]
		nndcg=0
		for [doc,rel] in query[mod].items():
			seq.append([doc,rel])
		seq.sort(key=takeSecond,reverse=True)
		for i in range(n):
			if i < len(seq) and seq[i][0] in query["sign"]:
				nndcg+=(pow(2.0,VALUE_FOR_UNSIGNED if query["sign"][seq[i][0]]==0 else query["sign"][seq[i][0]])-1.)/math.log(i+2,2)
			else:
				nndcg+=pow(2,VALUE_FOR_UNSIGNED)/math.log(i+2,2)
			#nndcg+=query["sign"][seq[i][0]])/math.log(i+2,2)
		ndcg[mod].append(nndcg/indcg)
	to_be_mark += len(seq)
	w[loss] += 1
	return True
rlt={}
for n in range(1,11):
	for item in ndcg.keys():
		ndcg[item]=[]
	cnt=0
	for (key,query) in signed_query.items():
		if "sign" in query:
			if calc_queryB(query,model_name,99):
				cnt += 1
	#print(w)
	#print(cnt,to_be_mark)
	for mod in model_name:
		#for i in ndcg[mod]:
		s = sum(ndcg[mod])/len(ndcg[mod])
		my_c = 0
		my_s = 0
		for i in range(len(ndcg[mod])):
			if ndcg[mod][i] < ndcg["MVCM_sconly_add"][i]:
				my_c += 1
			elif ndcg[mod][i] == ndcg["MVCM_sconly_add"][i]:
				my_s += 1
		cor1, pval1 = stats.ttest_rel(ndcg[mod],ndcg["MVCM_sc"])
		if mod not in rlt:
			rlt[mod]=mod
		if sum(ndcg[mod])/len(ndcg[mod]) > sum(ndcg["MVCM_sc"])/len(ndcg["MVCM_sc"]):
			pval1 = -pval1
		if n in [1,3,5,10]:
			#rlt[mod]+='\t'+"%.4f"%(sum(ndcg[mod])/len(ndcg[mod]))
			rlt[mod]+='\t'+"%.6f"%pval1
		#print(mod+'\t'+str(s)+'\t'+str(pval1)+'\t'+str(my_c/len(ndcg[mod]))+'\t'+str(my_s/len(ndcg[mod]))+'\t'+str(1.-my_c/len(ndcg[mod])-my_s/len(ndcg[mod])))
for mod in rlt.keys():
	print(rlt[mod])
	#print('random\t'+str(ndcg["random"]/cnt))
print(len(ndcg["MVCM_sc"]))