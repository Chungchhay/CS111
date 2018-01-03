#!/bin/bash

TARFIELD = lab3a-004843308.tar.gz
FILES = README Makefile lab3a.c ext2_fs.h

.SILENT:

default: 
	gcc lab3a.c -lm -o lab3a -Wall -Wextra

dist: default
	tar -czvf $(TARFIELD) $(FILES)

clean:
	rm -f $(TARFIELD) lab3a
