# build windows
gcc -fPIC -shared ec_point.c math.c -static-libgcc -static-libstdc++ -std=c99 -o3 -o libmath.dll -L../lib/ -lgmp

# build linux
gcc -fPIC -shared ec_point.c math.c -static-libgcc -static-libstdc++ -std=c99 -o3 -o libmath.so -lgmp
