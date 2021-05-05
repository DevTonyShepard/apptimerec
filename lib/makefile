
apptimerec.a: apptimerec.o utils.o
	ar rcv apptimerec.a apptimerec.o utils.o
utils.a: utils.o
	ar rcv
apptimerec.o: apptimerec.c
	gcc -c apptimerec.c
utils.o: utils.c
	gcc -c utils.c
	
clean:
	rm apptimerec.a *.o ../apptimerec
