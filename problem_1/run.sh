#!/bin/bash
truncate -s 0 results.txt

gcc -Wall -O3 -o countWords *.c -lpthread

printf "> 10 RUNS 1 THREAD\n" >> results.txt
for n in {1..10}
do
    printf "\n\n> RUN ${n}\n\n" >> results.txt
    ./countWords -n1 ./data/text0.txt ./data/text1.txt ./data/text2.txt ./data/text3.txt ./data/text4.txt >> results.txt
done


printf "\n\n\n> 10 RUNS 2 THREADS\n" >> results.txt
for n in {1..10}
do
    printf "\n\n> RUN ${n}\n\n" >> results.txt
    ./countWords -n2 ./data/text0.txt ./data/text1.txt ./data/text2.txt ./data/text3.txt ./data/text4.txt >> results.txt
done


printf "\n\n\n> 10 RUNS 4 THREADS\n" >> results.txt
for n in {1..10}
do
    printf "\n\n> RUN ${n}\n\n" >> results.txt
    ./countWords -n4 ./data/text0.txt ./data/text1.txt ./data/text2.txt ./data/text3.txt ./data/text4.txt >> results.txt
done


printf "\n\n\n> 10 RUNS 8 THREADS\n" >> results.txt
for n in {1..10}
do
    printf "\n\n> RUN ${n}\n\n" >> results.txt
    ./countWords -n8 ./data/text0.txt ./data/text1.txt ./data/text2.txt ./data/text3.txt ./data/text4.txt >> results.txt
done