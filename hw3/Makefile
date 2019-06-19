naive:
	nvcc naive.cu -o naive -O3 -arch=sm_60  -Xcompiler -O3 -std=c++11  -Xcompiler  -fopenmp -Xcompiler -std=c++11	
coalesced:
	nvcc coalesced.cu -o coalesced -O3 -arch=sm_60  -Xcompiler -O3 -std=c++11  -Xcompiler  -fopenmp -Xcompiler -std=c++11
shared: 
	nvcc only_x_shared.cu -o shared_naive -O3 -arch=sm_60  -Xcompiler -O3 -std=c++11  -Xcompiler  -fopenmp -Xcompiler -std=c++11
shared_coal:
	nvcc shared_coal.cu -o shared_coal -O3 -arch=sm_60  -Xcompiler -O3 -std=c++11  -Xcompiler  -fopenmp -Xcompiler -std=c++11
shared_coal_m:
	nvcc permanent.cu -o permanentGPU -O3 -arch=sm_60  -Xcompiler -O3 -std=c++11  -Xcompiler  -fopenmp -Xcompiler -std=c++11
clean:
	rm naive
	rm coalesced
	rm shared_naive
	rm shared_coal
	rm shared_m_coal
