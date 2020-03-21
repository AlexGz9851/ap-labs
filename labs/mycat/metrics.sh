#! /bin/sh
a="$@"

echo "Metrics of mycat:\n" > metrics.txt &&

for i in "$a"; do

    echo "File used: " >> metrics.txt &&
    echo "    $i \n\n" >> metrics.txt &&

    ss1=$(date '+%s')
    su1=$(( $(date '+%N') / 1000 ))
    ./mycat "$i"
    fs1=$(date '+%s')
    fu1=$(( $(date '+%N') / 1000 ))
    d1=$(($(($(( $fs1 - $ss1 )) * 1000000 )) +  $(( $fu1 - $su1 )) ))

    ss2=$(date '+%s')
    su2=$(( $(date '+%N') / 1000 ))
    cat "$i"
    fs2=$(date '+%s')
    fu2=$(( $(date '+%N') / 1000 ))
    d2=$(($(($(( $fs2 - $ss2 )) * 1000000 )) +  $(( $fu2 - $su2 )) ))


    echo "MyCatCommand:" >> metrics.txt &&
    echo "    Start: " >>  metrics.txt && 
    echo "        $ss1.$su1" >>  metrics.txt && 

    echo "    End: " >>  metrics.txt && 
    echo "        $fs1.$fu1" >>  metrics.txt &&
    echo "Duration: $d1 microseconds\n" >> metrics.txt &&



    echo "Cat Command: " >>  metrics.txt && 
    echo "    Start: " >>  metrics.txt && 
    echo "        $ss2.$su2" >>  metrics.txt && 

    echo "    End: " >>  metrics.txt && 
    echo "        $fs2.$fu2" >>  metrics.txt &&
    echo "    Duration: $d2 microseconds\n" >> metrics.txt
    done