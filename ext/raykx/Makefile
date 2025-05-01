CC = clang
TARGET = libraykx.so
CFLAGS = -fPIC -Wall -Wextra -std=c17 -Ofast -march=native -fassociative-math -ftree-vectorize\
 -fno-math-errno -funsafe-math-optimizations -ffinite-math-only -funroll-loops -m64

default:
	$(CC) $(CFLAGS) -shared -fPIC -I../core -o $(TARGET) raykx.c

clean:
	rm -f $(TARGET)
	