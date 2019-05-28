#!/usr/bin/python3
import os, shutil
import json
import math
from urllib.parse import quote
n = 10 #n of ndcg
signed_query={}

json_str = input()
SRR = json.loads(json_str)
flag=0

for query in SRR:
	query_word=quote(query["query"].encode('gb2312',errors='ignore'), 'gb2312')
	if query_word not in signed_query:
		signed_query[query_word] = {"sign":{}}
	for i in range(10):
		doc_name=str(query["results"][str(i)]["url"])
		doc_rel=int(query["results"][str(i)]["relevance"])
		signed_query[query_word]["sign"][doc_name] = doc_rel

id2query={}
id2url={}

in_file = open('../zhangjunqi_magic/query_id','r',errors='ignore')
line = in_file.readline()
while line:
	[query,iid]=line.strip().split('\t')
	id2query[iid]=quote(query.encode('gb2312',errors='ignore'), 'gb2312')
	line = in_file.readline()

in_file = open('../zhangjunqi_magic/url_id','r',errors='ignore')
line = in_file.readline()
while line:
	[url,iid]=line.strip().split('\t')
	id2url[iid]=url
	line = in_file.readline()

work_dir = '../rel'

model_name=[]
ndcg={}

for parent, dirnames, filenames in os.walk(work_dir,  followlinks=True):
	for filename in filenames:
		if filename[0] == '.':
			continue
		file_path = os.path.join(parent, filename)
		in_file = open(file_path,'r',errors='ignore')
		print(filename[:-4])
		model_name.append(filename[:-4])
		filename=filename[:-4]
		ndcg[filename]=0
		line = in_file.readline()
		while line:
			try:
				[doc_name,doc_rel] = line.strip().split('\t')
				[query_word,doc_name] = doc_name.split('#')
				if query_word in id2query:
					query_word=id2query[query_word]
				if doc_name in id2url:
					doc_name=id2url[doc_name]
				if query_word in signed_query:
					if filename not in signed_query[query_word]:
						signed_query[query_word][filename]={}
					signed_query[query_word][filename][doc_name] = float(doc_rel)
			except:
				print(filename,line)
			#print(doc_name,doc_rel)
			line = in_file.readline()

def takeSecond(elem):
	return elem[1]

def calc_query(query,mods):
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
		indcg+=(pow(2.0,seq[i][1]-1.)-1.)/math.log(i+2,2)
		#indcg+=(seq[i][1])/math.log(i+2,2)
	if indcg<1e-9:
		return False
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
			nndcg+=(pow(2.0,query["sign"][seq[i][0]]-1.)-1.)/math.log(i+2,2)
			#nndcg+=query["sign"][seq[i][0]])/math.log(i+2,2)
		ndcg[mod]+=nndcg/indcg
	return True

cnt=0
for query in signed_query.values():
	if "sign" in query:
		if calc_query(query,model_name):
			cnt += 1
print(cnt)
for mod in model_name:
	print(mod+'\t'+str(ndcg[mod]/cnt))

input_of_ranking = open('../output/ranking_all','w')
for query in signed_query.values():
	