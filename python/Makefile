CC = clang

default:
	$(CC) -shared -o _raypy.so -Wl,--whole-archive ../librayforce.a -Wl,--no-whole-archive raypy_wrap.c -I/usr/include/python3.10

clean:
	rm -f *.so