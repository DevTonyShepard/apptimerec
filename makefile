all: apptimerec 

apptimerec: main.c
	$(MAKE) -C lib/
	gcc -o apptimerec main.c "lib/apptimerec.a"
	
clean:
	rm apptimerec *.o
