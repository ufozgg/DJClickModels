import matplotlib.pyplot as plt 
f=open('x','r')
AA=[]
BB=[]
w=f.readline()
while w:
    w=int(w)
    AA.append(len(AA))
    BB.append(w)
    w=f.readline()
#plt.xlim(xmax=1100,xmin=1)
#plt.ylim(ymax=11,ymin=0)
plt.plot(AA,BB,'-',color='blue')
print('Input jpeg name:')
file_name=str(input())+'.jpeg'
plt.savefig(file_name)
