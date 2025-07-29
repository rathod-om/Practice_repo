P= employee_data



target: build execute clean

build:
	gcc -Wall -g $(P).c -o $(P) -lcjson

execute:
	./$(P) $(ARG) 

clean:
	rm -f *.o

debug:
	gdb ./$(P) $(ARG)
