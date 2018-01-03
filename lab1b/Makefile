#Name: Chungchhay Kuoch                                                                         
#Email: chungchhaykuoch@yahoo.com                                                               
#ID: 004 843 308                                                                                  
default: lab1b-server lab1b-client

lab1b-server:
	gcc -lmcrypt -o lab1b-server -g -Wall -Wextra lab1b-server.c

lab1b-client:
	gcc -lmcrypt -o lab1b-client -g -Wall -Wextra lab1b-client.c

clean:
	rm lab1b-server lab1b-client *.tar.gz

dist:
	tar -zcvf lab1b-004843308.tar.gz lab1b-server.c lab1b-client.c my.key Makefile README
