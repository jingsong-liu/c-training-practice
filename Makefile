HEADERS = sqlite/sqlite3.h

default: bk_server

bk.o: bk.c $(HEADERS)
	gcc -c bk.c -o bk.o
sqlite/sqlite.o: sqlite/sqlite3.c sqlite/shell.c $(HEADERS)
	gcc -c sqlite/sqlite3.c -o sqlite/sqlite.o -lpthread

bk_server: bk.o sqlite/sqlite.o
	gcc bk.o sqlite/sqlite.o -o bk_server
