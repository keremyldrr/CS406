all:
	export OMP_PLACES=sockets;g++ src.cpp -o out3 -fopenmp -mavx -mavx2 -O3 -std=c++11;g++ src.cpp  -o out0  -fopenmp  -O0 -std=c++11
