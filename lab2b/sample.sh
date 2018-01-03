#First & Second
./lab2_list --threads=1  --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=2  --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=4  --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=8  --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=12  --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=16  --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=24  --iterations=1000 --sync=m >> lab2b_list.csv

./lab2_list --threads=1  --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=2  --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=4  --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=8  --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=12  --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=16  --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=24  --iterations=1000 --sync=s >> lab2b_list.csv

#Third
./lab2_list --yield=id --lists=4 --threads=1 --iterations=1 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=2 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=4 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=8 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=16 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=1 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=2 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=4 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=8 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=16 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=1 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=2 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=4 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=8 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=16 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=1 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=2 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=4 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=8 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=16 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=1 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=2 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=4 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=8 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=16 >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=1 --iterations=10 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=20 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=40 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=80 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=10 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=20 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=40 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=80 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=10 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=20 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=40 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=80 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=10 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=20 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=40 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=80 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=10 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=20 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=40 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=80 --sync=m >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=1 --iterations=10 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=20 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=40 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=80 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=10 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=20 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=40 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=80 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=10 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=20 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=40 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=80 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=10 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=20 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=40 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=80 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=10 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=20 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=40 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=80 --sync=s >> lab2b_list.csv

#Fourth
./lab2_list --iterations=1000 --threads=1 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=1 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=1 --lists=16 --sync=m >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=2 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=2 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=2 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=2 --lists=16 --sync=m >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=4 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --lists=16 --sync=m >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=8 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=8 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=8 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=8 --lists=16 --sync=m >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=12 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=12 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=12 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=12 --lists=16 --sync=m >> lab2b_list.csv

#fifth
#./lab2_list --iterations=1000 --threads=1 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=1 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=1 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=1 --lists=16 --sync=s >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=2 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=2 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=2 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=2 --lists=16 --sync=s >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=4 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --lists=16 --sync=s >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=8 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=8 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=8 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=8 --lists=16 --sync=s >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=12 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=12 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=12 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=12 --lists=16 --sync=s >> lab2b_list.csv

