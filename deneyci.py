import sys
import subprocess
import os

k=4
data, temp = os.pipe()
st = "Num Threads,Result,Time,"
for i in range(50):
    s = subprocess.check_output("./simdO0 input"+str(k)+".txt",shell=True)
    print("Iteration ",i)
    st +=(s.decode("utf-8")) 
    

f1 = open("O0_simd_results_input"+str(k)+".csv","w")
        
for line in st:
    f1.write(line)
f1.close()


