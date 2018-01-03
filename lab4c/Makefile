#NAME: Chungchhay Kuoch
#EMAIL: chungchhaykuoch@yahoo.com
#ID: 004 843 308

default: 
	gcc -o lab4c_tcp lab4c_tcp.c -lm -lmraa -Wall -Wextra
	gcc -o lab4c_tls lab4c_tls.c -lm -lmraa -lssl -lcrypto 

clean: 
	rm *.tar.gz

dist:
	tar -cvzf lab4c-004843308.tar.gz lab4c_tcp.c lab4c_tls.c README Makefile