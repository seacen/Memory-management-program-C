import random

def gen(start,finish,num,name):

  random.seed()
  f=open(name,"w")
  
  for i in range(0,num):
      f.write("{0} {1}\n".format(i+1,random.randint(start,finish)))

  f.close()

  return

gen(20,150,700,"1000s.txt")
gen(100,280,700,"300l.txt")
gen(20,150,700,"300s.txt")
