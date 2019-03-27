import sys
import subprocess
import os

for k in [1,2,3]:
    variants = [0,3]
    for q in variants:
        data, temp = os.pipe()
        st = "NumThreads,Result,Time,\n"
        
        for i in range(50):
            s = subprocess.check_output("./numaO" + str(q) + " input"+str(k)+".txt",shell=True)
            print("Iteration ",i,"O" + str(q),"input ",k)
            st +=(s.decode("utf-8")) 
    

        f1 = open("O"+str(q)+"_numa_results_input"+str(k)+".csv","w")
            
        for line in st:
            f1.write(line)
        f1.close()


