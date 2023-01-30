#!/bin/bash
startTime=`date +%Y%m%d-%H:%M`
startTime_s=$(date +"%s.%N")

p4test ./examples/new-fenpian-cs-ndsc.p4 --p4v 16 --toJSON new-fenpian.json

endTime=`date +%Y%m%d-%H:%M`
endTime_s=$(date +"%s.%N")

function timediff() {
    start_time=$1
    end_time=$2

    start_s=${start_time%.*}
    start_nanos=${start_time#*.}
    end_s=${end_time%.*}
    end_nanos=${end_time#*.}

    if [ "$end_nanos" -lt "$start_nanos" ];then
        end_s=$(( 10#$end_s - 1 ))
        end_nanos=$(( 10#$end_nanos + 10**9 ))
    fi

    time=$(( 10#$end_s - 10#$start_s )).`printf "%03d\n" $(( (10#$end_nanos - 10#$start_nanos)/10**6 ))`

    echo "P4 to JSson cost:" $time "Seconds"
}

timediff  $startTime_s $endTime_s 
