g++ -std=c++23 -Iinclude $(find src -name '*.cpp') -o simulate
chmod +x simulate
./simulate $1 $2 $3
