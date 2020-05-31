#Build an SFML C++ project with g++

g++ -c $1.cpp -I/usr/include/ -Wall
g++ $1.o -o $2 -L/usr/include/lib -lsfml-graphics -lsfml-window -lsfml-system
./$2