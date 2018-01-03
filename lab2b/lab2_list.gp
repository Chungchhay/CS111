#! /usr/bin/gnuplot
#
# purpose:
#  generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
# 1. test name
# 2. # threads
# 3. # iterations per thread
# 4. # lists
# 5. # operations performed (threads x iterations x (ins + lookup + delete))
# 6. run time (ns)
# 7. run time per operation (ns)
#
# output:
# lab2b_1.png ... throughput vs number of threads for mutex and spin-lock synchronized list operations.
#   lab2b_2.png ... mean time per mutex wait and mean time per operation for mutex-synchronized list operations.
#   lab2b_3.png ... successful iterations vs threads for each synchronization method.
#   lab2b_4.png ... throughput vs number of threads for mutex synchronized partitioned lists.
#   lab2b_5.png ... throughput vs number of threads for spin-lock-synchronized partitioned lists.
#
# Note:
# Managing data is simplified by keeping all of the results in a single
# file.  But this means that the individual graphing commands have to
# grep to select only the data they want.
#

# general plot parameters
set terminal png
set datafile separator ","

# how many threads/iterations we can run without failure (w/o yielding)
set title "lab2b_1: throughput vs number of threads for mutex and spin-lock"
set xlabel "number of threads"
set logscale x 2
set ylabel "throughput"
set logscale y 10
set output 'lab2b_1.png'

# grep out only single threaded, un-protected, non-yield results
plot \
     "< grep -E \"list-none-m,[0-9]*,1000,1,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
     title 'list: mutex' with linespoints lc rgb 'blue', \
     "< grep -E \"list-none-s,[0-9]*,1000,1,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
     title 'list: spin-lock' with linespoints lc rgb 'red'

set title "lab2b_2: Time per op and Avg lock wait time vs Threads for mutex"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Time"
set logscale y 10
set output 'lab2b_2.png'
# note that unsuccessful runs should have produced no output
plot \
     "< grep -E \"list-none-m,[0-9]*,1000,1,\" lab2b_list.csv" using ($2):($7) \
     title 'time per op' with linespoints lc rgb 'red', \
     "< grep -E \"list-none-m,[0-9]*,1000,1,\" lab2b_list.csv" using ($2):($8) \
     title 'avg wait time for a lock' with linespoints lc rgb 'green'

set title "lab2b_3: successful iterations vs threads"
set xlabel "number of threads"
set logscale x 2
set xrange [0.75:]
set ylabel "successful iterations"
set logscale y 10
set output 'lab2b_3.png'
# note that unsuccessful runs should have produced no output
plot \
     "< grep -E \"list-id-none,[0-9]*,[0-9]*,4,\" lab2b_list.csv" using ($2):($3) \
     title "Unprotected" with points lc rgb "blue", \
    "< grep -E \"list-id-m,[0-9]*,[0-9]*,4,\" lab2b_list.csv" using ($2):($3) \
    title "Mutex" with points lc rgb "red", \
    "< grep -E \"list-id-s,[0-9]*,[0-9]*,4,\" lab2b_list.csv" using ($2):($3) \
    title "Spin-Lock" with points lc rgb "orange"

set title "lab2b_4: throughput vs number of threads for mutex synchronized partitioned lists"
set xlabel "number of threads"
set logscale x 2
set xrange [0.75:]
set ylabel "throughput"
set logscale y 10
set output 'lab2b_4.png'
# note that unsuccessful runs should have produced no output
plot \
     "< grep -E \"list-none-m,[0-9]*,1000,1,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '1 list' with linespoints lc rgb 'green', \
     "< grep -E \"list-none-m,[0-9]*,1000,4,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '4 lists' with linespoints lc rgb 'red', \
     "< grep -E \"list-none-m,[0-9]*,1000,8,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '8 lists' with linespoints lc rgb 'orange', \
     "< grep -E \"list-none-m,[0-9]*,1000,16,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '16 lists' with linespoints lc rgb 'blue'

set title "lab2b_5: throughput vs number of threads for spin-lock-synchronized partitioned lists"
set xlabel "number of threads"
set logscale x 2
set xrange [0.75:]
set ylabel "throughput"
set logscale y 10
set output 'lab2b_5.png'
# note that unsuccessful runs should have produced no output
plot \
     "< grep -E \"list-none-s,[0-9]*,1000,1,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '1 list' with linespoints lc rgb 'red', \
     "< grep -E \"list-none-s,[0-9]*,1000,4,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '4 lists' with linespoints lc rgb 'green', \
     "< grep -E \"list-none-s,[0-9]*,1000,8,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '8 lists' with linespoints lc rgb 'blue', \
     "< grep -E \"list-none-s,[0-9]*,1000,16,\" lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '16 lists' with linespoints lc rgb 'orange'



