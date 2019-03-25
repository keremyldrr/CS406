import sys
import subprocess
import os

data, temp = os.pipe()
st = "Num Threads,Result,Time,"
for i in range(50):
    s = subprocess.check_output("./out3 input4.txt", stdin = data, shell = True)
    print("Iteration ",i)
    st +=(s.decode("utf-8")) 


f1 = open("O3_results_input4.csv","w")

for line in st:
    f1.write(line)
f1.close()


