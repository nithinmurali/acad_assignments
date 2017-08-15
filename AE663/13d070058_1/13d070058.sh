#! /bin/bash

rm final-output.csv

tmp=""
while read line; do
    rol=`echo $line | cut -d " " -f 1`
    m1=`echo $line | cut -d " " -f 2`
    m2='NC'
    mf=$m1
    if [ $m1 -ge 20  ]
    then
        m2=`cat partB.csv | grep $rol | cut -d " " -f 2 `
        mf=$(($m1+$m2))
    fi
    nll=`echo "$rol,$m1,$m2,$mf"`
    tmp="$tmp ; $nll"
done < partA.csv

tmp=`echo $tmp | tr ';' '\n' | sort -t "," -k 4 -nr | tr '\n' ';' `

i=1
for p in $(echo $tmp | sed "s/;/ /g")
do
      m2=`echo $p | cut -d ',' -f 4`
      
      if [ `echo $tmp | tr ';' '\n' | cut -d ',' -f 4 | grep -c -w $m2` -eq 2 ]]
      then
          echo $i,$p,Yes >> final-output.csv
      else
          echo $i,$p,No >> final-output.csv
      fi
      i=$(($i+1))    
done