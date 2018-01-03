#!/bin/bash

default:
	gcc -o lab0 -g -Wall -Wextra lab0.c

check: default
	echo "test" > logfile.txt
	@echo "Test 1: wihtout arguments"
	./lab0 < logfile.txt > logfile1.txt
	cmp -s logfile.txt logfile1.txt; \
	if [ $$? -eq 0 ]; then \
		echo "Test 1: without arguments. Successful"; \
	else \
		echo "Test 1: without arguments. Failed"; \
	fi

	@echo "###########################"
	@echo "Test 2: input file to output file"
	./lab0 --input=logfile.txt --output=logfile1.txt
	cmp -s logfile.txt logfile1.txt; \
	if [ $$? -eq 0 ]; then \
		echo "Test 2: input to output. Successful"; \
	else \
		echo "Test 2: input to output. Failed"; \
	fi
	
clean:
	rm -f lab0 *.tar.gz *.o *.txt 

dist:
	tar -cvzf lab0-004843308.tar.gz lab0.c Makefile README backtrace.png breakpoint.png 
