main: main.o database.o pre_order_system.o
	clang -o main main.o database.o pre_order_system.o
main.o: main.c database.h pre_order_system.h
	clang -c main.c
database.o: database.c database.h
	clang -c database.c
pre_order_system.o: pre_order_system.c pre_order_system.h database.h
	clang -c pre_order_system.c