g++ -std=c++23 -Iinclude src/*.cpp -o simulate
chmod +x simulate
./simulate $1 $2 $3
