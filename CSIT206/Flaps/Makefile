all: flaps

flaps: flaps.o processes/process_details.o tree/process_tree.o
	gcc flaps.o processes/process_details.o tree/process_tree.o -o flaps

flaps.o: flaps.c
	gcc -c flaps.c -o flaps.o

processes/process_details.o: processes/process_details.c
	gcc -c processes/process_details.c -o processes/process_details.o

tree/process_tree.o: tree/process_tree.c
	gcc -c tree/process_tree.c -o tree/process_tree.o