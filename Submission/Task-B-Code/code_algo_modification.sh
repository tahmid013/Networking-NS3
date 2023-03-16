#!/bin/sh
cc_file="task_B_algo_modification"
flowmon_file="task_B.flowmonitor"
py_file="parsing.py"
			 # -------------Varying Nodes-------

clear_files () {
rm "throughput_t.txt"
rm "packetDropRatio.txt"
rm "packetDeliveryRatio.txt"
rm "fairness.txt"
rm "endToEndDelay.txt"

	
}

clear_files

vary="nodes"
echo "------------------nodes--------------------\n"

./waf --run "scratch/$cc_file --nCsmar=20"
python3 $py_file $flowmon_file 20

./waf --run "scratch/$cc_file --nCsmar=40"
python3 $py_file $flowmon_file 40

./waf --run "scratch/$cc_file --nCsmar=60"
python3 $py_file $flowmon_file 60


./waf --run "scratch/$cc_file --nCsmar=80"
python3 $py_file $flowmon_file 80

./waf --run "scratch/$cc_file --nCsmar=100"
python3 $py_file $flowmon_file 100



gnuplot -e "vary='${vary}'" plotDelvRatio.plt

gnuplot -e "vary='${vary}'" plotDropRatio.plt 
gnuplot -e "vary='${vary}'" plotThroughput.plt 
gnuplot -e "vary='${vary}'" plotFairness.plt 
gnuplot -e "vary='${vary}'" plotEndToEndDelay.plt 

 #                               -------------Varying Flow-------

vary="flows"
clear_files
echo "------------------flow--------------------\n"


./waf --run "scratch/$cc_file --flow=10"
python3 $py_file $flowmon_file 10 

./waf --run "scratch/$cc_file --flow=20"
python3 $py_file $flowmon_file 20

./waf --run "scratch/$cc_file --flow=30"
python3 $py_file $flowmon_file 30 


./waf --run "scratch/$cc_file --flow=40"
python3 $py_file $flowmon_file 40

./waf --run "scratch/$cc_file --flow=50"
python3 $py_file $flowmon_file 50 



gnuplot -e "vary='${vary}'" plotDelvRatio.plt
gnuplot -e "vary='${vary}'" plotDropRatio.plt 
gnuplot -e "vary='${vary}'" plotThroughput.plt 
gnuplot -e "vary='${vary}'" plotFairness.plt 
gnuplot -e "vary='${vary}'" plotEndToEndDelay.plt 

 #                               -------------Varying pps-------

vary="pps"

clear_files
echo "------------------pps--------------------\n"

./waf --run "scratch/$cc_file --pps=100"
python3 $py_file $flowmon_file 100 

./waf --run "scratch/$cc_file --pps=200"
python3 $py_file $flowmon_file 200

./waf --run "scratch/$cc_file --pps=300"
python3 $py_file $flowmon_file 300 


./waf --run "scratch/$cc_file --pps=400"
python3  $py_file $flowmon_file 400

./waf --run "scratch/$cc_file --pps=500"
python3 $py_file $flowmon_file 500 



gnuplot -e "vary='${vary}'" plotDelvRatio.plt
gnuplot -e "vary='${vary}'" plotDropRatio.plt 
gnuplot -e "vary='${vary}'" plotThroughput.plt 
gnuplot -e "vary='${vary}'" plotFairness.plt 
gnuplot -e "vary='${vary}'" plotEndToEndDelay.plt 
