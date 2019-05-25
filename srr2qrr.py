#!/usr/bin/python3
import json
from urllib.parse import quote
json_str = input()
SRR = json.loads(json_str)
flag=0
for query in SRR:
	for i in range(10):
		print(quote(query["query"].encode('gb2312',errors='ignore'), 'gb2312')+"\t"+str(query["results"][str(i)]["url"])+"\t"+str(query["results"][str(i)]["relevance"]))
