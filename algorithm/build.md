# build windows MinGW
gcc -fPIC -shared ec_point.c  keccak256.c secp256k1.c sha256.c sm2p256v1.c sm3.c sm4.c paillier.c -static-libgcc -static-libstdc++ -funroll-loops -finline-functions -std=c99 -O3 -L../lib/win64/ -o libalg.dll -lgmp

# build linux GCC
gcc -fPIC -shared ec_point.c  keccak256.c secp256k1.c sha256.c sm2p256v1.c sm3.c sm4.c paillier.c -static-libgcc -static-libstdc++ -funroll-loops -finline-functions -std=c99 -O3  -L../lib/linux/ -o libalg.so -lgmp 

# build windows static lib
gcc -fPIC -c ec_point.c  keccak256.c secp256k1.c sha256.c sm2p256v1.c sm3.c sm4.c paillier.c -static-libgcc -static-libstdc++ -L../lib/win64 -lgmp  -std=c99 -O3 -funroll-loops -finline-functions
ar -x libgmp.a
ar -rcs libalg-win64.a *.o

# build linux static lib
gcc -c ec_point.c  keccak256.c secp256k1.c sha256.c sm2p256v1.c sm3.c sm4.c paillier.c -static-libgcc -static-libstdc++ -L../lib/linux/ -lgmp  -std=c99 -O3 -funroll-loops -finline-functions
ar -x libgmp.a
ar -rcs libalg-linux.a *.o

# build binary
gcc ec_point.c  keccak256.c secp256k1.c sha256.c sm2p256v1.c sm3.c sm4.c paillier.c test.c -static-libgcc -static-libstdc++ -funroll-loops -finline-functions -std=c99 -O3 -L../lib/win64/ -o test.exe -lgmp
