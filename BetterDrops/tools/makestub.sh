LIBNAMETOGEN=mcpelauncher_tinysubstrate
touch nonexistantstub.c
arm-linux-androideabi-gcc -c -Wall -Werror -fpic nonexistantstub.c
arm-linux-androideabi-gcc -shared -nostdlib -o lib$LIBNAMETOGEN.so nonexistantstub.o
mv lib$LIBNAMETOGEN.so jni/
