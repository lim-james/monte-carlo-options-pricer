cmake -B build -DCMAKE_CXX_COMPILER=g++-15 
cmake --build build
./build/MonteCarloOptionsPricer $1 $2 $3
