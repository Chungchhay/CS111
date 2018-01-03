#NAME: Chungchhay Kuoch                                                                 #EMAIL: chungchhay@ucla.edu                                                     
#ID: 004 843 308
 
default:
	gcc -pthread -o lab2_list -g lab2_list.c SortedList.c -Wall -Wextra

tests: default
	chmod 744 sample.sh
	./sample.sh 2>/dev/null

profile: default Makefile 
	LD_PRELOAD=~/CS111/lab2b/gperftools/lib/libprofiler.so.0 CPUPROFILE=raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	~/CS111/lab2b/gperftools/bin/pprof --text ./lab2_list ./raw.gperf > profile.out
	~/CS111/lab2b/gperftools/bin/pprof --list=acquire_lock lab2_list ./raw.gperf >> profile.out

graphs: tests
	chmod 744 lab2_list.gp
	/usr/local/cs/bin/gnuplot lab2_list.gp 2>/dev/null

clean: 
	rm -rf lab2_list *.tar.gz *.gperf

dist: tests profile graphs
	tar -czf lab2b-004843308.tar.gz lab2_list.c SortedList.h SortedList.c Makefile README lab2_list.gp lab2b_list.csv *.png profile.out sample.sh
