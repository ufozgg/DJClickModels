for((i=1;i<=1;++i))do
time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 60 -m mvcm -e 011111111 -p 2 1>../output/TM/011111_111_20 2>../output/TM/err20_1x
time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 60 -m mvcm -e 001111111 -p 2 1>../output/TM/001111_111_20 2>../output/TM/err20_2x
done &
for((i=1;i<=1;++i))do
time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 60 -m mvcm -e 000111111 -p 2 1>../output/TM/000111_111_20 2>../output/TM/err20_3x 
time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 60 -m mvcm -e 000011111 -p 2 1>../output/TM/000011_111_20 2>../output/TM/err20_4x 
done &
for((i=1;i<=1;++i))do
time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 60 -m mvcm -e 110011111 -p 2 1>../output/TM/110011_111_20 2>../output/TM/err20_5x 
time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 60 -m mvcm -e 011111011 -p 2 1>../output/TM/011111_011_20 2>../output/TM/err20_1_1x 
done &
for((i=1;i<=1;++i))do
time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 60 -m mvcm -e 001111011 -p 2 1>../output/TM/001111_011_20 2>../output/TM/err20_1_2x 
time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 60 -m mvcm -e 000111011 -p 2 1>../output/TM/000111_011_20 2>../output/TM/err20_1_3x 
done &
for((i=1;i<=1;++i))do
time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 60 -m mvcm -e 000011011 -p 2 1>../output/TM/000011_011_20 2>../output/TM/err20_1_4x 
time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 60 -m mvcm -e 110011011 -p 2 1>../output/TM/110011_011_20 2>../output/TM/err20_1_5x 
done &
for((i=1;i<=1;++i))do
time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 60 -m mvcm -e 010111011 -p 2 1>../output/TM/010111_011_20 2>../output/TM/err20_1
time ./main -l ucf -i ../tmp_data/10_9999TM_train.ucf,../tmp_data/10_9999TM_test.ucf,../tmp_data/10_9999TM_val.ucf -r 60 -m mvcm -e 010011011 -p 2 1>../output/TM/010011_011_20 2>../output/TM/err20_2
done &
