default: bk_server

LD_LIBRARY_PATH=/usr/local/sqlite3/lib

bk.o: bk.c
	gcc -c bk.c -o bk.o
bk_server: bk.o
	gcc bk.o -o bk_server -l$(LD_LIBRARY_PATH)

clean:
	rm *.o
