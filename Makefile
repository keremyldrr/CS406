all:
	export OMP_PLACES=sockets;g++ src.cpp -o out3 -mavx -mavx2 -fopenmp  -O3 -std=c++11;g++ src.cpp  -o out0  -fopenmp  -O0 -std=c++11
