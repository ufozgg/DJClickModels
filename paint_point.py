import matplotlib.pyplot as plt 
f=open('../output/vertical_count','r')
AA=[]
BB=[]
w=f.readline()
while w:
    a,b=w.split(' ')
    a=int(a)
    b=float(b)
    AA.append(a)
    BB.append(b)
    w=f.readline()
plt.xlim(xmax=1100,xmin=1)
plt.ylim(ymax=11,ymin=0)
plt.semilogx(AA,BB,'ro',color='blue')
plt.show()