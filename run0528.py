import os
for a1 in ['2','3']:
	for a2 in ['2','3']:
		for s1 in ['2','3']:
			for s2 in ['2','3']:
				for  d1 in ['2','3']:
					for d2 in ['2','3']:
						runn='time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 25 -m mvcm -e 011111111 -p 1 -P '+a1+','+a2+','+s1+','+s2+','+d1+','+d2+','+d1+','+d2+','+d1+','+d2+' 1>../output/rlt'+a1+','+a2+','+s1+','+s2+','+d1+','+d2+','+d1+','+d2+','+d1+','+d2
						print(runn)
						os.system(runn)
