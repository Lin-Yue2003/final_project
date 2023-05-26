main: main.o database.o pre_order_system.o recommender.o
	clang -o main main.o database.o pre_order_system.o recommender.o
main.o: main.c database.h pre_order_system.h recommender.h
	clang -c main.c
database.o: database.c database.h
	clang -c database.c
pre_order_system.o: pre_order_system.c pre_order_system.h database.h recommender.h
	clang -c pre_order_system.c
recommender.o: recommender.c recommender.h database.h
	clang -c recommender.c