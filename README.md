# othello
Othello AI

C:\Users\user\Downloads\Mini-Project 3 Package_1\pj
main.exe hw1.exe baseline4.exe

depth 6 + X + C:
1. weight modified: 5000->20000, 2000->8000
b5->great fail

2. weight modified: 5000->20000 when me can hit sumi(stateValue)
        for (int i = 0; i < 4; i++)
        {
            if (spot == sumi[i] && ob.cur_player == me)
                sW = 20000;
        }
b5->win, but didnt hit sumi at the first time
b4->win
b1->win, 4~5cases didn't run through(11, 16, 19...)
b2(reverse)->fail, the others->win

3. weight modified:(solve)
            Point sumi[4] = { {0, 0}, {0, 7}, {7, 0}, {7, 7} };
            for (int i = 0; i < 4; i++)
            {
                if (spot == sumi[i])
                    value *= 2;
            }
b5->draw, hit sumi at the first time



// Ubutun 20.4 - g++ -v #include <climits>
Using built-in specs.
COLLECT_GCC=g++
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/7/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 7.5.0-3ubuntu1~18.04' --with-bugurl=file:///usr/share/doc/gcc-7/README.Bugs --enable-languages=c,ada,c++,go,brig,d,fortran,objc,obj-c++ --prefix=/usr --with-gcc-major-version-only --program-suffix=-7 --program-prefix=x86_64-linux-gnu- --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --enable-bootstrap --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-vtable-verify --enable-libmpx --enable-plugin --enable-default-pie --with-system-zlib --with-target-system-zlib --enable-objc-gc=auto --enable-multiarch --disable-werror --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic --enable-offload-targets=nvptx-none --without-cuda-driver --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu
Thread model: posix
gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04) 

// Windows 10 mingw
